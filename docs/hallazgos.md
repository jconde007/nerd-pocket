# Lo que permite el FM-1 (revisado el 2026-10-30, FM1 Open commit 4c7496d)

## Comprobado en el código
| Tema | Dato | Dónde |
|---|---|---|
| Pantalla | 240 × 240, color RGB565, se dibuja pixel por pixel | `sdk/include/fm1.h` |
| Velocidad de dibujo | Bandas de hasta 240 × 16 por DMA, sin bloquear el audio | `fm1_screen_submit` |
| LEDs | 41 (14 botones, 27 teclas), solo encendido o apagado | `fm1_led_set(key, on)` |
| Teclas | 27, de F3 a G5 | `struct fm1_controls` |
| Botones | 14: OCT−, OCT+, FX, SEL, ENV, LFO, EDIT, GLO, HOME, SAVE, ARP, SEQ, PLAY, REC | `sdk/Simulator.js` |
| Perillas | 7 encoders y una perilla de volumen | `struct fm1_controls`, `app/board/panel.c` |
| Audio | 44.1 kHz, bloques de 128 muestras, 24 bits, sin entrada analógica | `fm1_block.h`, `fm1_audio.h` |
| Memoria | ~511 KB de RAM, 1 MB de código | `sdk/config/linker.ld.in` |
| Decimales | El compilador activa FPU de precisión simple y el 303 usa `float` | `sdk/Build.ps1`, `third_party/synth/bass303.c` |
| Motor del aparato | Biblioteca binaria cerrada, `libfm1_engine.a` | `LICENSE.md`, `sdk/synth.json` |
| Punto para sumar audio | `fm1_output_mix`, código abierto, corre en cada bloque | `app/audio_output.c` |
| Regreso a fábrica | La herramienta ofrece volver a V15 | `sdk/Update.html.in` |

## Consecuencias
- La interfaz gráfica es libre: el firmware abierto dibuja toda la pantalla.
- El color de los LEDs no se cambia por software, porque cada uno recibe un solo bit. Se pueden prender, apagar y hacer
  parpadear. El color real solo se confirma viendo el aparato.
- Un motor propio no entra dentro de la biblioteca cerrada. La vía a probar es generar nuestras voces y sumarlas en
  `fm1_output_mix`, dentro del presupuesto del bloque.

## Falta medir en un FM-1
- Velocidad del procesador (`fm1_device_clock`) y CPU libre (`groove_audio_stats`).
- Cuántas voces del motor caben sin cortes.
- Compilar y flashear el firmware desde Windows; nada de esto se ha probado todavía.
