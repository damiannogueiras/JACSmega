/*
 * 
 * definiciones utilizadas en el codigo principal
 *
 */

#include <FastLED.h>
#include <Servo.h>

#define DEBUG true

#define FRAMERATE 25

// pasos por vuelta de los motores paso a paso
#define STEPS 2048
/*
 * 78 RPM son aprox 1 vuelta por segundo
 * para un framerate 25 -> aprox STEPS/6
 * (un poco raro, deberia ser /25, revisar en la practica)
 */
#define FRAMESTEPS 2048/6

// habilitar la recepcion de dragonframe
bool dragonframeON;

/*
 * PIN
 * Elemento
 */

// motor 1 - gramophono
// cuidado con el orden, en la placa 1-2-3-4 -> 22-24-26-28
Stepper motor1(STEPS, 22, 26, 24, 28);

// servo s1
Servo servo1;
#define PIN_SERVO1 40

// strip1 de neopixels
// How many leds in your strip?
#define NUM_LEDS 6
#define PIN_STRIP1 53
uint8_t max_bright = 128;
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
// Define the array of leds
CRGB _strip1[NUM_LEDS];
// Define array para almacenamiento de los valores de los leds
// Guardamoes 4 valores: r g b dimmer
int _strip1All[NUM_LEDS][4];
// msg OSC /strip1/nroLed/valor
#define MSG_LED 8
#define MSG_PARAM 10