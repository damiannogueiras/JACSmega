/**
 *   Recibe los comandos osc del esp y los ejecuta en el set
 *   Conectar:
 *             RX (esp8266) - RX1 (mega)
 *             TX (esp8266) - TX1 (mega)
 **/

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

// enciendo o apaga el efecto
int efectoOn = 0;
int v = 110;
int dot = 0;     // indice del pixel
int sentido = 1; // 1 avanza -1 retrocede
int hue = 180, sat = 100, val = 255;

/*
 * Tiras de led
 */
void strip(OSCMessage &msg, int patternOffset)
{
    char addr[] = "";
    if (DEBUG)
    {
        msg.getAddress(addr);
        Serial.println(addr);
    }

    // Mensaje OSC
    // /strip/[0-(NUM_STRIPS-1)]/[0-3]
    // MSG_STRIP es el numero strip
    int stripIndex = addr[MSG_STRIP] - '0'; // para pasar char a int
    // MSG_STRIP_PARAM seran los valores 1:r , 2:g , 3:b , 4:dimmer
    int parametro = addr[MSG_STRIP_PARAM] - '0';
    int valor = msg.getFloat(0);

    Serial.print(stripIndex);
    Serial.print(": ");
    Serial.print(parametro);
    Serial.print("->");
    Serial.println(valor);

    // actualiza valor en la matriz en memoria de un elemento
    _stripsAll[stripIndex][parametro - 1] = valor;

    // DEBUG
    /*for (int i = 0; i < NUM_NP; i++)
    {
        Serial.print(i); Serial.print(":");
        for (int j = 0; j < 4; j++) {
            Serial.print(_strip1All[i][j]);Serial.print("-");
        }
        Serial.println();
    }*/

    // actualizamos todo la tira con la matriz
    for (int led = 0; led < NUM_NP_STRIPS; led++)
    {
        _strips[stripIndex][led].red = _stripsAll[stripIndex][R];
        _strips[stripIndex][led].green = _stripsAll[stripIndex][G];
        _strips[stripIndex][led].blue = _stripsAll[stripIndex][B];
        _strips[stripIndex][led].fadeToBlackBy(_stripsAll[stripIndex][DIMMER]);
    }
    // mandamos nuevos valores a las tiras
    FastLED.show();
}

/*
 * Modificar leds
 */
void leds(OSCMessage &msg, int patternOffset)
{
    char addr[] = "";
    if (DEBUG)
    {
        msg.getAddress(addr);
        Serial.println(addr);
    }
    // recoje la posicion del led
    int i = addr[MSG_LED] - '0'; // para pasar char a int
    int valor = msg.getFloat(0);
    // actualiza valor en el array en memoria
    _leds[i][VALOR] = valor;
    // modifico led
    analogWrite(_leds[i][PIN_PLACA_LED], _leds[i][VALOR]);
}

void cualquiercosa(OSCMessage &msg)
{
    Serial.print("??: ");
    char addr[] = "";
    msg.getAddress(addr);
    Serial.println(addr);
}

/*
 * Efecto emdr - kitt el coche fantástico
 */
void kitt(OSCMessage &msg, int patternOffset)
{
    char addr[] = "";
    if (DEBUG)
    {
        msg.getAddress(addr);
        Serial.println(addr);
    }

    /**
     *  Mensaje OSC
     *
     * /kitt/velocidad/[0-1] velocidad de los pixels
     * /kitt/toggleOnOff/[0|1] enciendo o apaga la tira
     *
     */

    //
    // MSG_STRIP es el numero strip
    // int stripIndex = addr[MSG_STRIP] - '0'; // para pasar char a int
    // MSG_STRIP_PARAM seran los valores 1:r , 2:g , 3:b , 4:dimmer
    /*int parametro = addr[MSG_STRIP_PARAM] - '0';
    int valor = msg.getFloat(0);

        Serial.print(stripIndex); Serial.print(": ");
        Serial.print(parametro); Serial.print("->");
        Serial.println(valor);*/

    // actualiza valor en la matriz en memoria de un elemento
    // _stripsAll[stripIndex][parametro-1] = valor;

    // DEBUG
    /*for (int i = 0; i < NUM_NP; i++)
    {
        Serial.print(i); Serial.print(":");
        for (int j = 0; j < 4; j++) {
            Serial.print(_strip1All[i][j]);Serial.print("-");
        }
        Serial.println();
    }*/

    // actualizamos todo la tira con la matriz
    /*for(int led = 0; led < NUM_NP_STRIPS; led++) {
        _strips[stripIndex][led].red = _stripsAll[stripIndex][R];
        _strips[stripIndex][led].green = _stripsAll[stripIndex][G];
        _strips[stripIndex][led].blue = _stripsAll[stripIndex][B];
        _strips[stripIndex][led].fadeToBlackBy(_stripsAll[stripIndex][DIMMER]);
    }
    // mandamos nuevos valores a las tiras
    FastLED.show();
    */
}

