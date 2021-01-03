# JACS

## Just Another Control Set - mega2560 part

Recibe los mensajes OSC del esp8266 y actúa sobre el set.

Recibe nro de frame del DragonFrame y aplica cambios

Mensajes:

- /m1: Para mover el motor paso a paso del giradisco (1 segundo una vuelta a 25 framerate):
  - +1 giro clockwise, avanza 1/25 de una vuelta
  - -1 giro counterclockwise, retrocede 1/25 de una vuelta

- /strip1: RGB de cada led y su brillo (X es la posicion del neopixel 0..)
  - /X/1 (rojo)
  - /X/2 (verde)
  - /X/3 (azul)
  - /X/4 (dimmer)

- /dragonframe: habilita (1) o no (0) recepción de DragonFrame
  
Carga de Próximos pasos:

- Control de leds
- Control de servos
- Control de motores DC
