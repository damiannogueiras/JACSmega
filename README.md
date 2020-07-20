# JACS

## Just Another Control Set - mega2560 part

Recibe los mensajes OSC del esp8266 y actúa sobre el set.

Mensajes:

- /gramophono: Para mover el motor paso a paso del giradisco (1 segundo una vuelta a 25 framerate):
  - +1 giro clockwise, avanza 1/25 de una vuelta
  - -1 giro counterclockwise, retrocede 1/25 de una vuelta
  
Próximos pasos:

- Control de leds
- Control de neopixels
- Control de servos
- Control de motores DC
