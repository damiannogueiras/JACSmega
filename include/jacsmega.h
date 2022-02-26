/*
 * 
 * definiciones utilizadas en el codigo principal
 *
 */

#include <FastLED.h>
// la Servo da 'tics' en el servo con FastLed
// https://github.com/FastLED/FastLED/issues/620
// solucionado con esta libreria https://platformio.org/lib/show/600/PWMServo
#include <PWMServo.h>

#define DEBUG true
#define OFF 0
#define ON 1

#define FRAMERATE 25

// funcoines para efecto kitt
void kittOnOff(OSCMessage &msg, int param);
void kittVelocidad(OSCMessage &msg, int param);

// habilitar la recepcion de dragonframe
bool dragonframeON;

// motor 1 - gramophono
// pasos por vuelta de los motores paso a paso
#define STEPS 2048
/*
 * 78 RPM son aprox 1 vuelta por segundo
 * para un framerate 25 -> aprox STEPS/6
 * (un poco raro, deberia ser /25, revisar en la practica)
 */
#define FRAMESTEPS 2048/6
// cuidado con el orden, en la placa 1-2-3-4 -> 22-24-26-28
Stepper motor1(STEPS, 22, 26, 24, 28);

// servo s1
PWMServo servo1;
// pin en la placa
#define PIN_SERVO1 11

// nro de strips
#define NUM_STRIPS 2
// numero de np en las tiras
#define NUM_NP_STRIPS 30
// pin en la placa
#define PIN_STRIP0 51
#define PIN_STRIP1 53
// array de arrays
CRGB _strips[NUM_STRIPS][NUM_NP_STRIPS];
// Define array para almacenamiento de los valores de los strips
// Guardamoes 4 valores: r g b dimmer
int _stripsAll[NUM_STRIPS][4];



// strip1 de neopixels
// How many leds in your strip?
#define NUM_NP1 30

// strip2 de neopixels
// How many leds in your strip?
#define NUM_NP2 30

uint8_t max_bright = 128;
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
// Define the array of leds
CRGB _strip1[NUM_NP1];
CRGB _strip2[NUM_NP2];
// Define array para almacenamiento de los valores de los leds
// Guardamoes 4 valores: r g b dimmer
int _strip1All[NUM_NP1][4];
int _strip2All[NUM_NP2][4];
#define R 0
#define G 1
#define B 2
#define DIMMER 3
// msg OSC /strip1/[0..NUM_NP-1]/[1..4]
// 8 corresponde a la posicion del neop en la direccion
// 10 corresponde a la posicion del parametro correspondiente en la direccion
#define MSG_NP 8
#define MSG_PARAM 10

// msg OSC /strip/[0..NUM_STRIP-1]/[1..4]
// 7 corresponde a la posicion del nro de strip en la direccion
// 9 corresponde a la posicion del parametro correspondiente en la direccion
#define MSG_STRIP 7
#define MSG_STRIP_PARAM 9

// leds
// nro de leds
#define NUM_LEDS 5
// array de leds
// posicion del PIN en el array
#define PIN_PLACA_LED 0
// posicion del valor en el array
#define VALOR 1
int _leds[NUM_LEDS][2];
// msg OSC /leds/[0..NUM_LEDS-1]
// corresponde con la posicion del led en la direccion
#define MSG_LED 6
// pin de los leds - en el mega2560 2 - 13 y 44 - 46 son las pwm
int _pinLeds[] = {LED_BUILTIN,3,4,5,6};
