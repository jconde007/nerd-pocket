// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2026 Jorge Conde
// Generado por herramientas/generar_tablas.py; no editar.
#ifndef POCKET_TABLAS_H
#define POCKET_TABLAS_H

enum {
    P_LFO1_ONDA,
    P_LFO1_VEL,
    P_LFO1_CANTIDAD,
    P_LFO1_DESTINO,
    P_LFO2_VEL,
    P_LFO2_CANTIDAD,
    P_LFO2_DESTINO,
    P_MENV_ATAQUE,
    P_MENV_CAIDA,
    P_MENV_CANTIDAD,
    P_MENV_DESTINO,
    P_OSC1_ONDA,
    P_OSC_PW,
    P_OSC1_RANGO,
    P_OSC1_SEMI,
    P_OSC1_FINO,
    P_OSC2_ONDA,
    P_OSC2_SEMI,
    P_OSC2_FINO,
    P_OSC_SYNC,
    P_OSC_RING,
    P_OSC2_RANGO,
    P_OSC2_TECLADO,
    P_OSC_FM,
    P_UNISON,
    P_MIX_OSC1,
    P_MIX_OSC2,
    P_MIX_RUIDO,
    P_OCTAVA,
    P_GLIDE,
    P_GLIDE_AUTO,
    P_VOLUMEN,
    P_VOZ_MODO,
    P_FILTRO_TIPO,
    P_FILTRO_CORTE,
    P_FILTRO_RESO,
    P_FILTRO_ENV,
    P_FILTRO_TECLADO,
    P_FILTRO_VEL,
    P_FILTRO_DIST,
    P_FENV_A,
    P_FENV_D,
    P_FENV_S,
    P_FENV_R,
    P_AENV_A,
    P_AENV_D,
    P_AENV_S,
    P_AENV_R,
    P_AMP_VEL,
    P_AMP_GAIN,
    P_FX_DIST_ON,
    P_FX_DIST_DRIVE,
    P_FX_DIST_TONO,
    P_FX_DIST_MEZCLA,
    P_FX_CHORUS_ON,
    P_FX_CHORUS_VEL,
    P_FX_CHORUS_PROF,
    P_FX_CHORUS_MEZCLA,
    P_FX_DELAY_ON,
    P_FX_DELAY_SYNC,
    P_FX_DELAY_DIV,
    P_FX_DELAY_MS,
    P_FX_DELAY_FB,
    P_FX_DELAY_MEZCLA,
    P_FX_REV_ON,
    P_FX_REV_TAM,
    P_FX_REV_AMORT,
    P_FX_REV_MEZCLA,
    P_ARP_ON,
    P_ARP_MODO,
    P_ARP_DIV,
    P_ARP_GATE,
    P_ARP_RANGO,
    P_ARP_SYNC,
    P_ARP_HZ,
    P_MORPH_FUENTE,
    P_NUM
};

#define NUM_MORPH 28
#define NUM_PROGRAMAS 99
#define PROGRAMAS_POR_BANCO 99

typedef struct { float min, max, def, paso, skew; } def_param_t;
typedef struct { unsigned char id; float valor; } par_valor_t;
typedef struct { const char* nombre; const par_valor_t* valores; int n; const par_valor_t* morph; int nMorph; } programa_t;

extern const def_param_t TABLA_PARAMS[P_NUM];
extern const unsigned char TABLA_MORPH[NUM_MORPH];
extern const programa_t TABLA_PROGRAMAS[NUM_PROGRAMAS];

#endif
