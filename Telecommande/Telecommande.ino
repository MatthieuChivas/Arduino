// --------------------------------------------------------------- Client Bluetooth telecommande ----------------------------------------------------------------------------------
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
const String deviceName = "TelecommandeXYZ";
String ServerName = "FourmiXYZ";
bool connected;
// ------------------------------------------------------------- Fin Client Bluetooth telecommande --------------------------------------------------------------------------------

// -------------------------------------------------------------- Déclarations pour les joysticks ---------------------------------------------------------------------------------
#define JSXG_PIN  36 // ESP32 pin GIOP4 (ADC) connected to VRX pin joystick gauche
#define JSYG_PIN  39 // ESP32 pin GIOP2 (ADC) connected to VRY pin joystick gauche
#define JSXD_PIN  2  // ESP32 pin GIOP36 (ADC) connected to VRX pin joystick droit
#define JSYD_PIN  4  // ESP32 pin GIOP39 (ADC) connected to VRY pin joystick droit

#define LEFT_THRESHOLD  1000
#define RIGHT_THRESHOLD 3000
#define UP_THRESHOLD    1000
#define DOWN_THRESHOLD  3000

#define COMMAND_NO     0x00
#define COMMAND_LEFT   0x01
#define COMMAND_RIGHT  0x02
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08

int valXG = 0 ; // valeur X joystick gauche
int valYG = 0 ; // valeur Y joystick gauche
int valXD = 0 ; // valeur X joystick droit
int valYD = 0 ; // valeur Y joystick droit

int commandG = COMMAND_NO;  // Commande joystick gauche
int commandD = COMMAND_NO;  // Commande joystick droit
// -------------------------------------------------------------- Fin déclarations pour les joysticks -----------------------------------------------------------------------------

// -------------------------------------------------------------- Déclarations pour les boutons (interruptions) -------------------------------------------------------------------
#define boutonJaune 5
#define boutonVert 17
#define boutonRouge 16
#define boutonBlanc 18
#define boutonJSG 32
#define boutonJSD 19
bool flagBoutonJaune = 0, flagBoutonVert = 0, flagBoutonRouge = 0, flagBoutonBlanc = 0, flagBoutonJSG = 0, flagBoutonJSD = 0;
bool flagCycleBouton = 0;   // Sert à déterminer si on a déjà lu un bouton lors du cycle, reset à 0 à la fin d'un cycle

// -------------------------------------------------------------- Fin déclarations pour les boutons -------------------------------------------------------------------------------

// ------------------------------------------------------------------------ Déclarations pour les DEL -----------------------------------------------------------------------------
#define delJaune 33
#define delVert 14
#define delRouge 27
#define delBlanc 26
#define delBleu 25
// ---------------------------------------------------------------------- Fin déclarations pour les DEL ---------------------------------------------------------------------------

// ------------------------------------------------------------------------- Déclarations de fonctions ----------------------------------------------------------------------------
int LectureCommandes();
String CommandeBT;          // Variable modifiée par la fonction LectureCommandes selon la commande qu'on veut envoyer par bluetooth. Sert aussi à l'enregistrement de séquence

void IRAM_ATTR interruptBoutonJaune();    // Fonctions d'interruptions actives quand on presse un bouton
void IRAM_ATTR interruptBoutonVert();
void IRAM_ATTR interruptBoutonRouge();
void IRAM_ATTR interruptBoutonBlanc();
void IRAM_ATTR interruptBoutonJSG();
void IRAM_ATTR interruptBoutonJSD();
// ----------------------------------------------------------------------- Fin déclarations de fonctions --------------------------------------------------------------------------

// ----------------------------------------------------------------------------- Variables globales -------------------------------------------------------------------------------
int tableauEnregistrement[1000][2];       // Tableau dans lequel les commandes sont enregistrées

bool flagEnregistrement = 0;              // Lié à l'interruption du bouton rouge
bool flagClearTableauEnregistrement = 0;  // Lié à l'interruption du bouton rouge
int positionEcritureTableauX = 0;           // Compte à quel endroit on est dans le tableau en X pour l'enregistrement

bool flagJouerEnregistrement = 0;         // Lié à l'interruption du bouton blanc
int positionLectureTableauX = 0;    // Compte à quel endroit on est dans le tableau en X pour jouer l'enregistrement
int repetitionCommandes = 0;        // Sert à l'envois de plusieures commandes identiques consécutives
bool flagPremiereCommande = 0;       // Sert à l'envois des commandes enregistrées

