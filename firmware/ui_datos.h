// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2026 Jorge Conde
// Generado por herramientas/generar_datos_firmware.py; no editar.
#pragma once

enum { UI_NUM, UI_OPCION, UI_BOOL };
typedef struct { const char* nombre; const char* unidad; uint8_t tipo, nOpciones; const char* const* opciones; } ui_param_t;

static const char* const UI_OP_0[] = { "Triangle", "Saw", "Square", "Random", "Soft Rnd" };
static const char* const UI_OP_3[] = { "Pitch", "Osc2 Pch", "Filter", "Pulse W", "FM", "Amp", "Pan" };
static const char* const UI_OP_6[] = { "Pitch", "Amp", "Filter" };
static const char* const UI_OP_10[] = { "Osc2 Pch", "FM", "Pulse W" };
static const char* const UI_OP_11[] = { "Saw", "Pulse", "Triangle", "Sine" };
static const char* const UI_OP_13[] = { "8'", "4'", "2'" };
static const char* const UI_OP_16[] = { "Saw", "Pulse", "Triangle", "Noise" };
static const char* const UI_OP_21[] = { "8'", "4'", "2'" };
static const char* const UI_OP_28[] = { "-2", "-1", "0", "+1", "+2" };
static const char* const UI_OP_32[] = { "Poly", "Mono", "Legato" };
static const char* const UI_OP_33[] = { "LP 12", "LP 24", "HP", "BP", "Notch" };
static const char* const UI_OP_60[] = { "1/16", "1/8 T", "1/8", "1/8 .", "1/4", "1/4 .", "1/2" };
static const char* const UI_OP_69[] = { "Up", "Down", "Up/Down", "Random", "Echo" };
static const char* const UI_OP_70[] = { "1/32", "1/16 T", "1/16", "1/8 T", "1/8", "1/4 T", "1/4", "1/2", "1/1" };
static const char* const UI_OP_72[] = { "1", "2", "3", "4" };
static const char* const UI_OP_75[] = { "Off", "Mod Wheel", "Velocity" };

