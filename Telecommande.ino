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
bool flagEnregistrement = 0;              // Lié à l'interruption du bouton rouge
bool flagClearTableauEnregistrement = 0;  // Lié à l'interruption du bouton rouge
bool flagFinTableauEnregistrement = 0;    // Lié à l'interruption du bouton rouge
bool flagJouerEnregistrement = 0;         // Lié à l'interruption du bouton blanc
int tableauEnregistrement[1000][2];       // Tableau dans lequel les commandes sont enregistrées
int comptCommandesEnr = 0;                  // Compteur du nombre de commandes enregistrées
int positionTableauX = 0;
// --------------------------------------------------------------------------- Fin variables globales -----------------------------------------------------------------------------

// -------- Variables temporaire pour debug -----------
int nbCycles = 0;
// ------ Fin variables temporaire pour debug --------- 


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
  attachInterrupt(boutonJaune, interruptBoutonJaune, RISING);
  attachInterrupt(boutonVert, interruptBoutonVert, RISING);
  attachInterrupt(boutonRouge, interruptBoutonRouge, RISING);
  attachInterrupt(boutonBlanc, interruptBoutonBlanc, RISING);
  attachInterrupt(boutonJSG, interruptBoutonJSG, RISING);
  attachInterrupt(boutonJSD, interruptBoutonJSD, RISING);
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
  digitalWrite(delBleu, HIGH);
  // Fin initialisation DEL

} // Fin setup()

void loop() {
// Code principal
  int commandesInt = LectureCommandes();
  String commandesStr = String(commandesInt);      // Convertir commandesInt en commandesStr  
  SerialBT.print(String(commandesStr + ";"));   //Envoyer les commandes par BT (; à la fin de la commande pour filtrage à la réception)


  if(flagClearTableauEnregistrement == 1){  // Reset les données du tableau à 0, comme ça au prochain flagEnregistrement == 1, il recommence à 0
    for(int x=0; x<1000; x++){                 // Initialiser toutes les données du tableau à 0
      for(int y=0; y<2; y++){
          tableauEnregistrement[x][y] = 0;
      }
    }
    flagClearTableauEnregistrement = 0;
    comptCommandesEnr = 0;              // Reset le nombre de commandes enregistrées
    positionTableauX = 0;                // Reset la position dans le tableau d'enregistrement en X    
  }
  
  if(flagEnregistrement == 1){
    tableauEnregistrement[positionTableauX][0] = commandesInt; // Enregistrer la commande dans un tableau
    comptCommandesEnr++;
    
  }

  if(flagFinTableauEnregistrement == 1){  // Déclenché à la fin de l'enregistrement, metttre commande 999 à la fin du tableau
      tableauEnregistrement[comptCommandesEnr][0] = 999;
      flagFinTableauEnregistrement = 0;
  }


  if(flagEnregistrement == 0){
    



    
  }




    // Traiter commandesInt selon les DEL à illuminer
      // Si commandesInt = boutonRouge (enregistrement), il faut mettre commandesInt dans un tableau (effacer les données du tableau complet avant écriture, nouvel enregistement écrase l'ancien)
        // Si commandesInt = boutonBlanc (playback), il faut lire les infos du tableau, les convertir en commandesStr



// Fin code principal

// Communication Bluetooth - Terminal
  /*if (Serial.available()) {
    SerialBT.write(Serial.read());   // Envoyer ce qu'on écrit au terminal par bluetooth
  }
  if (SerialBT.available()) {
    Serial.println(SerialBT.readString());  //Écrire ce qu'on reçoit par bluetooth au terminal
  }
 //delay(20);
 */
// Fin communication Bluetooth - Terminal

// Temporaire pour debug
  Serial.print("fin cycle: ");
  Serial.println(nbCycles);
    Serial.println(commandesInt);
  nbCycles++;
// Fin temporaire pour debug  

delay(1000);
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
      // Si aucun flag == 1, alors les centaines restent à 0
  //----------------------------------------------------------------------------- Fin lecture des boutons

  //commandesFormatInt = 123; // Pour test, return toujours la commande 123
  return commandesFormatInt;
}   // Fin LectureCommandes()


    // Fonctions d'interruptions activées quand on presse un bouton (si on presse un bouton, les flags des autres sont reset pour qu'une seule fonction de bouton soit active en même temps)
