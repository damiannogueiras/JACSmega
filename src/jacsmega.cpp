#include <Arduino.h>
// https://github.com/CNMAT/OSC
#include <OSCMessage.h>
#include <SLIPEncodedSerial.h>
#include <Stepper.h>

#include <jacsmega.h>
#include "Frame.h"
#include "LinkedListLib.h"
/*
 * Lista que relaciona  cada frame
 * con la posicion de los motores, servos, leds, etc
 * El indice de la lista es el frame
 * Los elementos son objetos de la Clase Frame
 * son los distintos elementos en el set
 */
LinkedList<Frame> matrix = LinkedList<Frame>();
LinkedList<String> matrix_s;
bool matrixLista = false;
String array_s[100] = {};

SLIPEncodedSerial SLIPSerial(Serial1);
OSCErrorCode error;

/*
 * Control del gramophono
 * Recibe los pasos que se mueve el motor
 */
void m1(OSCMessage &msg)
{
    if (DEBUG)
        Serial.println("recibido en M1");
    if (msg.isFloat(0))
    {
        // recogemos el valor
        int valor = msg.getFloat(0);
        // movemos el motor
        motorGramophono.step(valor);

        if (DEBUG)
        {
            Serial.print("Motor: ");
            Serial.println(valor);
        }
    }
}

/*
 * Tira de led1
 * escritorio
 */
void strip1(OSCMessage &msg, int patternOffset){
    char addr[] = "";
    if (DEBUG) {  
        msg.getAddress(addr);
        Serial.println(addr);
    }

    // /strip1/[0-5]/[0-3]
    // MSG_LED (ocho) es posicion del led
    // MSG_VALOR seran los valores 1:r , 2:g , 3:b , 4:dimmer
    int i = addr[MSG_LED] - '0'; // para pasar char a int
    int parametro = addr[MSG_PARAM] - '0';
    int valor = msg.getFloat(0);

        /*Serial.println(i);
        Serial.println(parametro);
        Serial.println(valor);*/

    // actualiza valor en la matriz en memoria
    _strip1All[i][parametro-1] = valor;
    // DEBUG
    for (int i = 0; i < NUM_LEDS; i++)
    {
        Serial.print(i); Serial.print(":");
        for (int j = 0; j < 4; j++) {
            Serial.print(_strip1All[i][j]);Serial.print("-");
        }
        Serial.println();        
    }
    // actualiza el led correspondiente de la tira
    _strip1[i].red = _strip1All[i][0];
    _strip1[i].green = _strip1All[i][1];
    _strip1[i].blue = _strip1All[i][2];
    _strip1[i].fadeToBlackBy(_strip1All[i][3]);

    FastLED.show();
}

/*
 * Enable or Disable
 * recepcion de dragonframe
 */
void dragonframeEnable(OSCMessage &msg)
{
    if (DEBUG) {
        Serial.println("recibido en dragonframeEnable");
        // para saber el tipo de dato que manda
        Serial.println(msg.getType(0));
    }
    if (msg.isFloat(0)){
        // ponemos a 1 o 0 (true or false)
        dragonframeON = msg.getFloat(0);
        Serial.print("Status Dragonframe ");
        Serial.println(dragonframeON);
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
    //msggramo.add(matrix.get(1)->_m1);
    m1(msggramo);
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

/*
void initMatrix() {
    // m_gramo, s_ventana, l_desk
    Frame *f0000 = new Frame(0, 0, 0);
    Frame *f0001 = new Frame(FRAMESTEPS, 90, 1);
    Frame *f0002 = new Frame(FRAMESTEPS/2, 90, 1);
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
*/
/*
 * Receive a line of the csv file with frame information
 * from esp8266
 */
int f, _m1, _m2, _s1, _s2, _l1, _l2, _l3, _np1r, _np1g, _np1b;

void fileFrames(OSCMessage &msg) {

    char linea[]="";
    // Recibida linea
    //Serial.println(linea);
    msg.getString(0, linea);
    
    if (DEBUG) {
        //Serial.print(".");
        //Serial.println(f);
        //Serial.println(linea);
        Serial.print("array_s:");
        Serial.println(array_s[f]);
        //Serial.println(frame->_s1);
    }

    msg.empty();
    /*
    // mostramos la matriz
    if (f > 5) {
        Serial.println("MOSTRANDO MATRIX");
        Serial.println(matrix_s.GetHead());
        //for (int i=0; i < 26; i++) {
        //    Serial.print(i); Serial.print(": ");
        Serial.println(matrix_s.GetAt(3));
        //}
        //Serial.println(matrix_s.GetTail());
    }*/
}

void cualquiercosa(OSCMessage &msg) {
    Serial.print("Cualquiercosa: ");
    char addr[] = "";
    msg.getAddress(addr);
    Serial.println(addr);
}

void setup()
{
    // initialize
    SLIPSerial.begin(57600);
    Serial.begin(57600);
    pinMode(LED_BUILTIN, OUTPUT);

    // inicializamos motor gramophono
    motorGramophono.setSpeed(16);

    // inicializamos tira leds
    pinMode(PIN_STRIP1, OUTPUT);
    FastLED.addLeds<LED_TYPE, PIN_STRIP1, COLOR_ORDER>(_strip1, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(max_bright);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        for (int j = 0; j < 4; j++) {
            _strip1All[i][j] = 0;
        }        
    }

    Serial.println("Setup done");
}

void loop()
{
    OSCMessage msgIN;
    int size;

    // Espera por un packet
    // El loop NO se queda aqui, ver
    // https://github.com/CNMAT/OSC/blob/master/examples/SerialReceiveInfiniteLoop/SerialReceiveInfiniteLoop.ino
    if (SLIPSerial.available())
        while (!SLIPSerial.endofPacket()) 
            if ((size = SLIPSerial.available()) > 0) 
                while (size--) {
                    if (DEBUG) {
                        // Serial.print(".");
                    }
                    msgIN.fill(SLIPSerial.read());
                }
    // esto se ejecuta en cada loop
    if (!msgIN.hasError())
    {
        char addr[] = "";
        msgIN.getAddress(addr);
        
        // DEBUG
        Serial.print("Recibiendo ");
        //Serial.println(addr);
        
        // despachamos segun el address pattern
        // motor1
        msgIN.dispatch("/m1", m1);
        // tira de led 1
        msgIN.route("/strip1", strip1);

        // enable or disable dragon
        msgIN.dispatch("/dragonframe", dragonframeEnable);
        if (dragonframeON) {
            msgIN.dispatch("/dragonframe/shoot", dragonframeShoot);
            msgIN.dispatch("/dragonframe/position", dragonframePosition);
        } 
        // msgIN.dispatch("/frame", fileFrames);

        // cualquier otra cosa  
        msgIN.dispatch("/*", cualquiercosa);


        // msgIN.empty(); no vaciar, hacerlo en la funcion callback 
    }
    else
    {
        /*if (DEBUG)
        {
            error = msgIN.getError();
            Serial.print("error general: ");
            Serial.println(error);
        }*/
    }
}