/*
* 
* definiciones utilizadas en el codigo principal
*
*/

#define DEBUG true

#define FRAMERATE 25

// enable or disable recepcion de Dragonframe

//bool dragonframeON = false; 

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

// gramophono
#define GRAMOPHONO_MOTOR 13
// cuidado con el orden
Stepper motorGramophono(STEPS, 22, 26, 24, 28);