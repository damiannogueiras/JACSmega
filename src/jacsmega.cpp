#include <Arduino.h>
#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>
#include <Stepper.h>

#include <jacsmega.h>

#define DEBUG true

SLIPEncodedSerial SLIPSerial(Serial);
OSCErrorCode error;

#define STEPS 2048
Stepper motorGramo(STEPS, 22, 26, 24, 28);
int velocidad = 0;

void gramophono(OSCMessage &msg)
{
  if (DEBUG) Serial.println("recibido en gramophono");
  if (msg.isFloat(0))
  {
    // recogemos el valor 
    int valor = msg.getFloat(0);

    // si recibimos 1, clockwise
    // si recibimos -1, counterclockwise
    // 78 RPM son aprox 1 vuelta por segundo
    // para un framerate 25 -> aprox STEPS/6
    motorGramo.step(valor*STEPS/6);

    if (DEBUG) {
      Serial.print("Valor: ");
      Serial.println(valor);
    }
    
  }
}

void setup() {
  // initialize
  SLIPSerial.begin(9600);
  //Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // inicializamos motor gramophono
  motorGramo.setSpeed(16);
}

void loop() {
  OSCMessage msgIN;
  int size;

  // recibimos message
  while(!SLIPSerial.endofPacket())
    if ((size = SLIPSerial.available()) > 0) {
      while(size--)
        msgIN.fill(SLIPSerial.read());
        //if (DEBUG) Serial.println(SLIPSerial.read());
    }

  if(!msgIN.hasError()) {
    // if (DEBUG) Serial.println("recibido en loop");
    // despachamos segun el address pattern
    msgIN.dispatch("/gramophono", gramophono);
  } else {
      error = msgIN.getError();
      Serial.print("error: ");
      Serial.println(error);
  }

}