void IRAM_ATTR interruptBoutonJaune(){
  flagBoutonJaune = 1;
  flagBoutonVert  = 0;
  flagBoutonRouge = 0;
  flagBoutonBlanc = 0;
  flagBoutonJSG   = 0;
  flagBoutonJSD   = 0;
}
void IRAM_ATTR interruptBoutonVert(){
  flagBoutonJaune = 0;
  flagBoutonVert  = 1;
  flagBoutonRouge = 0;
  flagBoutonBlanc = 0;
  flagBoutonJSG   = 0;
  flagBoutonJSD   = 0;
}
void IRAM_ATTR interruptBoutonRouge(){
  flagBoutonJaune = 0;
  flagBoutonVert  = 0;
  flagBoutonRouge = 1;
  flagBoutonBlanc = 0;
  flagBoutonJSG   = 0;
  flagBoutonJSD   = 0;

  if(flagEnregistrement == 0){    // flagEnregistrement: 1 = enregistrer, 0 = ne pas enregistrer
    flagEnregistrement = 1;
    flagClearTableauEnregistrement = 1;
    digitalWrite(delRouge, LOW);  // Allumer la DEL rouge
  }
  else{
    flagEnregistrement = 0;
    flagFinTableauEnregistrement = 1;
    digitalWrite(delRouge, HIGH);  // Éteindre la DEL rouge
  }
}
void IRAM_ATTR interruptBoutonBlanc(){
  flagBoutonJaune = 0;
  flagBoutonVert  = 0;
  flagBoutonRouge = 0;
  flagBoutonBlanc = 1;
  flagBoutonJSG   = 0;
  flagBoutonJSD   = 0;

  if(flagJouerEnregistrement == 0){    // flagJouerEnregistrement: 1 = jouer l'enregistrement, 0 = ne pas jouer l'enregistrement
    flagJouerEnregistrement = 1;
    digitalWrite(delBlanc, LOW);  // Allumer la DEL blanche
  }
  else{
    flagJouerEnregistrement = 0;
    digitalWrite(delBlanc, HIGH);  // Éteindre la DEL blanche
  }  
}
void IRAM_ATTR interruptBoutonJSG(){    // Les boutons JSG et JSD n'on pas de fonctions définis, leur interrupt ne fait rien
  /*flagBoutonJaune = 0;
  flagBoutonVert  = 0;
  flagBoutonRouge = 0;
  flagBoutonBlanc = 0;
  flagBoutonJSG   = 1;
  flagBoutonJSD   = 0;*/
}
void IRAM_ATTR interruptBoutonJSD(){
  /*flagBoutonJaune = 0;
  flagBoutonVert  = 0;
  flagBoutonRouge = 0;
  flagBoutonBlanc = 0;
  flagBoutonJSG   = 0;
  flagBoutonJSD   = 1;*/
}
// ------------------------------------------------------------------------------ Fin fonctions -----------------------------------------------------------------------------------


























/*        // Test pour Interruptions sur GPIO
#define pushButton_pin 16
#define LED_pin 2

void IRAM_ATTR toggleLED()
{
  digitalWrite(LED_pin, !digitalRead(LED_pin));
}

void setup()
{
  pinMode(LED_pin, OUTPUT);
  pinMode(pushButton_pin, INPUT_PULLUP);
  attachInterrupt(pushButton_pin, toggleLED, RISING);
} 
void loop()
{
}
*/




/*        // Test pour Pullup resistor sur GPIO
#define boutonJaune 16
bool varBoutonJaune;

void setup() {
  Serial.begin(115200);
        // gpio 16 bouton pullup actif à 0
    pinMode(boutonJaune, INPUT_PULLUP);

}

void loop() {
    varBoutonJaune = digitalRead(boutonJaune);
    Serial.println(varBoutonJaune);
}
*/