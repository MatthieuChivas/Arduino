//Serveur Bluetooth Fourmi
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
const String deviceName = "FourmiLOL";
String blueetoothSendMsgStr;
char blueetoothSendMsgChar[4];
int blueetoothSendMsg;


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
int reculerD = 0;

int vitesse = 50;


int test=0;

int posBaseGav1=110;
int posBaseGav2=145;
int posBaseGav3=165;
  
int posBaseDm1=95;
int posBaseDm2=35;
int posBaseDm3=15;

int posBaseGar1=85;
int posBaseGar2=145;
int posBaseGar3=165;

//indice pour mouvement Mandibules
int indiceM = 0;


//indices pour position tete
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
  SerialBT.begin(deviceName);
  
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
    //---------------- Reception Bluetooth -----------
  if (SerialBT.available()) {
    blueetoothSendMsgStr = SerialBT.readStringUntil(';');
    blueetoothSendMsgStr.toCharArray(blueetoothSendMsgChar, 4);
    //blueetoothSendMsg.trim();
    SerialBT.print("Bluetooth : ");
    SerialBT.println(blueetoothSendMsgStr);
    blueetoothSendMsg = atoi(blueetoothSendMsgChar);
    
    if (blueetoothSendMsg!=0){
      SerialBT.println("Conversion OK");
    }
    else {  
      SerialBT.println("Conversion Pas OK :(");
    }
  }

    //------------- Mouvement du corps --------------
//Avancer
  if (traduireUnite(blueetoothSendMsg) == 1) {
    if (!avancerD) {
      avancerG();
    } else {      
      Maitre.print("avancer;");
      SerialBT.println("Envoi info autre servo");   
      avancerG();
    }
  }

//Reculer
  if (traduireUnite(blueetoothSendMsg) == 2){

    if (!reculerD) { 
      reculerG();
    } else {
      Maitre.print("reculer;");    
      reculerG();
    }
  }
//Droite
  if (traduireUnite(blueetoothSendMsg) == 4){
    if (!droiteD) { 
      droiteG();
    } else { 
      Maitre.print("droite;");     
      droiteG();
    }
  }
//Gauche
  if (traduireUnite(blueetoothSendMsg) == 3){
    if (!gaucheD) { 
      gaucheG();
    } else { 
      Maitre.print("gauche;");     
      gaucheG();
    }
  }
  
    //-------------- Tête --------------------
//Haute
  if (traduireDizaine(blueetoothSendMsg) == 1) {
    HeadUP();
    SerialBT.println("Tête haute");
  }
//Basse
  if (traduireDizaine(blueetoothSendMsg) == 2) {
    HeadDOWN();
    SerialBT.println("Tête basse");
  }
//Droite
  if (traduireDizaine(blueetoothSendMsg) == 4) {
    HeadR();
    SerialBT.println("Tête droite");
  }
//Gauche
  if (traduireDizaine(blueetoothSendMsg) == 3) {
    HeadL();
    SerialBT.println("Tête gauche");
  }
//Pour l'instant je touche pas aux mandibules car normalement il y a deux fonctions fermé et ouverte?
  if (traduireDizaine(blueetoothSendMsg) == 5) {
    Mandibules();
    SerialBT.println("Mandibules");
  }
  

    //---------- Fonctionnalité ------------
    if(traduireCentaine(blueetoothSendMsg) == 1){
        SerialBT.println("Jauuuuune");
    }
    if(traduireCentaine(blueetoothSendMsg) == 2){
        SerialBT.println("Vert");
    }
    if(traduireCentaine(blueetoothSendMsg) == 3){
        SerialBT.println("Rouge");
    }
    if(traduireCentaine(blueetoothSendMsg) == 4){
        SerialBT.println("Blanc");
    }
  
  delay(vitesse);
}

//---- CODE pour tourner à gauche des pattes "gauches" ----//
void gaucheG(){
  moveGLegGav();
  moveLegDm();
  moveGLegGar();

}

//---- CODE pour tourner à droite des pattes "gauches" ----//
void droiteG(){
  moveLegGav();
  moveDLegDm();  
  moveLegGar();
}


