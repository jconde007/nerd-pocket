// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2026 Jorge Conde
// Motor Pocket: los sonidos de Nerd Lead en C, sin JUCE. Corre a 44.1 kHz fijos, sin sobremuestreo (perfil Teensy del plugin).
// Una sola instancia, sin memoria dinámica. Todo se llama desde el mismo hilo que pocket_render.
#ifndef POCKET_H
#define POCKET_H

#include <stdint.h>
#include "tablas.h"

#define POCKET_SR 44100.0f

#ifndef POCKET_VOCES
#define POCKET_VOCES 8   // falta medir cuántas caben en el FM-1
#endif

void pocket_iniciar (void);

// Parámetros en unidades reales (Hz, s, %, semitonos); opciones y botones por índice o 0-1, como en parametros.json.
void  pocket_param (int id, float valor);
float pocket_leer_param (int id);

// Para la interfaz: de valor real a posición de perilla (0..1) y de vuelta, y la cantidad de Morph de cada parámetro.
float pocket_a_normal (int id, float valor);
float pocket_de_normal (int id, float n);
int   pocket_tiene_morph (int id);
float pocket_leer_morph (int id);
void  pocket_poner_morph (int id, float cantidad);

// Carga un programa del banco: todo a su valor inicial y luego lo que trae el programa.
void pocket_programa (int indice);
int  pocket_num_programas (void);
const char* pocket_nombre_programa (int indice);

void pocket_nota_on (int nota, float velocidad);   // velocidad 0..1
void pocket_nota_off (int nota);
void pocket_rueda (float valor);                   // 0..1
void pocket_bend (float valor);                    // -1..1
void pocket_bpm (float bpm);                       // lo usan el arpegiador y el delay sincronizados
// Reloj MIDI de afuera: con externo, el arpegiador sincronizado da sus pasos en los pulsos (24 por negra) en vez de con
// su propio reloj; el tempo para el gate y el delay sigue llegando por pocket_bpm. inicio = Start: la frase y la rejilla
// vuelven a empezar en el siguiente pulso.
void pocket_reloj_externo (int on);
void pocket_reloj_pulso (void);
void pocket_reloj_inicio (void);

// Llena L y R con n muestras.
void pocket_render (float* L, float* R, int n);

// Otro núcleo para la mitad de las voces: lanzar devuelve 1 si tomó el trabajo; esperar, 0 si lo terminó.
void pocket_paralelo (int (*lanzar) (void (*) (uint32_t), uint32_t), int (*esperar) (void));
// Apaga la voz que menos se oye; para llamarse cuando el bloque casi no alcanzó. pocket_alivios cuenta cuántas van.
void pocket_aliviar (void);
unsigned pocket_alivios (void);

int pocket_voces_activas (void);

// Recortes de costo (bits): 1 = osciladores a 1× con FM o sync, 2 = la mitad de voces de unison. 0 = como el plugin.
void pocket_ligero (int recortes);

// Solo pruebas: fase inicial fija de los osciladores (0..1; −1 = azar).
void pocket_fijar_fases (float fase1, float fase2);
// Solo pruebas: 1 si alguna voz está tocando esa nota.
int pocket_nota_suena (int nota);
// Solo pruebas: cuántos bytes ocupa el estado del motor.
unsigned pocket_tam_estado (void);
// Con POCKET_PERFIL: LFOs, voces, distorsión, chorus, delay, reverb y salida, en ticks de 24 MHz por bloque.
#define POCKET_PERFIL_PARTES 7
extern unsigned pocket_perfil[POCKET_PERFIL_PARTES];

#endif