// --------------------------------------------------------------------------- Fin variables globales -----------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.println("--- Demarrage de la telecommande ---");

  // Bluetooth
     // Désactiver bluetooth pour les tests, sinon la telecommande essaie de se connecter à la fourmi et le code bloque ici
  SerialBT.begin(deviceName, true);   // Démarrage du bluetooth
  Serial.print("Connexion a " + ServerName);
  while(!connected){                  // Connexion à la fourmi
    connected = SerialBT.connect(ServerName);
    Serial.print(".");
    delay(20);
  }
  Serial.println("Connexion etablie");
  delay(500);
  
  // Fin Bluetooth
  
  // Boutons en interruption
  pinMode(boutonJaune, INPUT_PULLUP);
  pinMode(boutonVert, INPUT_PULLUP);
  pinMode(boutonRouge, INPUT_PULLUP);
  pinMode(boutonBlanc, INPUT_PULLUP);
  pinMode(boutonJSG, INPUT_PULLUP);
  pinMode(boutonJSD, INPUT_PULLUP);
  attachInterrupt(boutonJaune, interruptBoutonJaune, FALLING);
  attachInterrupt(boutonVert, interruptBoutonVert, FALLING);
  attachInterrupt(boutonRouge, interruptBoutonRouge, FALLING);
  attachInterrupt(boutonBlanc, interruptBoutonBlanc, FALLING);
  attachInterrupt(boutonJSG, interruptBoutonJSG, FALLING);
  attachInterrupt(boutonJSD, interruptBoutonJSD, FALLING);
  // Fin boutons en interruption

  // Initialisation DEL
  pinMode(delJaune, OUTPUT);
  pinMode(delVert, OUTPUT);
  pinMode(delRouge, OUTPUT);
  pinMode(delBlanc, OUTPUT);
  pinMode(delBleu, OUTPUT);
  digitalWrite(delJaune, HIGH);   // HIGH = Off, LOW = On
  digitalWrite(delVert, HIGH);
  digitalWrite(delRouge, HIGH);
  digitalWrite(delBlanc, HIGH);
  digitalWrite(delBleu, LOW);   // Allumer la DEL bleu pour indiquer que l'initialisation est complétée
  // Fin initialisation DEL

} // Fin setup()

void loop() {
  
  int commandesInt = LectureCommandes();        // Lire les commandes de la télécommande et formatter les informations dans un int

  if(flagClearTableauEnregistrement == 1){  // Reset les données du tableau à 0, comme ça au prochain flagEnregistrement == 1, il recommence à 0
    for(int x=0; x<1000; x++){                 // Initialiser toutes les données du tableau: 999 en X et 0 en Y
      for(int y=0; y<2; y++){
        if(y == 0){
          tableauEnregistrement[x][y] = 999;
        }
        else{
          tableauEnregistrement[x][y] = 0;
        }
      }
    }
    flagClearTableauEnregistrement = 0;
    positionEcritureTableauX = 0;               // Reset la position dans le tableau d'enregistrement en X    
  }
  
  if(flagEnregistrement == 1){    // Lors de l'enregistrement, chaque commandes lues (fonction LectureCommandes()) sont enregistrées 
    if(tableauEnregistrement[positionEcritureTableauX][0] == 999){  // S'il n'y a aucune commande dans la case du tableau, on peut y écrire notre commande (999 est une case vide)
      tableauEnregistrement[positionEcritureTableauX][0] = commandesInt;  // Écrire la commande dans le tableau
      tableauEnregistrement[positionEcritureTableauX][1] = (tableauEnregistrement[positionEcritureTableauX][1]) + 1;   // Augmenter de 1 la case sous celle de la commande (répétitions)
    }
    else if(tableauEnregistrement[positionEcritureTableauX][0] == commandesInt){     // Si la commande est la même que la dernière qui a été enregistrée
      tableauEnregistrement[positionEcritureTableauX][1] = (tableauEnregistrement[positionEcritureTableauX][1]) + 1;   // Augmenter de 1 la case sous celle de la commande (répétitions)
    }    
    else if(tableauEnregistrement[positionEcritureTableauX][0] != commandesInt){    // Si la commande est différente que la dernière qui a été enregistrée
      positionEcritureTableauX++;   // Aller à la prochaine case en X
      tableauEnregistrement[positionEcritureTableauX][0] = commandesInt;  // Écrire la commande dans le tableau
      tableauEnregistrement[positionEcritureTableauX][1] = (tableauEnregistrement[positionEcritureTableauX][1]) + 1;   // Augmenter de 1 la case sous celle de la commande (répétitions)
    }
  }   // L'enregistrement se termine quand on presse le bouton rouge une deuxième fois (flagEnregistrement==0)

  if(flagJouerEnregistrement == 1){      // Jouer l'enregistrement (si l'enregistrement est arrêté et que le bouton blanc est pressé)
    if(repetitionCommandes == 0 && flagPremiereCommande == 1){        // La première commande envoyée est traitée différemment
      commandesInt = tableauEnregistrement[positionLectureTableauX][0];
      repetitionCommandes = tableauEnregistrement[positionLectureTableauX][1];
      flagPremiereCommande = 0;
    }
    else if(repetitionCommandes == 0 && flagPremiereCommande == 0){        // Quand une commande a été envoyé le bon nombre de fois, on passe à la prochaine
      positionLectureTableauX++;
      commandesInt = tableauEnregistrement[positionLectureTableauX][0];
      repetitionCommandes = tableauEnregistrement[positionLectureTableauX][1];
    }

    if(commandesInt == 999){    // Quand on lis une commande 999, c'est que l'enregistrement est terminé
      commandesInt = 0;   // Envoyer la commande 0 (ne rien faire)
      flagJouerEnregistrement = 0;  // Arrêter de jouer la commande
      digitalWrite(delBlanc, HIGH); // Éteindre la DEL blanche
    }
    
    if(repetitionCommandes > 0){    // Si la commande est la même que la précédente, on réenvoie la commande et on décrémente le compteur de répétition
      commandesInt = tableauEnregistrement[positionLectureTableauX][0];
      repetitionCommandes--;
    }    
  }

  String commandesStr = String(commandesInt);   // Convertir commandesInt en commandesStr  
  SerialBT.print(String(commandesStr + ";"));   //Envoyer les commandes par BT (; à la fin de la commande pour filtrage à la réception)
  Serial.println(commandesInt); // Imprimer la commande envoyée par BT au terminal

  flagCycleBouton = 0;    // Reset le flag pour permettre la lecture d'un bouton
  digitalWrite(delJaune, HIGH);  // Éteindre DEL jaune et verte si elles sont allumées
  digitalWrite(delVert, HIGH);
  
  delay(500);     // Délais d'envois des commandes
} // Fin loop()




