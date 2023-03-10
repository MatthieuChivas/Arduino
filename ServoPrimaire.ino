#include "BluetoothSerial.h" 
#include "esp_bt_device.h" 
#include <ESP32Servo.h>  
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED) 
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it 
#endif 

BluetoothSerial SerialBT; 
const String deviceName = "Fourmi"; 

//setup le port 2 comme maitre (envoyer des informations) 
HardwareSerial Maitre(2); 

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

int avancerD=0; 
int vitesse=30; 

//----------------- Setup Moteur ------------ 

//Servo gauche avant 
Servo Gav1; 
Servo Gav2;
Servo Gav3; 

//Servo gauche arrière 
Servo Gar1; 
Servo Gar2; 
Servo Gar3; 

//Servo droite milieu 
Servo Dm1; 
Servo Dm2; 
Servo Dm3; 

//association des servomoteurs aux pins 

//Servo gauche avant 
const int pinServoGav1 = 4; 
const int pinServoGav2 = 16; 
const int pinServoGav3 = 17; 

//Servo gauche arrière 
const int pinServoGar1 = 27; 
const int pinServoGar2 = 14; 
const int pinServoGar3 = 12; 

//Servo droite milieu 
const int pinServoDm1 = 5; 
const int pinServoDm2 = 17; 
const int pinServoDm3 = 16; 

void setup() {   

  //initialise console 
  Serial.begin(115200); 
  Serial.println("---Demarrage de la fourmi---"); 

  //initialise quel Pin va être utilisé 
  Maitre.begin(115200,SERIAL_8N1,SenderTxPin,SenderRxPin); 

  // Démarrage du bluetooth 
  SerialBT.begin(deviceName);

  // Timer pour les servo  
  ESP32PWM::allocateTimer(0); 
  ESP32PWM::allocateTimer(1); 
  ESP32PWM::allocateTimer(2); 
  ESP32PWM::allocateTimer(3); 

  Gav1.setPeriodHertz(50);
  Gav2.setPeriodHertz(50);   
  Gav3.setPeriodHertz(50); 

  Gar1.setPeriodHertz(50); 
  Gar2.setPeriodHertz(50);   
  Gar3.setPeriodHertz(50);  

  Dm1.setPeriodHertz(50); 
  Dm2.setPeriodHertz(50);   
  Dm3.setPeriodHertz(50); 

  Gav1.attach(pinServoGav1, 500, 2400); 
  Gav2.attach(pinServoGav2, 500, 2400); 
  Gav3.attach(pinServoGav3, 500, 2400); 

  Gar1.attach(pinServoGar1, 500, 2400); 
  Gar2.attach(pinServoGar2, 500, 2400); 
  Gar3.attach(pinServoGar3, 500, 2400); 

  Dm1.attach(pinServoDm1, 500, 2400); 
  Dm2.attach(pinServoDm2, 500, 2400); 
  Dm3.attach(pinServoDm3, 500, 2400); 
} 

void loop() { 

  String blueetoothSendMsg; 

  if (SerialBT.available()) { 
    blueetoothSendMsg = SerialBT.readString(); 
    blueetoothSendMsg.trim(); 
  } 

  if(blueetoothSendMsg=="avancer"){ 

    //tu codes en else if car l'avancement se fait petit pas par petit pas la boucle utilise le boucle du programme principal 

    if(!avancerD){ 
      avancerG(); 
    } 
    else{ 
    Maitre.print("avancer"); 
    avancerG(); 
    } 

  } 
  delay(vitesse); 
} 

void avancerG(){ 
    moveLegGav(); 
    moveLegGar(); 
    moveLegDm(); 
} 

//C'est cette patte qui donne le rythme 
void moveLegGav(){ 
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
    Dm1.write(30 + indiceGauch2); 
  } 

  //bas 
  if (indiceGauch2 > 20 & indiceGauch3 <= 10) { 
    Dm3.write(100 - indiceGauch3 * 2); 
    Dm2.write(80 - indiceGauch3 * 3); 
  } 

  //rotate -- 
  if (indiceGauch2 >= 30) { 
    Dm1.write(60 - indiceGauch4); 
  } 

} 
 