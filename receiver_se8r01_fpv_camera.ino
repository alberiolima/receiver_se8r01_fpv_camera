/*
 * pinagem modulo wireless
 * 
 * Nome  se8r01   UNO
 * 3.3V   1       3.3V
 * GND    2       GND
 * CE     3       7
 * CSN    4       10
 * SCK    5       13
 * MOSI   6       11
 * MISO   7       12
 * IRQ    8       8
 */

#include "se8r01.h"
#include <Servo.h>

#define intervaloControle   50 //Intervalo entre os envios

const int PAYLOAD_WIDTH = 12;
byte rxData[PAYLOAD_WIDTH];
Servo myservoH; 
Servo myservoV; 
byte iServoH = 90;
byte iServoV = 90;
unsigned long tempoControle = 0;

//Canel 1 = joystick1 Aceleração
//Canal 2 = joystick1 Não utilizado nesse projeto
//Canal 3 = joystick2 Não utilizado nesse projeto
//Canal 4 = joystick2 Direcao
//Canel 5 = joystick1 Botão de seleção, Não utilizado nesse projeto
//Canel 6 = joystick2 Botão de seleção, Não utilizado nesse projeto
int radioCanal[6]= {512,512,512,512,0,0};

void init_radio() {

  
  if (!init_rf(10, 7, 8, PAYLOAD_WIDTH)) {
    Serial.println("Chip not found!");
    while (1);

  }
  setPower(POWER_5dbm);
  selectTxPipe(0);
  setRtr(15);
  changeMode(RX_MODE);
}

void setup() {
  #ifdef c_debbug
    Serial.begin(9600);
  #endif
  
  init_radio();
  
  myservoH.attach(6); 
  myservoV.attach(5); 

}

void loop() {
  recebeRadio();
  if ( millis() > tempoControle ) {
    tempoControle = millis() + intervaloControle; 
    
    iServoH = map( radioCanal[0], 0, 1023, 0, 180 );
    iServoV = map( radioCanal[1], 0, 1023, 0, 180 );

    myservoH.write( iServoH ); 
    myservoV.write( iServoV ); 
    
  }
}

void recebeRadio(){
  if(digitalRead(8)==LOW) {
    delayMicroseconds(240);  
    byte pipe = 7;
    pipe = getRxData(rxData);
    if (pipe < 7) {
      while ((getStatusReg() & 0xE) != 0xE){
        getRxPayload(rxData, PAYLOAD_WIDTH);  
      }
      radioCanal[0] = (rxData[0] * 256) + rxData[1];
      radioCanal[1] = (rxData[2] * 256) + rxData[3];
      radioCanal[2] = (rxData[4] * 256) + rxData[5];
      radioCanal[3] = (rxData[6] * 256) + rxData[7];
      radioCanal[4] = (rxData[8] * 256) + rxData[9];
      radioCanal[5] = (rxData[10] * 256) + rxData[11];
        
//      for (int i = 0;i<6;i++){
//        Serial.print( radioCanal[i] );
//        Serial.print( " " );
//      }
//      Serial.println();
    }
  }
}