// -------------------------------------------------------------------------------- Fonctions -------------------------------------------------------------------------------------
int LectureCommandes(){
  int commandesFormatInt = 0; // Les commandes lues sont formatées dans un int
  //--------------------------------------------------------------- Lecture des joystick 
  // read X and Y analog values
  valXG = analogRead(JSXG_PIN);
  valYG = analogRead(JSYG_PIN);
  valXD = analogRead(JSXD_PIN);
  valYD = analogRead(JSYD_PIN);

  // converts the analog value to commands
  // reset commands
  commandG = COMMAND_NO;
  commandD = COMMAND_NO;

  // Vérification de commande joystick gauche (corps)
  if (valXG < LEFT_THRESHOLD)
    commandG = commandG | COMMAND_LEFT;
  else if (valXG > RIGHT_THRESHOLD)
    commandG = commandG | COMMAND_RIGHT;

  // check up/down commands
  if (valYG < UP_THRESHOLD)
    commandG = commandG | COMMAND_UP;
  else if (valYG > DOWN_THRESHOLD)
    commandG = commandG | COMMAND_DOWN;

  switch (commandG){
    case COMMAND_NO:
      commandesFormatInt = commandesFormatInt + 0;
      break;
    case COMMAND_UP:
      commandesFormatInt = commandesFormatInt + 1;
      break;
    case COMMAND_DOWN:
      commandesFormatInt = commandesFormatInt + 2;
      break;
    case COMMAND_LEFT:
      commandesFormatInt = commandesFormatInt + 3;
      break;
    case COMMAND_RIGHT:
      commandesFormatInt = commandesFormatInt + 4;
      break;
  }

  // Vérification de commande joystick droite (tête)
  if (valXD < LEFT_THRESHOLD)
    commandD = commandD | COMMAND_LEFT;
  else if (valXD > RIGHT_THRESHOLD)
    commandD = commandD | COMMAND_RIGHT;

  // check up/down commands
  if (valYD < UP_THRESHOLD)
    commandD = commandD | COMMAND_UP;
  else if (valYD > DOWN_THRESHOLD)
    commandD = commandD | COMMAND_DOWN;

  switch (commandD){
    case COMMAND_NO:
      commandesFormatInt = commandesFormatInt + 0;
      break;
    case COMMAND_UP:
      commandesFormatInt = commandesFormatInt + 10;
      break;
    case COMMAND_DOWN:
      commandesFormatInt = commandesFormatInt + 20;
      break;
    case COMMAND_LEFT:
      commandesFormatInt = commandesFormatInt + 30;
      break;
    case COMMAND_RIGHT:
      commandesFormatInt = commandesFormatInt + 40;
      break;
  }
  //----------------------------------------------------------------------------- Fin lecture des joystick 

  //----------------------------------------------------------------------------- Lecture des boutons
  if(flagBoutonJaune == 1){
    commandesFormatInt = commandesFormatInt + 100;
    flagBoutonJaune = 0;
  }
  if(flagBoutonVert == 1){
    commandesFormatInt = commandesFormatInt + 200;
    flagBoutonVert = 0;
  }
  if(flagBoutonRouge == 1){
    commandesFormatInt = commandesFormatInt + 300;
    flagBoutonRouge = 0;
  }
  if(flagBoutonBlanc == 1){
    commandesFormatInt = commandesFormatInt + 400;
    flagBoutonBlanc = 0;
  }
  if(flagBoutonJSG == 1){
    commandesFormatInt = commandesFormatInt + 500;
    flagBoutonJSG = 0;
  }
  if(flagBoutonJSD == 1){
    commandesFormatInt = commandesFormatInt + 600;
    flagBoutonJSD = 0;
  }
      // Si aucun flag == 1, alors les centaines restent à 0, un seul flag de bouton peut être == 1 par cycle
  //----------------------------------------------------------------------------- Fin lecture des boutons

  return commandesFormatInt;
}   // Fin LectureCommandes()


    // Fonctions d'interruptions activées quand on presse un bouton (si on presse un bouton, les flags des autres sont reset pour qu'une seule fonction de bouton soit active en même temps)
