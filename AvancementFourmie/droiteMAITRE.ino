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
int gaucheD = 0;
int droiteD = 0;
int vitesse = 50;

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


//indices pour mouvement tête
int indiceH1 = 0;
int indiceH2 = 0;


int posBaseH1 = 135; // 0 côté vert, max 180, min 60
int posBaseH2 = 120; //0 en haut, max 90 et min 170
int posBaseH3 = 50; // pblm engrenage; max 100 min 0


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


  blueetoothSendMsg = "droite";      // Pour les tests, on fait comme si la fourmi recevoit toujours la commande "gauche"

  if (blueetoothSendMsg == "droite") {
    //tu codes en else if car l'avancement se fait petit pas par petit pas la boucle utilise le boucle du programme principal
    if (!droiteD) { //le booleen = 0, donc on commence avec la fonction gauche d'abord
      droiteG();
      Serial.println("Tourner a droite Droite");
    } else { // le booleen est maintenant = 1; donc on envoie la commande a l'esclave et on continue notre fonction

      Maitre.print("droite;");     // Envoyer commande avec ; à la fin (caractère d'arrêt)

      droiteG();
      Serial.println("Tourner a droite");
    }
  }

  delay(vitesse);
}


void droiteG() {

  moveLegGav();
  moveDLegDm();  
  moveLegGar();

}

void moveLegGav() { //DOUBLON
  // Rise the leg
  if (indiceGauch1 <= 10) {
    Gav3.write(posBaseGav3 + (indiceGauch1 * 2));
    Gav2.write(posBaseGav2 + (indiceGauch1 * 3));
    indiceGauch1++;
  }
  // Rotate the leg
  if (indiceGauch2 <= 30) {
    Gav1.write(posBaseGav1 + indiceGauch2); //on rajoute des degrés à la pos in 30=>60
    indiceGauch2++;
  }
  // Move back to touch the ground
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Gav3.write(posBaseGav3+20 - (indiceGauch3 * 2));
    Gav2.write(posBaseGav2+30 - (indiceGauch3 * 3));
    indiceGauch3++;
  }
  // Stance phase - move leg while touching the ground
  // Rotate back to initial position
  if (indiceGauch2 >= 30) {
    Gav1.write((posBaseGav1 + 30) - indiceGauch4);  //Gav1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
    indiceGauch4++;
    //ici il envoie l'information de bouger la partie droite!!
    droiteD = 1;
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


void moveDLegDm() { // CHANGEE
  //haut
  if (indiceGauch1 <= 10) {
    Dm3.write(posBaseDm3 - (indiceGauch1 * 2));
    Dm2.write(posBaseDm2 - (indiceGauch1 * 3));
  }

  //rotate ++
  if (indiceGauch2 <= 30) {
    Dm1.write(posBaseDm1 + indiceGauch2);

  }

  //bas
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Dm3.write(posBaseDm3-20 + (indiceGauch3 * 2));
    Dm2.write(posBaseDm2-30 + (indiceGauch3 * 3));
    Serial.println("bas D");
  }

  //rotate --
  if (indiceGauch2 >= 30) {
    Dm1.write((posBaseDm1 + 30) - indiceGauch4); //Dm1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
  }
}


void moveLegGar() { //DOUBLON
  //rise leg
  if (indiceGauch1 <= 10) {
    Gar3.write(posBaseGar3 + (indiceGauch1 * 2));
    Gar2.write(posBaseGar2 + (indiceGauch1 * 3));
  }

  //rotate vers l'avant
  if (indiceGauch2 <= 30) {
    Gar1.write(posBaseGar1 + indiceGauch2); //ajouter des degrés à la pos in
  }

  //en bas
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Gar3.write(posBaseGar3+20 - (indiceGauch3 * 2));
    Gar2.write(posBaseGar2+30 - (indiceGauch3 * 3));
    Serial.println("bas G");
  }

  //rotate vers l'arrière
  if (indiceGauch2 >= 30) {
    Gar1.write((posBaseGar1 + 30) - indiceGauch4); //Gar1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
  } //on retire des degrés à la pos in
}
