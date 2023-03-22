//Serveur Bluetooth Fourmi
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
const String deviceName = "Fourmi";
String blueetoothSendMsg;

//setup le port 2 comme maitre (envoyer des informations)
HardwareSerial Maitre(2);

//--SERVOO--
Servo Gav1;
Servo Gav2;
Servo Gav3;

Servo Dm1;
Servo Dm2;
Servo Dm3;

Servo Gar1;
Servo Gar2;
Servo Gar3;

Servo H1;
Servo H2;
Servo H3;

//Mise en place servo moteur GAV/GARR/DM1
#define pinServoGav1 4
#define pinServoGav2 16
#define pinServoGav3 17

#define pinServoDm1 25
#define pinServoDm2 33
#define pinServoDm3 32

#define pinServoGar1 5
#define pinServoGar2 18
#define pinServoGar3 19


//Mise en place servo moteur pour la tête
#define pinServoH1 2
#define pinServoH2 15
#define pinServoH3 13

//define les Pin qu'on va utilier pour transmettre des informations
//Tx : transmetteur doit être branché sur un receveur de l'autre ESP32
//Rx : receveur doit être branché sur un transmetteur de l'autre ESP32
#define SenderTxPin 22
#define SenderRxPin 23

//indice pour avancement des pattes gauches
int indiceGauch1 = 0;
int indiceGauch2 = 0;
int indiceGauch3 = 0;
int indiceGauch4 = 0;

int avancerD = 0;
int vitesse = 90;

int test = 0;
int posBaseGav1 = 110;
int posBaseGav2 = 145;
int posBaseGav3 = 165;

int posBaseDm1 = 95;
int posBaseDm2 = 35;
int posBaseDm3 = 15;

int posBaseGar1 = 85;
int posBaseGar2 = 145;
int posBaseGar3 = 165;


//indice pour mouvement Mandibules
int indiceM = 0;

int posBaseH1 = 135; // 0 côté vert, max 180, min 60 => gauche droite
int posBaseH2 = 120; //0 en haut, max 90 et min 170 => haut bas
int posBaseH3 = 50; // max 100, ouvertes min 0 fermées


//-------------------------Variables Temporaires ci-dessous------------------------



void setup() {
  //initialise console
  Serial.begin(115200);
  Serial.println("---Demarrage de la fourmi---");

  //initialise quel Pin va être utilisé
  Maitre.begin(115200, SERIAL_8N1, SenderTxPin, SenderRxPin);

  // Démarrage du bluetooth
  // SerialBT.begin(deviceName);

  //Initialise les servomoteurs :
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  Gav1.setPeriodHertz(50);
  Gav2.setPeriodHertz(50);
  Gav3.setPeriodHertz(50);

  Dm1.setPeriodHertz(50);
  Dm2.setPeriodHertz(50);
  Dm3.setPeriodHertz(50);

  Gar1.setPeriodHertz(50);
  Gar2.setPeriodHertz(50);
  Gar3.setPeriodHertz(50);

  H1.setPeriodHertz(50);
  H2.setPeriodHertz(50);
  H3.setPeriodHertz(50);

  Gav1.attach(pinServoGav1, 500, 2400);
  Gav2.attach(pinServoGav2, 500, 2400);
  Gav3.attach(pinServoGav3, 500, 2400);

  Dm1.attach(pinServoDm1, 500, 2400);
  Dm2.attach(pinServoDm2, 500, 2400);
  Dm3.attach(pinServoDm3, 500, 2400);

  Gar1.attach(pinServoGar1, 500, 2400);
  Gar2.attach(pinServoGar2, 500, 2400);
  Gar3.attach(pinServoGar3, 500, 2400);

  H1.attach(pinServoH1, 500, 2400);
  H2.attach(pinServoH2, 500, 2400);
  H3.attach(pinServoH3, 500, 2400);

  //-------------------position de base des pattes au démarrage--------------------
  Gav1.write(posBaseGav1);
  Gav2.write(posBaseGav2);
  Gav3.write(posBaseGav3);

  Dm1.write(posBaseDm1);
  Dm2.write(posBaseDm2);
  Dm3.write(posBaseDm3);

  Gar1.write(posBaseGar1);
  Gar2.write(posBaseGar2);
  Gar3.write(posBaseGar3);

  H1.write(posBaseH1);
  H2.write(posBaseH2);
  H3.write(posBaseH3);


  Serial.println("Starting Loop in 3 sec");
  delay(3000);
}

void loop() {
  /*
    if (SerialBT.available()) {
      blueetoothSendMsg = SerialBT.readString();
      blueetoothSendMsg.trim();
      Serial.print("Bluetooth : ");
      Serial.println(blueetoothSendMsg);
    }
  */


  blueetoothSendMsg = "Mandibules";    //Pour les tests, on fait comme si la fourmi recevait toujours la commande "lève la tête"
  //Serial.println(blueetoothSendMsg = "HeadUP");


  if (blueetoothSendMsg == "HeadUP") {
    HeadUP();
    SerialBT.println("Tête haute");
  }
  if (blueetoothSendMsg == "HeadDOWN") {
    HeadDOWN();
    SerialBT.println("Tête basse");
  }
  if (blueetoothSendMsg == "HeadR") {
    HeadR();
    SerialBT.println("Tête droite");
  }
  if (blueetoothSendMsg == "HeadL") {
    HeadL();
    SerialBT.println("Tête gauche");
  }
  if (blueetoothSendMsg == "Mandibules") {
    Mandibules();
    SerialBT.println("Mandibules");
  }
  delay(vitesse);
}

void HeadUP() {
  if (posBaseH2 >= 90) {
    H2.write(posBaseH2);
    Serial.println(posBaseH2);
    posBaseH2--;
  }
  else {
    H2.write(90);
    Serial.println(posBaseH2);
  }
}

void HeadDOWN() {
  if (posBaseH2 <= 170) {
    H2.write(posBaseH2);
    Serial.println(posBaseH2);
    posBaseH2++;
  }
  else {
    H2.write(170);
    Serial.println(posBaseH2);
  }
}

void HeadR() {
  if (posBaseH1 <= 180) {
    H1.write(posBaseH1);
    Serial.println(posBaseH1);
    posBaseH1++;
  }
  else {
    H1.write(180);
    Serial.println(posBaseH1);
  }
}

void HeadL() {
  if (posBaseH1 >= 60) {
    H1.write(posBaseH1);
    Serial.println(posBaseH1);
    posBaseH1--;
  }
  else {
    H1.write(60);
    Serial.println(posBaseH1);
  }
}

void Mandibules() {
  for (indiceM = 0; indiceM <= 50; indiceM++) {
    H3.write(posBaseH3 ++);
    Serial.println(posBaseH3);
    delay(10);
  }
  for (indiceM = 0; indiceM <= 50; indiceM++) {
    H3.write(posBaseH3 --);
    Serial.println(posBaseH3);
    delay(10);
  }
}