void IRAM_ATTR interruptBoutonJaune(){
  if(flagCycleBouton == 0){
    flagBoutonJaune = 1;
    flagBoutonVert  = 0;
    flagBoutonRouge = 0;
    flagBoutonBlanc = 0;
    flagBoutonJSG   = 0;
    flagBoutonJSD   = 0;
    flagCycleBouton = 1;
    digitalWrite(delJaune, LOW);  // Allumer la DEL jaune
  }
}
void IRAM_ATTR interruptBoutonVert(){
  if(flagCycleBouton == 0){
    flagBoutonJaune = 0;
    flagBoutonVert  = 1;
    flagBoutonRouge = 0;
    flagBoutonBlanc = 0;
    flagBoutonJSG   = 0;
    flagBoutonJSD   = 0;
    flagCycleBouton = 1;
    digitalWrite(delVert, LOW);  // Allumer la DEL verte
  }
}
void IRAM_ATTR interruptBoutonRouge(){
  if(flagCycleBouton == 0){
    flagBoutonJaune = 0;
    flagBoutonVert  = 0;
    flagBoutonRouge = 1;
    flagBoutonBlanc = 0;
    flagBoutonJSG   = 0;
    flagBoutonJSD   = 0;
    flagCycleBouton = 1;

    if(flagEnregistrement == 0 && flagJouerEnregistrement == 0){    // flagEnregistrement: 1 = enregistrer, 0 = ne pas enregistrer
      flagEnregistrement = 1;
      flagClearTableauEnregistrement = 1;
      digitalWrite(delRouge, LOW);  // Allumer la DEL rouge
    }
    else if(flagEnregistrement == 1){
      flagEnregistrement = 0;
      digitalWrite(delRouge, HIGH);  // Éteindre la DEL rouge
    }
  }
}
void IRAM_ATTR interruptBoutonBlanc(){
  if(flagCycleBouton == 0){
    flagBoutonJaune = 0;
    flagBoutonVert  = 0;
    flagBoutonRouge = 0;
    flagBoutonBlanc = 1;
    flagBoutonJSG   = 0;
    flagBoutonJSD   = 0;
    flagCycleBouton = 1;
  
    if(flagJouerEnregistrement == 0 && flagEnregistrement == 0){    // jouer l'enregistrement si on n'est pas en train d'enregistrer
      flagJouerEnregistrement = 1;
      positionLectureTableauX = 0;    // Reset d'où on commence à lire le tableau (0 est le début)
      repetitionCommandes = 0;
      flagPremiereCommande = 1;       // Indiquer qu'on commence la lecture du tableauEnregistrement
      digitalWrite(delBlanc, LOW);  // Allumer la DEL blanche
    }
    else if(flagJouerEnregistrement == 1){
      flagJouerEnregistrement = 0;
      digitalWrite(delBlanc, HIGH);  // Éteindre la DEL blanche
    }  
  }
}
void IRAM_ATTR interruptBoutonJSG(){    // Les boutons JSG et JSD n'ont pas de fonctions définies, leur interrupts ne font rien
  /*if(flagCycleBouton == 0){
    flagBoutonJaune = 0;
    flagBoutonVert  = 0;
    flagBoutonRouge = 0;
    flagBoutonBlanc = 0;
    flagBoutonJSG   = 1;
    flagBoutonJSD   = 0;
    flagCycleBouton = 1;
  }*/
}
void IRAM_ATTR interruptBoutonJSD(){
  /*if(flagCycleBouton == 0){
    flagBoutonJaune = 0;
    flagBoutonVert  = 0;
    flagBoutonRouge = 0;
    flagBoutonBlanc = 0;
    flagBoutonJSG   = 0;
    flagBoutonJSD   = 1;
    flagCycleBouton = 1;
  }*/
}
// ------------------------------------------------------------------------------ Fin fonctions -----------------------------------------------------------------------------------