//---- CODE pour reculer des pattes "gauches" -----//
void reculerG(){
  moveRevLegGav();
  moveRevLegDm();
  moveRevLegGar();

}



// ----- CODE pour avancer des pates "gauches" ----//
void avancerG() {
  
  moveLegGav();
  moveLegDm();
  moveLegGar();
  
}

//C'est cette patte qui donne le rythme
void moveLegGav() {
  // Rise the leg
  if (indiceGauch1 <= 10) {
    Gav3.write(posBaseGav3 + (indiceGauch1*2));
    Gav2.write(posBaseGav2 + (indiceGauch1*3));
    indiceGauch1++;
  }
  // Rotate the leg
  if (indiceGauch2 <= 30) {
    Gav1.write(posBaseGav1 + indiceGauch2);
    indiceGauch2++;
  }
  // Move back to touch the ground
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Gav3.write(posBaseGav3+20 - (indiceGauch3*2));
    Gav2.write(posBaseGav2+30 - (indiceGauch3*3));
    indiceGauch3++;
  }
  // Stance phase - move leg while touching the ground
  // Rotate back to initial position
  if (indiceGauch2 >= 30) {
    Gav1.write((posBaseGav1+30) - indiceGauch4);    //Gav1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
    indiceGauch4++;
    //ici il envoie l'information de bouger la droite!!
    avancerD = 1;
    droiteD = 1;
  }
  
  // Reset the counters for repeating the process
  if (indiceGauch4 >= 30) {
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
    Gar3.write(posBaseGar3 + (indiceGauch1*2));
    Gar2.write(posBaseGar2 + (indiceGauch1*3));
  }

  //rotate vers l'avant
  if (indiceGauch2 <= 30) {
    Gar1.write(posBaseGar1 + indiceGauch2);
  }

  //en bas
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Gar3.write(posBaseGar3+20 - (indiceGauch3*2));
    Gar2.write(posBaseGar2+30 - (indiceGauch3*3));
  }

  //rotate vers l'arrière
  if (indiceGauch2 >= 30) {
    Gar1.write((posBaseGar1+30) - indiceGauch4);  //Gar1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
  }
}

void moveLegDm() {
  //haut
  if (indiceGauch1 <= 10) {
    Dm3.write(posBaseDm3 - (indiceGauch1*2));
    Dm2.write(posBaseDm2 - (indiceGauch1*3));
  }

  //rotate ++
  if (indiceGauch2 <= 30) {
    Dm1.write(posBaseDm1 - indiceGauch2);

  }

  //bas
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Dm3.write(posBaseDm3-20 + (indiceGauch3*2));
    Dm2.write(posBaseDm2-30 + (indiceGauch3*3));
  }

  //rotate --
  if (indiceGauch2 >= 30) {
    Dm1.write((posBaseDm1-30) + indiceGauch4);  //Dm1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
  }
}

void moveGLegGav() {
  // Rise the leg
  if (indiceGauch1 <= 10) {
    Gav3.write(posBaseGav3 + (indiceGauch1 * 2));
    Gav2.write(posBaseGav2 + (indiceGauch1 * 3));
    indiceGauch1++;
  }
  // Rotate the leg
  if (indiceGauch2 <= 30) {
    Gav1.write(posBaseGav1 - indiceGauch2); //on rajoute des degrés à la pos in 30=>60
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
    Gav1.write((posBaseGav1 - 30) + indiceGauch4);  //Gav1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
    indiceGauch4++;
    //ici il envoie l'information de bouger la partie droite!!
    gaucheD = 1;
    reculerD = 1;
  }

  // Reset the counters for repeating the process
  if (indiceGauch4 >= 30) {
    indiceGauch1 = 0;
    indiceGauch2 = 0;
    indiceGauch3 = 0;
    indiceGauch4 = 0;
  }
  // Each iteration or step is executed in the main loop section where there is also a delay time for controlling the speed of movement
}

