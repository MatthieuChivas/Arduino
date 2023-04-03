/*
  On envoie des commandes avec le terminal BT du le 1er ESP32, connecté en BT avec le 2e ESP32. Celui-ci représente
  la fourmi. Le 1er ESP32 enregistre les commandes envoyées dans le tableau. Il doit ensuite aller les rechercher
  et les envoyer au 2e ESP32 lorsqu'on lui envoie la commande "exécute". Il faut vérifier que le 2e reçoit les
  commandes


  => plutôt mettre un bouton record plutôt que une commande
*/



#include "BluetoothSerial.h"

String commande;
//int nbEnvois = 0;
byte mat[1000][2]; //tableau de stockage des positions
int i = 0, j;
int enregistrer, sequence;
byte compteur = 0, k;
String avancer, gauche, droite, reculer, arreter;

BluetoothSerial SerialBT;
const String deviceName = "Telecommande";
String ServerName = "Fourmi1";
bool connecte;

//on place une led sur D13
const byte LedPin = 13;
//bouton Record sur pin D0
const byte RecordPin = 2; 
//bouton Exécuter sur pin D1
const byte EXPin = 3; 

//On appelle 4 Pins pour les boutons avancer, reculer, droite et gauche
const byte AvPin = 18;
const byte ArPin = 15; 
const byte DrPin = 16;
const byte GaPin = 17;









void setup() {

  Serial.begin(115200);/*
  Serial.println("---Demarrage de la telecommande ---");
  SerialBT.begin(deviceName, true); //Bluetooth demarrage
  Serial.println("Pret a la connexion en BT à " + ServerName);


  while (!connecte)  // Connexion à la fourmi
  {
    connecte = SerialBT.connect(ServerName);
    Serial.print(".");
    delay(20);
  }
  Serial.println("Connexion etablie");
  delay(500);*/

  pinMode(AvPin, INPUT); // init de mon bouton
}

void loop() {

/*
  if (Serial.available()) {
    SerialBT.write(Serial.read());   // Envoyer ce qu'on écrit au terminal par bluetooth
  }
  if (SerialBT.available()) {
    Serial.println(SerialBT.readString());  //Écrire ce qu'on reçoit par bluetooth au terminal
  }
  delay(20);*/


  

  /*// Lecture des entrées 

  if(nbEnvois <= 5){
    commande = "avancer";
    Serial.println(commande);
    nbEnvois++;
  }
  if(nbEnvois > 5 && nbEnvois < 10){
    commande = "reculer";
    Serial.println(commande);
    nbEnvois++;
  }
  if(nbEnvois > 9 && nbEnvois < 15){
    commande = "gauche";
    Serial.println(commande);
    nbEnvois++;
  }*/
      
  // Envoyer par BT commande toute les 500ms
  //Algorithme avec boutons : avancer, reculer, gauche et droite = commande 

  if (digitalRead(AvPin) == HIGH){
    commande = "avancer";
  }
 if (digitalRead(ArPin) == LOW){
    commande = "reculer";
  }
 if (digitalRead(DrPin) == LOW){
    commande = "droite";
  }
   if (digitalRead(GaPin) == LOW){
    commande = "gauche";
  }

  //Détécter l'utilisation d'un bouton et enregistrer les données dans le tableau
  //On appuie sur le bouton enregistrer sur la télécommande
  while (digitalRead(RecordPin) == LOW) { 

    //On allume la Led
    digitalWrite(LedPin, HIGH); 

    //Si on pousse le joystick pour avancer, on envoie cette commande à la fourmi
    if (commande == "avancer") { 
      //On ajoute 1 au compteur de répétition de la commande
      compteur = 1; 
      while (commande == "avancer") {
        // On ajoute 1 au compteur de répétition de la commande
        compteur++;
        commande = "avancer";     //utile de faire ça? car commande déjà = avancer dans le while 
        commande = Serial.read();
        SerialBT.write(Serial.read()); 
        }

      //On rentre la valeur associée à la commande dans la 1ère ligne du tableau
      mat[i][0] = 1; 
      // On rentre la valeur du compteur associée à la commande dans le tableau
      mat[i][1] = compteur;
      Serial.println(mat[i][compteur]);
      // On se déplace d'une colonne vers la gauche
      i++; 
      }
    

    //Si on pousse le joystick pour reculer
    if (commande == "reculer") { 
      compteur = 1;
      while (commande == "reculer") {
        compteur++;
        commande = "reculer";
        commande = Serial.read();
        SerialBT.write((const uint8_t*)"commande", 0);
      }
      mat[i][0] = 2;
      mat[i][1] = compteur;
      //Serial.println(mat[i][compteur]);
      i++;

    }

    //Si on pousse le joystick pour tourner à droite
    if (commande == "droite") { 
      compteur = 1;
      while (commande == "droite") {
          compteur++;
          commande = Serial.read();
          SerialBT.write((const uint8_t*)"commande", 0);
      }
      mat[i][0] = 3;
      mat[i][1] = compteur;
      //Serial.println(mat[i][compteur]);
      i++;

    }
    //Si on pousse le joystick pour tourner à gauche
    if (commande == "gauche") { 
      compteur = 1;
      while (commande == "gauche") {
          compteur++;
          commande = Serial.read();
          SerialBT.write((const uint8_t*)"commande", 0);
        
      }
      mat[i][0] = 4;
      mat[i][1] = compteur;
      //Serial.println(mat[i][compteur]);
      i++;

    }
  }
  digitalWrite(LedPin, LOW); //On éteint la Led


  //Fonction pour lire les données du tableau


  //Si le bouton Exécuter est appuyé
  if (digitalRead(EXPin) == LOW) {
    //Serial.println("Execute"); //On pourrait ici aussi allumer la LED pour montrer qu'on exécute le programme
    for (j = 0; j < i; j++) {
      if (mat[j][0] == 1) {
        for (k = 0; k <= mat[j][1]; k++) {
          // Envoyer à la fourmi  "avancer"
          //Ici on utilise le print pour pouvoir checker le code sans utiliser le BT
          Serial.println("La fourmie devrait avancer avec le bluetooth");
          delay (500);
        }
      }
      if (mat[j][0] == 2) {
        for (k = 0; k <= mat[j][1]; k++) {
          // Envoyer à la fourmi  "reculer" : SerialBT.write("avancer")
          Serial.println("La fourmie devrait reculer avec le bluetooth");
          delay (500);
        }
      }
      if (mat[j][0] == 3) {
        for (k = 0; k <= mat[j][1]; k++) {
          // Envoyer à la fourmi  "droite"
          Serial.println("La fourmi devrait aller à droite avec le bluetooth");
          //SerialBT.write((const uint8_t*)"droite", 0); 
          delay (500);
        }
      }
      if (mat[j][0] == 4) {
        for (k = 0; k <= mat[j][1]; k++) {
          // Envoyer à la fourmi  "gauche"
          Serial.println("La fourmi devrait aller à gauche avec le bluetooth");
          //SerialBT.write((const uint8_t*)"gauche", 0); 
          delay (500);
        }
      }
    }


  }
delay(500);
}
