#include <ESP32Servo.h> 

HardwareSerial Esclave(1);

//--SERVOO--
Servo Dav1;
Servo Dav2;
Servo Dav3;

Servo Gm1;
Servo Gm2;
Servo Gm3;

Servo Dar1;
Servo Dar2;
Servo Dar3;

//Mise en place servo moteur GM/DAV/DAR
#define pinServoDav1 5
#define pinServoDav2 17
#define pinServoDav3 16

#define pinServoGm1 27
#define pinServoGm2 14
#define pinServoGm3 12

#define pinServoDar1 4
#define pinServoDar2 2
#define pinServoDar3 15

// put your setup code here, to run once:
#define SenderTxPin 22
#define SenderRxPin 23

//indice pour avancement des pattes gauches
int indiceDroit1 = 0;
int indiceDroit2 = 0;
int indiceDroit3 = 0;
int indiceDroit4 = 0;


int vitesse=90;

  int posBaseDav1=90;
  int posBaseDav2=35;
  int posBaseDav3=15;
  
  int posBaseGm1=100;
  int posBaseGm2=150;
  int posBaseGm3=165;

  int posBaseDar1=110;
  int posBaseDar2=35;
  int posBaseDar3=15;

String recu;

void setup() {
  Serial.begin(115200);
  Serial.println("---Demarrage de la fourmi---");
  Esclave.begin(115200,SERIAL_8N1,SenderTxPin,SenderRxPin);

  //setup moteur 
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  Dav1.setPeriodHertz(50);
  Dav2.setPeriodHertz(50);
  Dav3.setPeriodHertz(50);
  
  Gm1.setPeriodHertz(50);
  Gm2.setPeriodHertz(50);
  Gm3.setPeriodHertz(50);
  
  Dar1.setPeriodHertz(50);
  Dar2.setPeriodHertz(50);
  Dar3.setPeriodHertz(50);
  
  Dav1.attach(pinServoDav1,500,2400);
  Dav2.attach(pinServoDav2,500,2400);
  Dav3.attach(pinServoDav3,500,2400);

  Gm1.attach(pinServoGm1,500,2400);
  Gm2.attach(pinServoGm2,500,2400);
  Gm3.attach(pinServoGm3,500,2400);

  Dar1.attach(pinServoDar1,500,2400);
  Dar2.attach(pinServoDar2,500,2400);
  Dar3.attach(pinServoDar3,500,2400);

  //-------------------position de base des pattes au démarrage--------------------
  Dav1.write(posBaseDav1);
  Dav2.write(posBaseDav2);
  Dav3.write(posBaseDav3);

  Gm1.write(posBaseGm1);
  Gm2.write(posBaseGm2);
  Gm3.write(posBaseGm3);

  Dar1.write(posBaseDar1);
  Dar2.write(posBaseDar2);
  Dar3.write(posBaseDar3);
}


void loop() {

  while(Esclave.available()){

    recu = Esclave.readStringUntil(';');
    Serial.println(recu);
  
    if(recu=="avancer"){
      Serial.println("j'avance");
      moveLegDav();
      moveLegDar();
      moveLegGm();
    }
    else if(recu=="reculer"){
      
    }
    else if(recu=="tournerD"){
      
    }
    else if(recu=="tournerG"){
      
    }
    delay(vitesse);
  }

}

//C'est cette patte qui donne le rythme
void moveLegDav(){
  // Rise the leg
  if (indiceDroit1 <= 10) {
    Dav3.write(posBaseDav3 - (indiceDroit1*2));
    Dav2.write(posBaseDav2 - (indiceDroit1*3));
    indiceDroit1++;
  }
  // Rotate the leg
  if (indiceDroit2 <= 30) {
    Dav1.write(posBaseDav1 - indiceDroit2);
    indiceDroit2++;
  }
  // Move back to touch the ground
  if (indiceDroit2 > 20 & indiceDroit3 <= 10) {
    Dav3.write(posBaseDav3-20 + (indiceDroit3*2));
    Dav2.write(posBaseDav2+30 + (indiceDroit3*3));
    indiceDroit3++;
  }
  // Stance phase - move leg while touching the ground
  // Rotate back to initial position
  if (indiceDroit2 >= 30) {
    Dav1.write((posBaseDav1-30) + indiceDroit4);  //Dav1 est à sa (posBase-30) pcq elle est à la position avancée. On la rétracte vers sa posBase (+30)
    indiceDroit4++;
  }
  // Reset the counters for repeating the process
  if (indiceDroit4 >= 30) {
    indiceDroit1 = 0;
    indiceDroit2 = 0;
    indiceDroit3 = 0;
    indiceDroit4 = 0;
  }
  // Each iteration or step is executed in the main loop section where there is also a delay time for controlling the speed of movement
}

void moveLegDar() {
  //rise leg
  if (indiceDroit1 <= 10) {
    Dar3.write(posBaseDar3 - (indiceDroit1*2));
    Dar2.write(posBaseDar2 - (indiceDroit1*3));
  }

  //rotate vers l'avant
  if (indiceDroit2 <= 30) {
    Dar1.write(posBaseDar1 - indiceDroit2);
  }

  //en bas
  if (indiceDroit2 > 20 & indiceDroit3 <= 10) {
    Dar3.write(posBaseDar3-20 + (indiceDroit3*2));
    Dar2.write(posBaseDar2+30 + (indiceDroit3*3));
  }

  //rotate vers l'arrière
  if (indiceDroit2 >= 30) {
    Dar1.write((posBaseDar1-30) + indiceDroit4);  //Gar1 est à sa (posBase-30) pcq elle est à la position avancée. On la rétracte vers sa posBase (+30)
  }
}

void moveLegGm() {
  //haut
  if (indiceDroit1 <= 10) {
    Gm3.write(posBaseGm3 + (indiceDroit1*2));
    Gm2.write(posBaseGm2 + (indiceDroit1*3));
  }

  //rotate ++
  if (indiceDroit2 <= 30) {
    Gm1.write(posBaseGm1 + indiceDroit2);

  }

  //bas
  if (indiceDroit2 > 20 & indiceDroit3 <= 10) {
    Gm3.write(posBaseGm3+20 - (indiceDroit3*2));
    Gm2.write(posBaseGm2+30 - (indiceDroit3*3));
  }

  //rotate --
  if (indiceDroit2 >= 30) {
    Gm1.write((posBaseGm1+30) - indiceDroit4);  //Dm1 est à sa (posBase-30) pcq elle est à la position avancée. On la rétracte vers sa posBase (+30)
  }
}
