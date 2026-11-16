// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2026 Jorge Conde
// Nerd Pocket: la mitad del contrato con la plataforma de fm1-base (device/unity.c, host/plat_host.c).
#pragma once
#include <stdint.h>
#include "plat.h"

#define APP_NAME "NERD POCKET"
#define APP_SPLASH_RGB RGB(192, 48, 58)
#define APP_BLOCK 256u                  // 5.8 ms a 44.1 kHz; el motor procesa hasta 256 de un jalón
#define APP_CANVAS_PX (240u * 64u)      // el motor ocupa casi todo el pool: la pantalla se dibuja por franjas de 64 filas
#define NP_MEDIDAS 0                    // 1: "screen" da CPU y tiempos para herramientas/medir_fm1.py, en vez de hablar
#define APP_OVERLOAD                    // el bloque pasó de 90 %: app_overload apaga una voz
#define APP_TOUCH                       // velocidad de las teclas (lib/touch.h)
#define APP_SPLASH                      // portada propia al prender (app_splash en ui.c)

// La flash se comparte con los datos de Felucca, que siguen ahí si se regresa a ella. Todo el store de la plataforma es
// suyo; la calibración del panel (la plataforma la exige y solo se escribe si se recalibra) va en su autosave, que Felucca
// da por perdido sin quejarse si no lo encuentra. Los ajustes van fuera del store (AJUSTES_FLASH en app.c).
enum { OBJ_PANEL, OBJ_NOBJ };
#define APP_STORE_MAP {0xE5000u}

// La plataforma llama a estas.
void app_init (void);
void app_render (int32_t* lr, uint32_t n, uint32_t master_q12);   // interrupción de audio: n cuadros estéreo de 24 bits
void app_panic (void);
void app_overload (void);
void app_ui_init (void);
void app_frame (void);
void app_input_only (void);
void app_say (const char* a, const char* b);
void app_splash (void);

// Comprobaciones del simulador ("expect"): programa, octava, voces, nota.
#define APP_HOST_EXPECT
int app_expect (const char* que, int* valor);