static const ui_param_t UI_PARAMS[P_NUM] = {
    { "Wave", "", UI_OPCION, 5, UI_OP_0 },   // lfo1_onda
    { "Rate", "Hz", UI_NUM, 0, 0 },   // lfo1_vel
    { "Amount", "%", UI_NUM, 0, 0 },   // lfo1_cantidad
    { "Dest", "", UI_OPCION, 7, UI_OP_3 },   // lfo1_destino
    { "Rate", "Hz", UI_NUM, 0, 0 },   // lfo2_vel
    { "Amount", "%", UI_NUM, 0, 0 },   // lfo2_cantidad
    { "Dest", "", UI_OPCION, 3, UI_OP_6 },   // lfo2_destino
    { "Attack", "s", UI_NUM, 0, 0 },   // menv_ataque
    { "Decay", "s", UI_NUM, 0, 0 },   // menv_caida
    { "Amount", "%", UI_NUM, 0, 0 },   // menv_cantidad
    { "Dest", "", UI_OPCION, 3, UI_OP_10 },   // menv_destino
    { "Wave", "", UI_OPCION, 4, UI_OP_11 },   // osc1_onda
    { "Pulse W", "%", UI_NUM, 0, 0 },   // osc_pw
    { "Range", "", UI_OPCION, 3, UI_OP_13 },   // osc1_rango
    { "Semitone", "st", UI_NUM, 0, 0 },   // osc1_semi
    { "Fine", "ct", UI_NUM, 0, 0 },   // osc1_fino
    { "Wave", "", UI_OPCION, 4, UI_OP_16 },   // osc2_onda
    { "Semi", "st", UI_NUM, 0, 0 },   // osc2_semi
    { "Fine", "ct", UI_NUM, 0, 0 },   // osc2_fino
    { "Sync", "", UI_BOOL, 0, 0 },   // osc_sync
    { "Ring Mod", "", UI_BOOL, 0, 0 },   // osc_ring
    { "Range", "", UI_OPCION, 3, UI_OP_21 },   // osc2_rango
    { "Kbd Trk", "", UI_BOOL, 0, 0 },   // osc2_teclado
    { "FM", "%", UI_NUM, 0, 0 },   // osc_fm
    { "Unison", "%", UI_NUM, 0, 0 },   // unison
    { "Osc 1", "%", UI_NUM, 0, 0 },   // mix_osc1
    { "Osc 2", "%", UI_NUM, 0, 0 },   // mix_osc2
    { "Noise", "%", UI_NUM, 0, 0 },   // mix_ruido
    { "Octave", "", UI_OPCION, 5, UI_OP_28 },   // octava
    { "Glide", "%", UI_NUM, 0, 0 },   // glide
    { "Auto", "", UI_BOOL, 0, 0 },   // glide_auto
    { "Volume", "dB", UI_NUM, 0, 0 },   // volumen
    { "Mode", "", UI_OPCION, 3, UI_OP_32 },   // voz_modo
    { "Type", "", UI_OPCION, 5, UI_OP_33 },   // filtro_tipo
    { "Cutoff", "Hz", UI_NUM, 0, 0 },   // filtro_corte
    { "Reso", "%", UI_NUM, 0, 0 },   // filtro_reso
    { "Env Amt", "%", UI_NUM, 0, 0 },   // filtro_env
    { "Kbd Trk", "%", UI_NUM, 0, 0 },   // filtro_teclado
    { "Velocity", "%", UI_NUM, 0, 0 },   // filtro_vel
    { "Dist", "%", UI_NUM, 0, 0 },   // filtro_dist
    { "Attack", "s", UI_NUM, 0, 0 },   // fenv_a
    { "Decay", "s", UI_NUM, 0, 0 },   // fenv_d
    { "Sustain", "%", UI_NUM, 0, 0 },   // fenv_s
    { "Release", "s", UI_NUM, 0, 0 },   // fenv_r
    { "Attack", "s", UI_NUM, 0, 0 },   // aenv_a
    { "Decay", "s", UI_NUM, 0, 0 },   // aenv_d
    { "Sustain", "%", UI_NUM, 0, 0 },   // aenv_s
    { "Release", "s", UI_NUM, 0, 0 },   // aenv_r
    { "Velocity", "%", UI_NUM, 0, 0 },   // amp_vel
    { "Gain", "%", UI_NUM, 0, 0 },   // amp_gain
    { "On", "", UI_BOOL, 0, 0 },   // fx_dist_on
    { "Drive", "%", UI_NUM, 0, 0 },   // fx_dist_drive
    { "Tone", "%", UI_NUM, 0, 0 },   // fx_dist_tono
    { "Mix", "%", UI_NUM, 0, 0 },   // fx_dist_mezcla
    { "On", "", UI_BOOL, 0, 0 },   // fx_chorus_on
    { "Rate", "Hz", UI_NUM, 0, 0 },   // fx_chorus_vel
    { "Depth", "%", UI_NUM, 0, 0 },   // fx_chorus_prof
    { "Mix", "%", UI_NUM, 0, 0 },   // fx_chorus_mezcla
    { "On", "", UI_BOOL, 0, 0 },   // fx_delay_on
    { "Sync", "", UI_BOOL, 0, 0 },   // fx_delay_sync
    { "Time", "", UI_OPCION, 7, UI_OP_60 },   // fx_delay_div
    { "Free ms", "ms", UI_NUM, 0, 0 },   // fx_delay_ms
    { "Feedback", "%", UI_NUM, 0, 0 },   // fx_delay_fb
    { "Mix", "%", UI_NUM, 0, 0 },   // fx_delay_mezcla
    { "On", "", UI_BOOL, 0, 0 },   // fx_rev_on
    { "Size", "%", UI_NUM, 0, 0 },   // fx_rev_tam
    { "Damping", "%", UI_NUM, 0, 0 },   // fx_rev_amort
    { "Mix", "%", UI_NUM, 0, 0 },   // fx_rev_mezcla
    { "On", "", UI_BOOL, 0, 0 },   // arp_on
    { "Mode", "", UI_OPCION, 5, UI_OP_69 },   // arp_modo
    { "Rate", "", UI_OPCION, 9, UI_OP_70 },   // arp_div
    { "Gate", "%", UI_NUM, 0, 0 },   // arp_gate
    { "Range", "", UI_OPCION, 4, UI_OP_72 },   // arp_rango
    { "Sync", "", UI_BOOL, 0, 0 },   // arp_sync
    { "Free Hz", "Hz", UI_NUM, 0, 0 },   // arp_hz
    { "Source", "", UI_OPCION, 3, UI_OP_75 },   // morph_fuente
};