void moveGLegGar() {
  //rise leg
  if (indiceGauch1 <= 10) {
    Gar3.write(posBaseGar3 + (indiceGauch1 * 2));
    Gar2.write(posBaseGar2 + (indiceGauch1 * 3));
  }
  //rotate vers l'avant

  if (indiceGauch2 <= 30) {
    Gar1.write(posBaseGar1 - indiceGauch2); //ajouter des degrés à la pos in
  }

  //en bas
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Gar3.write(posBaseGar3+20 - (indiceGauch3 * 2));
    Gar2.write(posBaseGar2+30 - (indiceGauch3 * 3));
  }

  //rotate vers l'arrière
  if (indiceGauch2 >= 30) {
    Gar1.write((posBaseGar1 - 30) + indiceGauch4); //Gar1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
  } //on retire des degrés à la pos in
}

void moveDLegDm() {
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
  }

  //rotate --
  if (indiceGauch2 >= 30) {
    Dm1.write((posBaseDm1 + 30) - indiceGauch4); //Dm1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
  }
}


void moveRevLegGav() {
  // Rise the leg
  if (indiceGauch1 <= 10) {
    Gav3.write(posBaseGav3 + (indiceGauch1 * 2));
    Gav2.write(posBaseGav2 + (indiceGauch1 * 3));
    indiceGauch1++;
  }
  // Rotate the leg
  if (indiceGauch2 <= 30) {
    Gav1.write((posBaseGav1 + 15) - indiceGauch2); //on avance la patte pour le mouvement
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
    Gav1.write((posBaseGav1 - 15) + indiceGauch4);  //Gav1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
    indiceGauch4++;
    //ici il envoie l'information de bouger la partie droite!!
    reculerD = 1;
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


void moveRevLegDm() { 
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


void moveRevLegGar() {
  //rise leg
  if (indiceGauch1 <= 10) {
    Gar3.write(posBaseGar3 + (indiceGauch1 * 2));
    Gar2.write(posBaseGar2 + (indiceGauch1 * 3));
  }
  //rotate vers l'avant

  if (indiceGauch2 <= 30) {
    Gar1.write((posBaseGar1 + 15) - indiceGauch2); //ajouter des degrés à la pos in
  }

  //en bas
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) {
    Gar3.write(posBaseGar3+20 - (indiceGauch3 * 2));
    Gar2.write(posBaseGar2+30 - (indiceGauch3 * 3));
    Serial.println("bas G");
  }

  //rotate vers l'arrière
  if (indiceGauch2 >= 30) {
    Gar1.write((posBaseGar1 - 15) + indiceGauch4); //Gar1 est à sa (posBase+30) pcq elle est à la position avancée. On la rétracte vers sa posBase (-30)
  } //on retire des degrés à la pos in
}


//Tete
void HeadUP() {
  if (posBaseH2 >= 90) {
    H2.write(posBaseH2);
    posBaseH2--;
  }
  else {
    H2.write(90);
  }
}

void HeadDOWN() {
  if (posBaseH2 <= 170) {
    H2.write(posBaseH2);
    posBaseH2++;
  }
  else {
    H2.write(170);
  }
}

void HeadR() {
  if (posBaseH1 <= 180) {
    H1.write(posBaseH1);
    posBaseH1++;
  }
  else {
    H1.write(180);
  }
}

void HeadL() {
  if (posBaseH1 >= 60) {
    H1.write(posBaseH1);
    posBaseH1--;
  }
  else {
    H1.write(60);
  }
}

void Mandibules() {
  if (indiceM <= 100) {
    H3.write(posBaseH3 ++);
    indiceM ++;
  }
  
  if ((100 <= indiceM) && (indiceM <= 200)) {
    H3.write(posBaseH3 --);
    indiceM++;
  }
  
  if (indiceM == 200){
    indiceM = 0;
  }
}

//Boutton
int traduireCentaine(int numberCommand){
    int centaine=numberCommand/100;
    centaine=centaine%10;
    return(centaine);
}

//Tete
int traduireDizaine(int numberCommand){
    int dizaine=numberCommand/10;
    dizaine=dizaine%10;
    return(dizaine);
}

//Corps
int traduireUnite(int numberCommand){
    int unite = numberCommand%10;
    return(unite);
}
    delay(10);
  }
}

