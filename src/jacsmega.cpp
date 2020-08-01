#include <Arduino.h>
#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>
#include <Stepper.h>

#include <jacsmega.h>
#include "Frame.h"
#include <LinkedList.h>
/*
 * Lista que relaciona  cada frame
 * con la posicion de los motores, servos, leds, etc
 * El indice de la lista es el frame
 * Los elementos son objetos de la Clase Frame
 * son los distintos elementos en el set
 */
LinkedList<Frame*> matrix = LinkedList<Frame*>();

#define DEBUG true

SLIPEncodedSerial SLIPSerial(Serial);
OSCErrorCode error;

/*
 * Control del gramophono
 * Recibe los pasos que se mueve el motor
 */
void gramophono(OSCMessage &msg)
{
  if (DEBUG)
    Serial.println("recibido en gramophono");
  if (msg.isInt(0))
  {
    // recogemos el valor
    int valor = msg.getInt(0);
    // movemos el motor
    motorGramophono.step(valor);

    if (DEBUG)
    {
      Serial.print("Motor: ");
      Serial.println(valor);
    }
  }
}

/**
 * Eventos del Dragonframe SHOOT
 * antes de capturar frame x
 */
void dragonframeShoot(OSCMessage &msg)
{
  int frame = 0;
  if (DEBUG)
    Serial.println("recibido en dragonframeShoot");
  if (msg.isInt(0))
  {
    frame = msg.getInt(0);
    Serial.print("SHOOT: ");
    Serial.println(frame);
  }
  // enviamos para mover el motor del gramophono 1/FRAMERATE 
  OSCMessage msggramo("/gramophono");
  // recogemos el frame
  msggramo.add(matrix.get(1)->motor_gramo);
  gramophono(msggramo);
}

/**
 * Eventos del Dragonframe POSITION
 * listo para captura del frame x
 */
void dragonframePosition(OSCMessage &msg)
{
  int frame = 0;
  if (DEBUG)
    Serial.println("recibido en dragonframePosition");
  if (msg.isInt(0))
  {
    frame = msg.getInt(0);
    Serial.print("POSITION: ");
    Serial.println(frame);
  }
}

void initMatrix(){
  // m_gramo, s_ventana, l_desk
  Frame *f0000 = new Frame(0,0,0);
  Frame *f0001 = new Frame(FRAMESTEPS,90,1);
  Frame *f0002 = new Frame(FRAMESTEPS/2,90,1);
  matrix.add(0, f0000);
  matrix.add(1, f0001);
  matrix.add(2, f0002);
  if (DEBUG) {
    for (int i=0; i < matrix.size(); i++) {
      Serial.print(i); Serial.print(": ");
      Serial.println(matrix.get(i)->motor_gramo);
    }
  }

}
void setup()
{
  // initialize
  SLIPSerial.begin(9600);
  //Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // inicializamos los valores que queremos de cada elemento del set
  initMatrix();
  // inicializamos motor gramophono
  motorGramophono.setSpeed(16);
}

void loop()
{
  OSCMessage msgIN;
  int size;

  // recibimos message
  while (!SLIPSerial.endofPacket())
    if ((size = SLIPSerial.available()) > 0)
    {
      while (size--)
        msgIN.fill(SLIPSerial.read());
      //if (DEBUG) Serial.println(SLIPSerial.read());
    }

  if (!msgIN.hasError())
  {
    // if (DEBUG) Serial.println("recibido en loop");
    // despachamos segun el address pattern
    msgIN.dispatch("/gramophono", gramophono);
    msgIN.dispatch("/dragonframe/shoot", dragonframeShoot);
    msgIN.dispatch("/dragonframe/position", dragonframePosition);
  }
  else
  {
    if (DEBUG)
    {
      error = msgIN.getError();
      Serial.print("error general: ");
      Serial.println(error);
    }
  }
}