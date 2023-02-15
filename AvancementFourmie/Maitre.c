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
int vitesse = 60;

void setup() {
  //initialise console
  Serial.begin(115200);
  Serial.println("---Demarrage de la fourmi---");

  //initialise quel Pin va être utilisé
  Maitre.begin(115200, SERIAL_8N1, SenderTxPin, SenderRxPin);

  // Démarrage du bluetooth
  SerialBT.begin(deviceName);
  
  //Initialise les servomoteurs :
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  Gav1.setPeriodHertz(30);
  Gav2.setPeriodHertz(30);
  Gav3.setPeriodHertz(30);
  
  Dm1.setPeriodHertz(30);
  Dm2.setPeriodHertz(30);
  Dm3.setPeriodHertz(30);
  
  Gar1.setPeriodHertz(30);
  Gar2.setPeriodHertz(30);
  Gar3.setPeriodHertz(30);
  
  Gav1.attach(pinServoGav1, 500, 2400);
  Gav2.attach(pinServoGav2, 500, 2400);
  Gav3.attach(pinServoGav3, 500, 2400);

  Dm1.attach(pinServoDm1, 500, 2400);
  Dm2.attach(pinServoDm2, 500, 2400);
  Dm3.attach(pinServoDm3, 500, 2400);

  Gar1.attach(pinServoGar1, 500, 2400);
  Gar2.attach(pinServoGar2, 500, 2400);
  Gar3.attach(pinServoGav3, 500, 2400);
}



void loop() {
  if (SerialBT.available()) {
    blueetoothSendMsg = SerialBT.readString();
    blueetoothSendMsg.trim();
    Serial.print("Bluetooth : ");
    Serial.println(blueetoothSendMsg);
  }


  if (blueetoothSendMsg == "avancer") {
    //tu codes en else if car l'avancement se fait petit pas par petit pas la boucle utilise le boucle du programme principal
    if (!avancerD) {
      avancerG();
      Serial.println("Avancer Gauche");
    } else {
      Maitre.print("avancer");
      avancerG();
      Serial.println("Avancer Droite et gauche");
    }
  }

  delay(vitesse);
}

void avancerG() {
  moveLegGav();
  moveLegGar();
  moveLegDm();
}

//C'est cette patte qui donne le rythme
void moveLegGav() {
  // Rise the leg
  if (indiceGauch1 <= 10) {
    Gav3.write(60 - indiceGauch1 * 2);
    Gav2.write(90 - indiceGauch1 * 3);
    indiceGauch1++;
  }
  // Rotate the leg
  if (indiceGauch2 <= 30) {
    Gav1.write(100 - indiceGauch2);
    indiceGauch2++;
  }
  // Move back to touch the ground
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Gav3.write(40 + indiceGauch3 * 2);
    Gav2.write(60 + indiceGauch3 * 3);
    indiceGauch3++;
  }
  // Stance phase - move leg while touching the ground
  // Rotate back to initial position
  if (indiceGauch2 >= 30) {
    Gav1.write(70 + indiceGauch4);
    indiceGauch4++;
    //ici il envoie l'information de bouger la droite!!
    avancerD = 1;
  }
  // Reset the counters for repeating the process
  if (indiceGauch4 >= 30) {
    Serial.println("Premier cycle fini");
    indiceGauch1 = 0;
    indiceGauch2 = 0;
    indiceGauch3 = 0;
    indiceGauch4 = 0;
  }
  // Each iteration or step is executed in the main loop section where there is also a delay time for controlling the speed of movement
}

void moveLegGar() {
  //rise leg
  if (indiceGauch1 <= 10) {
    Gar3.write(50 - indiceGauch1 * 2);
    Gar2.write(80 - indiceGauch1 * 3);
  }

  //rotate vers l'avant
  if (indiceGauch2 <= 30) {
    Gar1.write(80 - indiceGauch2);
  }

  //en bas
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Gar3.write(30 + indiceGauch3 * 2);
    Gar2.write(50 + indiceGauch3 * 3);
  }

  //rotate vers l'arrière
  if (indiceGauch2 >= 30) {
    Gar1.write(50 + indiceGauch4);
  }
}

void moveLegDm() {
  //haut
  if (indiceGauch1 <= 10) {
    Dm3.write(80 + indiceGauch1 * 2);
    Dm2.write(50 + indiceGauch1 * 3);
  }

  //rotate ++
  if (indiceGauch2 <= 30) {
    Dm1.write(90 + indiceGauch2);
  }

  //bas
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Dm3.write(100 - indiceGauch3 * 2);
    Dm2.write(80 - indiceGauch3 * 3);
  }

  //rotate --
  if (indiceGauch2 >= 30) {
    Dm1.write(120 - indiceGauch4);
  }
}
