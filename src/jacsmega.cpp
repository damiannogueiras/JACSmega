#include <Arduino.h>
#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>
#include <Stepper.h>

#include <jacsmega.h>

#define DEBUG true

SLIPEncodedSerial SLIPSerial(Serial);
OSCErrorCode error;

// pasos por vuelta
#define STEPS 2048
// cuidado con el orden
Stepper motorGramo(STEPS, 22, 26, 24, 28);
int velocidad = 0;

void gramophono(OSCMessage &msg)
{
  if (DEBUG)
    Serial.println("recibido en gramophono");
  if (msg.isInt(0))
  {
    // recogemos el valor
    int valor = msg.getInt(0);

    /* si recibimos 1, clockwise
     * si recibimos -1, counterclockwise
     * 78 RPM son aprox 1 vuelta por segundo
     * para un framerate 25 -> aprox STEPS/6
     * (un poco raro, deberia ser 25, revisar en la practica)
     */

    motorGramo.step(valor * STEPS / 6);

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
  int valor = 0;
  if (DEBUG)
    Serial.println("recibido en dragonframeShoot");
  if (msg.isInt(0))
  {
    valor = msg.getInt(0);
    Serial.print("SHOOT: ");
    Serial.println(valor);
  }
  // movemos el motor del gramophono
  OSCMessage msggramo("/gramophono");
  msggramo.add(1);
  gramophono(msggramo);
}

/**
 * Eventos del Dragonframe POSITION
 * listo para captura del frame x
 */
void dragonframePosition(OSCMessage &msg)
{
  int valor = 0;
  if (DEBUG)
    Serial.println("recibido en dragonframePosition");
  if (msg.isInt(0))
  {
    valor = msg.getInt(0);
    Serial.print("POSITION: ");
    Serial.println(valor);
  }
}

void setup()
{
  // initialize
  SLIPSerial.begin(9600);
  //Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // inicializamos motor gramophono
  motorGramo.setSpeed(16);
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