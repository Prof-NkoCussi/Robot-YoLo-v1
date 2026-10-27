# Flujo de armado de robots

Reglas y pasos fijos para armar cualquier robot del proyecto (YoLo v1, YoLo v2, Yarbi y los que vengan).
Siempre se arranca por la Fase 0 y **no se pasa a la fase siguiente sin cumplir su "Listo cuando"**.

---

## Reglas de oro

1. **De a un módulo por vez.** Se conecta una cosa, se prueba, y recién después se suma la siguiente.
2. **Primero en banco, después en el chasis.** Protoboard y ruedas en el aire. El montaje final va casi al final.
3. **Primero el papel, después los cables.** No se conecta nada que no esté en el diagrama y en la tabla de pines.
4. **Primero la energía, después las placas.** El regulador se ajusta y se mide antes de conectar cualquier placa.
5. **Si algo falla, se vuelve un paso atrás.** No se suman módulos sobre algo que no anda.
6. **Todo cambio se anota.** Pines, piezas o decisiones nuevas van a la ficha del robot.
7. **No se copian diagramas de internet sin revisarlos.** Muchos tienen errores (polaridad, pines de arranque, niveles de tensión).

---

## Fase 0 · Ficha del robot

Antes de tocar nada, completar:

| Campo | Ejemplo |
|---|---|
| Nombre y versión | YoLo v1 |
| Objetivo | Robot con cámara manejado por web vía MQTT |
| Placa principal | ESP32-CAM AI-Thinker |
| Qué cambia respecto de la versión anterior | — |
| Decisiones tomadas | Alimentación 2S, sensor anti-choque |
| Pendientes | Firmware, case 3D |

**Listo cuando:** la ficha está completa y el objetivo entra en dos líneas.

---

## Fase 1 · Kit y hardware

1. Armar la lista de materiales (BOM) por categoría: placas, energía, motores, sensores, componentes sueltos, cableado, herramientas.
2. Marcar qué hay y qué falta.
3. Si se reemplaza una pieza (ej. L298N por L9110S), anotar qué cambia: pines, tensión, tamaño, consumo.
4. Medir las placas con calibre (sirve para el case y la distribución).
5. Revisar cada módulo a ojo: pines doblados, soldaduras, baterías golpeadas o hinchadas.

| Pieza | Cantidad | ¿La tengo? | Observación |
|---|---|---|---|
| | | | |

**Listo cuando:** está todo lo necesario o hay una decisión tomada sobre cada faltante.

---

## Fase 2 · Diagrama

Elegir la herramienta según el caso:

| Caso | Herramienta |
|---|---|
| Diagrama de conexiones | Cirkit Designer (o dibujado por Claude) |
| Simular lógica con ESP32 DevKit (sensores, MQTT) | Wokwi (ver Fase 2b) |
| Placas que no se simulan (ESP32-CAM, cámara) | Solo diagrama + banco real |
| Arduino Uno | Tinkercad Circuits |

Contenido obligatorio:

1. **Diagrama de bloques** de la alimentación (batería → interruptor → reguladores → placas).
2. **Tabla de pines** completa.
3. **Revisión del diagrama** (checklist de abajo).

| Función | GPIO | Va a | Observación |
|---|---|---|---|
| | | | |

Checklist de revisión:

- [ ] Ningún pin de arranque usado sin revisar (en ESP32: GPIO 0, 2, 12 y 15).
- [ ] Niveles de tensión compatibles: nada de 5 V en un pin de 3,3 V.
- [ ] Cada módulo alimentado con la tensión correcta.
- [ ] GND común entre todo.
- [ ] Polaridad de batería revisada dos veces.
- [ ] Salidas de 5 V de otros módulos que no se usan, marcadas como "sin conectar".
- [ ] Consumo total estimado vs. lo que entrega el regulador.

**Listo cuando:** el diagrama y la tabla de pines coinciden y pasaron el checklist.

### Fase 2b · Simulación de lógica en Wokwi

Sirve para probar el código **antes de tocar el hardware**: sensores, MQTT y lógica (ej. anti-choque).

1. Crear el proyecto en Wokwi con un **ESP32 DevKit** (aunque el robot use ESP32-CAM).
2. Agregar los sensores que se puedan simular (ej. HC-SR04).
3. Motores: representarlos con LEDs (un LED por pin de control) para ver qué se activa.
4. Wi-Fi: usar la red `Wokwi-GUEST`.
5. MQTT: usar un broker público de prueba (el simulador no ve el Mosquitto de la red local).
6. Probar con `mosquitto_pub` / `mosquitto_sub` o un cliente MQTT web.

Reglas:

- Pines como constantes al principio del archivo: al pasar al hardware real solo cambian esos valores.
- La cámara **no se simula**: el video se prueba directo en banco.
- En el broker público usar tópicos únicos (ej. `yolo-cussi/<id>/comando`) y no mandar datos sensibles.
- Lo que anda en Wokwi se vuelve a probar en banco: la simulación no reemplaza la prueba real.

**Listo cuando:** el sensor, el MQTT y la lógica funcionan en la simulación.

---

## Fase 3 · Alimentación

Se arma y se prueba **sin placas conectadas**.

1. Batería → interruptor (en el positivo) → reguladores.
2. Medir la batería con multímetro.
3. Ajustar cada regulador a su tensión **sin carga** (ej. LM2596 a 5,0–5,1 V).
4. Medir de nuevo con una carga de prueba si es posible.

**Listo cuando:** cada salida mide lo que tiene que medir y el interruptor corta todo.

---

## Fase 4 · Conexiones y armado en banco

Orden sugerido (adaptar a cada robot):

1. Placa principal sola (por USB).
2. Driver de motores + motores, ruedas en el aire.
3. Sensores, de a uno.
4. Periféricos (display, servo, buzzer, LEDs).

En cada paso:

- [ ] Conectar con el robot **apagado**.
- [ ] Revisar contra la tabla de pines antes de encender.
- [ ] Encender y probar ese módulo (Fase 5).
- [ ] Recién después, sumar el siguiente.

**Listo cuando:** todos los módulos están conectados en banco y cada uno pasó su prueba.

---

## Fase 5 · Programación por capas

Un sketch o una función por capa. No se suma una capa si la anterior no anda.

1. **Placa viva:** blink o mensaje por Serial.
2. **Red:** conexión Wi-Fi y IP.
3. **Comunicación:** MQTT (suscribirse, publicar estado, LWT).
4. **Actuadores:** motores con PWM limitado.
5. **Sensores:** lectura, filtrado y publicación.
6. **Lógica:** anti-choque, watchdog, frenado sin comandos.
7. **Extras:** video, display, sonidos, etc.

Reglas de código:

- Pines y parámetros como constantes al principio del archivo.
- Credenciales (Wi-Fi, MQTT) en un archivo aparte que no se sube a GitHub.
- Versionar en Git: un commit por capa que funciona.

**Listo cuando:** cada capa funciona sola y todas juntas.

---

## Fase 6 · Pruebas

| Nivel | Qué se prueba | Dónde |
|---|---|---|
| Módulo | Cada pieza por separado | Banco |
| Integración | Todas las capas juntas | Banco, ruedas en el aire |
| Red | `ping`, broker, servidor, app | Red del robot |
| Campo | Manejo real, anti-choque, batería baja | Piso |

Registrar cada prueba:

| Fecha | Prueba | Resultado | Qué se cambió |
|---|---|---|---|
| | | | |

**Listo cuando:** pasan todas las pruebas de campo sin reinicios ni fallas.

---

## Fase 7 · Montaje y case

1. Diseñar el case (OpenSCAD) con las medidas reales de la Fase 1.
2. Imprimir un prototipo y ajustar.
3. Distribuir en el chasis: peso bajo y centrado, regulador lejos de la antena, interruptor accesible.
4. Fijar cables (precintos), sin tensión en los conectores.
5. **Repetir las pruebas de campo** después del montaje.

**Listo cuando:** el robot montado pasa de nuevo la Fase 6.

---

## Fase 8 · Documentación y cierre

- [ ] Ficha del robot actualizada.
- [ ] Diagrama y tabla de pines finales.
- [ ] Firmware final en GitHub, con versión.
- [ ] README con armado, conexiones y problemas frecuentes.
- [ ] Fotos del armado.
- [ ] Lecciones aprendidas: qué cambiaría en la próxima versión.

**Listo cuando:** otra persona puede armar el mismo robot con esta documentación.

---

## Seguridad (siempre)

- Conectar y desconectar con el robot **apagado**.
- Ajustar los reguladores **antes** de conectar placas.
- Baterías de litio: no sobredescargar, no cargar sin supervisión, no usar celdas hinchadas o golpeadas.
- Sin BMS: cortar a 3,2 V por celda y revisar cada celda por separado.
- Probar motores con las ruedas en el aire y nunca al borde de la mesa.
- Al programar con batería conectada: unir solo GND, TX y RX.

---

## Forma de trabajo con Claude

- Arrancar cada robot pidiendo la **Fase 0**.
- Avanzar de a una fase, paso a paso.
- Claude espera las instrucciones antes de sugerir, salvo riesgos de seguridad o de dañar el hardware (en una línea).