void kittOnOff(OSCMessage &msg, int param)
{
    efectoOn = msg.getFloat(0);
    // DEBUG
    Serial.print("ON/OFF:");
    Serial.println(efectoOn);

    if (!efectoOn)
    {
        Serial.println("Apagando");
        FastLED.clear();
        FastLED.show();
    }
}
/**
 * realiza el efecto kitt
 * @param v velocidad de desplazamiento del pixel
 *
 */
void kittEfecto()
{

    // DEBUG
    // Serial.print("dot:");
    // Serial.println(dot);
    // _strips[0][dot] = CRGB::Blue;
    fill_solid( &(_strips[0][dot]), 1 /*number of leds*/, CHSV(hue, sat, val) );
    FastLED.show();
    delay(v);
    // clear this led for the next time around the loop
    _strips[0][dot] = CRGB::Black;
    delay(v);
    // va para la derecha
    if (dot < NUM_NP_STRIPS && sentido > 0)
    {
        dot = dot + sentido;
    }
    // va para la izquierda
    if (dot > 0 && sentido < 0)
    {
        dot = dot + sentido;
    }

    // cambio de sentido para la derecha
    if (dot == 0 && sentido < 0)
    {
        sentido = 1;
    }

    // cambio de sentido para la izquierda
    if (dot == NUM_NP_STRIPS - 1 && sentido > 0)
    {
        sentido = -1;
    }

    // DEBUG
    // Serial.println("Effect");
}

void kittVelocidad(OSCMessage &msg, int param)
{
    v = 120 - msg.getFloat(0);
    // DEBUG
    // Serial.println(v);
}

void kittColor(OSCMessage &msg, int param){
    Serial.print("color: ");
    Serial.print(msg.getFloat(0));
    // Serial.println(msg.getFloat(1));
    hue = msg.getFloat(0);
    // sat = msg.getFloat(1);
}

void kittBrillo(OSCMessage &msg, int param) {
    val = msg.getFloat(0);
    Serial.print("brillo: ");
    Serial.print(msg.getFloat(0));
}

void setup()
{
    // initialize
    SLIPSerial.begin(57600);
    Serial.begin(57600);
    pinMode(LED_BUILTIN, OUTPUT);

    // inicializamos motor gramophono
    motor1.setSpeed(16);

    // inicializamos servo
    servo1.attach(PIN_SERVO1);

    // inicializamos tiras neopixels
    pinMode(PIN_STRIP0, OUTPUT);
    pinMode(PIN_STRIP1, OUTPUT);
    FastLED.addLeds<LED_TYPE, PIN_STRIP0, COLOR_ORDER>(_strips[0], NUM_NP1).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, PIN_STRIP1, COLOR_ORDER>(_strips[1], NUM_NP1).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(max_bright);

    // inicializamos los leds
    for (int i = 0; i < NUM_LEDS; i++)
    {
        _leds[i][PIN_PLACA_LED] = _pinLeds[i];
        _leds[i][VALOR] = OFF;
        pinMode(_leds[i][PIN_PLACA_LED], OUTPUT);
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
                while (size--)
                {
                    if (DEBUG)
                    {
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
        Serial.print("Rx :");
        Serial.println(addr);

        // efecto emdr
        msgIN.route("/kitt/toggleOnOff", kittOnOff);
        msgIN.route("/kitt/velocidad", kittVelocidad);
        msgIN.route("/kitt/color", kittColor);
        msgIN.route("/kitt/brillo", kittBrillo);

        // leds individuales
        msgIN.route("/leds", leds);

        // despachamos segun el address pattern

        // tiras de neopixels
        msgIN.route("/strip", strip);

        // leds individuales
        msgIN.route("/leds", leds);

        // cualquier otra cosa
        // msgIN.dispatch("/*", cualquiercosa);

        // msgIN.empty(); no vaciar, hacerlo en la funcion callback
    } /*else {
        if (DEBUG)
        {
            error = msgIN.getError();
            Serial.print("error general: ");
            Serial.println(error);
        }
    }*/

    if (efectoOn)
    {
        kittEfecto();
    }
}