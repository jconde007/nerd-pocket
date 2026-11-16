// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2026 Jorge Conde
// La interfaz aprobada en ui/prototipo.html: arriba una pantalla oscura con el dato principal, su gráfica y un selector de LEDs;
// abajo un módulo con cuatro perillas, en el orden de K1 a K4. Solo se redibuja la zona que cambió, por franjas de 64 filas.
#include "ui_datos.h"

#define C_ROJO     RGB (192, 48, 58)
#define C_ROJO2    RGB (165, 35, 45)
#define C_BANDA    RGB (144, 148, 187)
#define C_BANDA2   RGB (122, 126, 168)
#define C_LAVANDA  RGB (202, 203, 224)
#define C_LAVANDA2 RGB (143, 142, 166)
#define C_PASTILLA RGB (111, 116, 168)
#define C_MARINO   RGB (46, 45, 69)
#define C_NEGRO    RGB (30, 27, 34)
#define C_BOTON    RGB (58, 54, 64)
#define C_VERDE    RGB (105, 197, 77)
#define C_LEDOFF   RGB (28, 26, 32)
#define C_SEG      RGB (255, 51, 38)
#define C_SEGOFF   RGB (23, 4, 4)
#define C_DISPLAY  RGB (23, 4, 4)
#define C_REJA     RGB (58, 20, 20)
#define C_ARCO     RGB (224, 31, 44)
#define C_ORO      RGB (184, 134, 11)

enum { G_ONDA, G_OSC1, G_OSC2, G_MIXER, G_FILTRO, G_AENV, G_FENV, G_MENV, G_LFO1, G_LFO2, G_DRIVE, G_CHORUS, G_DELAY, G_REVERB, G_ARP, G_VOZ, G_MORPH, G_SEQ };
#define NINGUNO (-1)

typedef struct { const char* titulo; int16_t k[4]; uint8_t grafica; int16_t lista, destacado, led; } vista_t;

static const vista_t PANELES[] = {
    { "nerd pocket",  { P_FILTRO_CORTE, P_FILTRO_RESO, P_FILTRO_ENV, P_AENV_D }, G_ONDA, NINGUNO, NINGUNO, NINGUNO },
    { "oscillator 1", { P_OSC1_ONDA, P_OSC1_RANGO, P_OSC1_SEMI, P_OSC1_FINO }, G_OSC1, P_OSC1_ONDA, NINGUNO, NINGUNO },
    { "oscillator 2", { P_OSC2_ONDA, P_OSC2_RANGO, P_OSC2_SEMI, P_OSC2_FINO }, G_OSC2, P_OSC2_ONDA, NINGUNO, NINGUNO },
    { "sync / ring",  { P_OSC_SYNC, P_OSC_RING, P_OSC2_TECLADO, P_OSC_PW }, G_OSC2, NINGUNO, NINGUNO, NINGUNO },
    { "mixer",        { P_MIX_OSC1, P_MIX_OSC2, P_MIX_RUIDO, P_OSC_FM }, G_MIXER, NINGUNO, NINGUNO, NINGUNO },
    { "filter",       { P_FILTRO_TIPO, P_FILTRO_CORTE, P_FILTRO_RESO, P_FILTRO_ENV }, G_FILTRO, P_FILTRO_TIPO, P_FILTRO_CORTE, NINGUNO },
    { "filter",       { P_FILTRO_TECLADO, P_FILTRO_VEL, P_FILTRO_DIST, NINGUNO }, G_FILTRO, NINGUNO, P_FILTRO_CORTE, NINGUNO },
    { "amplifier",    { P_AENV_A, P_AENV_D, P_AENV_S, P_AENV_R }, G_AENV, NINGUNO, NINGUNO, NINGUNO },
    { "amplifier",    { P_AMP_VEL, P_AMP_GAIN, NINGUNO, NINGUNO }, G_AENV, NINGUNO, NINGUNO, NINGUNO },
    { "filter env",   { P_FENV_A, P_FENV_D, P_FENV_S, P_FENV_R }, G_FENV, NINGUNO, NINGUNO, NINGUNO },
    { "mod env",      { P_MENV_ATAQUE, P_MENV_CAIDA, P_MENV_CANTIDAD, P_MENV_DESTINO }, G_MENV, P_MENV_DESTINO, NINGUNO, NINGUNO },
    { "lfo 1",        { P_LFO1_ONDA, P_LFO1_VEL, P_LFO1_CANTIDAD, P_LFO1_DESTINO }, G_LFO1, P_LFO1_ONDA, P_LFO1_VEL, NINGUNO },
    { "lfo 2",        { P_LFO2_VEL, P_LFO2_CANTIDAD, P_LFO2_DESTINO, NINGUNO }, G_LFO2, P_LFO2_DESTINO, NINGUNO, NINGUNO },
    { "drive",        { P_FX_DIST_ON, P_FX_DIST_DRIVE, P_FX_DIST_TONO, P_FX_DIST_MEZCLA }, G_DRIVE, NINGUNO, P_FX_DIST_DRIVE, P_FX_DIST_ON },
    { "chorus",       { P_FX_CHORUS_ON, P_FX_CHORUS_VEL, P_FX_CHORUS_PROF, P_FX_CHORUS_MEZCLA }, G_CHORUS, NINGUNO, P_FX_CHORUS_VEL, P_FX_CHORUS_ON },
    { "delay",        { P_FX_DELAY_ON, P_FX_DELAY_SYNC, P_FX_DELAY_DIV, P_FX_DELAY_MS }, G_DELAY, NINGUNO, P_FX_DELAY_DIV, P_FX_DELAY_ON },
    { "delay",        { P_FX_DELAY_FB, P_FX_DELAY_MEZCLA, NINGUNO, NINGUNO }, G_DELAY, NINGUNO, P_FX_DELAY_FB, P_FX_DELAY_ON },
    { "reverb",       { P_FX_REV_ON, P_FX_REV_TAM, P_FX_REV_AMORT, P_FX_REV_MEZCLA }, G_REVERB, NINGUNO, P_FX_REV_TAM, P_FX_REV_ON },
    { "arpeggiator",  { P_ARP_ON, P_ARP_MODO, P_ARP_DIV, P_ARP_GATE }, G_ARP, P_ARP_MODO, P_ARP_DIV, P_ARP_ON },
    { "arpeggiator",  { P_ARP_RANGO, P_ARP_SYNC, P_ARP_HZ, NINGUNO }, G_ARP, P_ARP_RANGO, P_ARP_HZ, P_ARP_ON },
    { "voice",        { P_VOZ_MODO, P_OCTAVA, P_GLIDE, P_GLIDE_AUTO }, G_VOZ, P_VOZ_MODO, P_GLIDE, NINGUNO },
    { "voice",        { P_UNISON, P_VOLUMEN, P_MORPH_FUENTE, NINGUNO }, G_MORPH, P_MORPH_FUENTE, P_UNISON, NINGUNO },
    { "sequencer",    { NINGUNO, NINGUNO, NINGUNO, NINGUNO }, G_SEQ, NINGUNO, NINGUNO, NINGUNO },
};
#define N_PANELES ((int) (sizeof PANELES / sizeof PANELES[0]))

// Cada botón abre un módulo; volver a pulsarlo pasa a su siguiente panel.
static const struct { uint8_t boton, primero, cuantos; } FAMILIAS[] = {
    { B_HOME, 0, 1 }, { B_EDIT, 1, 6 }, { B_ENV, 7, 4 }, { B_LFO, 11, 2 }, { B_FX, 13, 5 }, { B_ARP, 18, 2 }, { B_GLO, 20, 2 }, { B_SEQ, 22, 1 },
};
#define N_FAMILIAS ((int) (sizeof FAMILIAS / sizeof FAMILIAS[0]))
#define PANEL_SEQ 22

#define X_PLACA 6
#define Y_PLACA 26
#define W_PLACA 228
#define H_PLACA 106
#define Y_MODULO 136
#define Y_CELDA 152
#define W_LISTA 72
#define TECLAS_MAX NKEYS

static struct
{
    uint32_t teclas, botones, avisoHasta, focoHasta, proximoOnda;
    uint8_t nota[NKEYS], sucio, sucioCab, sucioPlaca, sucioMod, recargar;
    int16_t foco;
    int panel, cursor, pasoVisto;
    uint32_t tiempoVisto, estadoVisto, cambiosMidi;
    uint32_t introDesde, proximoIntro;   // la portada animada de los primeros segundos
    uint8_t intro;
    float introFase;                 // 0..1 a lo largo de la portada          // con reloj de afuera: el último tiempo (negra) mostrado en la celda Clock
    uint8_t grabando, focoSeq, editado;
    float rueda, par[P_NUM], mor[P_NUM];
    char aviso[24];
    int16_t trazo[W_PLACA], trazo2[W_PLACA];
    int nTrazo;
    uint32_t cuadros, fps, marcaFps;
} ui;

static float senoArco[61], cosenoArco[61];

// ---------- Texto ----------
static char* poner (char* d, const char* s) { while (*s) *d++ = *s++; *d = 0; return d; }

static char* poner_entero (char* d, int32_t v)
{
    char t[12];
    int n = 0;
    uint32_t u = (uint32_t) (v < 0 ? -v : v);
    do t[n++] = (char) ('0' + u % 10u); while ((u /= 10u) && n < 11);
    if (v < 0) *d++ = '-';
    while (n) *d++ = t[--n];
    *d = 0;
    return d;
}

static char* poner_decimal (char* d, float v, int decimales)
{
    const int32_t escala = decimales == 2 ? 100 : 10;
    const int32_t c = (int32_t) (fm_fabsf (v) * (float) escala + 0.5f);
    if (v < 0.0f && c) *d++ = '-';
    d = poner_entero (d, c / escala);
    *d++ = '.';
    int32_t f = c % escala;
    if (decimales == 2) *d++ = (char) ('0' + f / 10);
    *d++ = (char) ('0' + f % 10);
    *d = 0;
    return d;
}

static void texto_param (int id, char* d)
{
    const ui_param_t* p = &UI_PARAMS[id];
    const def_param_t* t = &TABLA_PARAMS[id];
    const float v = ui.par[id];
    if (p->tipo == UI_OPCION) { poner (d, p->opciones[(int) (v + 0.5f)]); return; }
    if (p->tipo == UI_BOOL) { poner (d, v > 0.5f ? "On" : "Off"); return; }
    if (p->unidad[0] == 's' && ! p->unidad[1])
    {
        if (v < 1.0f) poner (poner_entero (d, (int32_t) (v * 1000.0f + 0.5f)), " ms");
        else poner (poner_decimal (d, v, 2), " s");
        return;
    }
    if (p->unidad[0] == 'H')
    {
        if (v >= 1000.0f) poner (poner_decimal (d, v * 0.001f, 2), " kHz");
        else if (v < 10.0f) poner (poner_decimal (d, v, 2), " Hz");
        else poner (poner_entero (d, (int32_t) (v + 0.5f)), " Hz");
        return;
    }
    const int entero = t->paso > 0.0f || fm_fabsf (v) >= 10.0f || t->max - t->min >= 60.0f;
    char* e = entero ? poner_entero (d, (int32_t) (v + (v < 0.0f ? -0.5f : 0.5f))) : poner_decimal (d, v, 1);
    if (p->unidad[0] == '%') poner (e, "%");
    else if (p->unidad[0]) { *e++ = ' '; poner (e, p->unidad); }
}

static void centrado (const felucca_font_t* f, const char* s, int32_t cx, int32_t y, uint16_t k) { cv_text (cx - text_w (f, s) / 2, y, f, s, k); }
static void a_derecha (const felucca_font_t* f, const char* s, int32_t x, int32_t y, uint16_t k) { cv_text (x - text_w (f, s), y, f, s, k); }

// ---------- Cabecera ----------
// 0-9, A (10) y b (11): A01…A99 y b01…b99, como en el panel del plugin.
static const char* const SEGMENTOS[12] = { "abcdef", "bc", "abged", "abgcd", "fgbc", "afgcd", "afgedc", "abc", "abcdefg", "abcdfg", "abcefg", "fgedc" };
static void digito (int i, int32_t x, int32_t y)
{
    const int32_t a = 9, h = 15, t = 2, m = (h - t) / 2;
    const int32_t s[7][4] = { { x + t, y, a - 2 * t, t }, { x + a - t, y + t, t, m - t }, { x + a - t, y + m + t, t, m - t }, { x + t, y + h - t, a - 2 * t, t },
                              { x, y + m + t, t, m - t }, { x, y + t, t, m - t }, { x + t, y + m, a - 2 * t, t } };
    const char* on = SEGMENTOS[i];
    for (int k = 0; k < 7; ++k)
    {
        int prendido = 0;
        for (const char* c = on; *c; ++c) if (*c == 'a' + k) prendido = 1;
        cv_rect (s[k][0], s[k][1], s[k][2], s[k][3], prendido ? C_SEG : C_SEGOFF);
    }
}

static void dibujar_cabecera (void)
{
    cv_rect (4, 3, 39, 19, C_DISPLAY);
    const uint32_t n = ajustes.programa % PROGRAMAS_POR_BANCO + 1u;
    digito (ajustes.programa < PROGRAMAS_POR_BANCO ? 10 : 11, 7, 5);
    digito ((int) (n / 10u), 19, 5);
    digito ((int) (n % 10u), 31, 5);
    const int32_t fin = cv_text (49, 4, &FONT_B, plat_ms() < ui.avisoHasta ? ui.aviso : pocket_nombre_programa (ajustes.programa), C_WHITE);
    if (ui.editado && plat_ms() >= ui.avisoHasta) cv_text (fin + 2, 4, &FONT_B, "*", C_WHITE);
    // Estado del secuenciador, a la vista en todas las pantallas (el LED de PLAY no se ve en todos los aparatos): una pastilla verde
    // "PLAY" o gris "STOP" a la derecha, y con reloj de afuera, un punto que parpadea en cada tiempo (blanco en el 1 del compás).
    const int tocando = seqTocando && transporte;
    const int xp = 236 - 46;
    cv_rect (xp - (relojExterno ? 14 : 0) - 2, 3, 236 - xp + (relojExterno ? 14 : 0) + 2, 19, C_ROJO);   // tapa el nombre si es largo
    cv_rect (xp, 3, 46, 19, tocando ? C_VERDE : C_MARINO);
    centrado (&FONT_B, tocando ? "PLAY" : "STOP", xp + 23, 4, tocando ? C_NEGRO : C_LAVANDA2);
    if (relojExterno)
    {
        const uint32_t p = pulsosSeq ? pulsosSeq - 1u : 0u;
        const int pulso = p % 24u < 6u, uno = p % 96u < 6u;
        cv_rect (xp - 12, 8, 9, 9, C_NEGRO);
        cv_rect (xp - 11, 9, 7, 7, pulso ? (uno ? C_WHITE : C_SEG) : C_LEDOFF);
    }
}

// ---------- Gráficas: se calculan una vez y luego se dibujan franja por franja ----------
static void trazar_columna (int16_t* t, int w, int y, int h, int i, float v) { (void) w; t[i] = (int16_t) (y + h - 3 - (int32_t) (fm_clampf (v, 0.0f, 1.0f) * (float) (h - 6))); }

static float onda_osc (int o, float p, float pw)
{
    if (o == 0) return p;
    if (o == 1) return p >= 1.0f - pw ? 0.92f : 0.08f;
    if (o == 2) return p < 0.5f ? 2.0f * p : 2.0f - 2.0f * p;
    return 0.5f + 0.5f * fm_sinf (6.2831853f * p);
}

static float normal_de (int id) { return fm_clampf (pocket_a_normal (id, ui.par[id]), 0.0f, 1.0f); }

static void calcular_grafica (int tipo, int y, int w, int h)
{
    const float* P = ui.par;
    ui.nTrazo = 0;
    switch (tipo)
    {
        case G_ONDA:
        {
            int i0 = 0;
            for (int i = 1; i < APP_BLOCK - w; ++i) if (ondaViva[i - 1] <= 0.0f && ondaViva[i] > 0.0f) { i0 = i; break; }
            float pico = 0.02f;
            for (int i = 0; i < w; ++i) { const float a = fm_fabsf (ondaViva[i0 + i]); if (a > pico) pico = a; }
            const float g = fm_minf (0.46f / pico, 12.0f);
            for (int i = 0; i < w; ++i) trazar_columna (ui.trazo, w, y, h, i, 0.5f + ondaViva[i0 + i] * g);
            ui.nTrazo = 1;
            break;
        }
        case G_OSC1:
        case G_OSC2:
        {
            const int o = (int) (P[tipo == G_OSC1 ? P_OSC1_ONDA : P_OSC2_ONDA] + 0.5f), ruido = tipo == G_OSC2 && o == 3;
            uint32_t semilla = 7u;
            for (int i = 0; i < w; ++i)
            {
                float u = 3.0f * (float) i / (float) w;
                u -= (float) (int) u;
                semilla = semilla * 1664525u + 1013904223u;
                trazar_columna (ui.trazo, w, y, h, i, ruido ? (float) (semilla >> 24) / 255.0f : onda_osc (o, u, P[P_OSC_PW] * 0.01f));
            }
            ui.nTrazo = 1;
            break;
        }
        case G_FILTRO:
        {
            const int t = (int) (P[P_FILTRO_TIPO] + 0.5f);
            const float fc = fm_log2f (P[P_FILTRO_CORTE] / 20.0f) / 9.96578428f, q = P[P_FILTRO_RESO] * 0.01f;
            for (int i = 0; i < w; ++i)
            {
                const float u = (float) i / (float) w, d = (u - fc) * 9.0f, e = (u - fc) * 22.0f, pico = q * 0.42f * fm_expf (-e * e);
                const float lp1 = d < 0.0f ? 1.0f : fm_expf (-d * 0.45f), lp2 = d < 0.0f ? 1.0f : fm_expf (-d * 0.9f), hp = d > 0.0f ? 1.0f : fm_expf (d * 1.8f);
                float v;
                if (t == 0) v = 0.55f * lp1 + pico;
                else if (t == 1) v = 0.55f * lp2 + pico;
                else if (t == 2) v = 0.55f * hp + pico;
                else if (t == 3) v = 0.55f * (d < 0.0f ? 1.0f : fm_expf (-d * 0.675f)) * (d > 0.0f ? 1.0f : fm_expf (d * 1.5f)) + pico;
                else v = 0.55f * lp1 * (1.0f - fm_expf (-4.0f * d * d) * (1.0f - q)) + pico * 0.5f;
                trazar_columna (ui.trazo, w, y, h, i, v);
            }
            ui.nTrazo = 1;
            break;
        }
        case G_AENV:
        case G_FENV:
        {
            const int b = tipo == G_AENV ? P_AENV_A : P_FENV_A;
            const float a = normal_de (b) * 0.27f + 0.01f, d = normal_de (b + 1) * 0.27f + 0.01f, r = normal_de (b + 3) * 0.27f + 0.01f, s = P[b + 2] * 0.01f, sus = 1.0f - a - d - r;
            for (int i = 0; i < w; ++i)
            {
                const float u = (float) i / (float) w;
                trazar_columna (ui.trazo, w, y, h, i, u < a ? u / a : u < a + d ? s + (1.0f - s) * fm_expf (-4.0f * (u - a) / d) : u < a + d + sus ? s : s * fm_expf (-4.0f * (u - a - d - sus) / r));
            }
            ui.nTrazo = 1;
            break;
        }
        case G_MENV:
        {
            const float a = normal_de (P_MENV_ATAQUE) * 0.4f + 0.01f, d = normal_de (P_MENV_CAIDA) * 0.55f + 0.01f, c = 0.1f + fm_fabsf (P[P_MENV_CANTIDAD]) * 0.009f;
            for (int i = 0; i < w; ++i)
            {
                const float u = (float) i / (float) w;
                trazar_columna (ui.trazo, w, y, h, i, c * (u < a ? u / a : fm_expf (-4.0f * (u - a) / d)));
            }
            ui.nTrazo = 1;
            break;
        }
        case G_LFO1:
        case G_LFO2:
        {
            const int o = tipo == G_LFO1 ? (int) (P[P_LFO1_ONDA] + 0.5f) : 0;
            const float ciclos = 1.0f + normal_de (tipo == G_LFO1 ? P_LFO1_VEL : P_LFO2_VEL) * 7.0f, c = 0.15f + P[tipo == G_LFO1 ? P_LFO1_CANTIDAD : P_LFO2_CANTIDAD] * 0.0085f;
            for (int i = 0; i < w; ++i)
            {
                const float x = ciclos * (float) i / (float) w;
                const int n = (int) x;
                const float p = x - (float) n;
                const float v = o == 1 ? 1.0f - 2.0f * p : o == 2 ? (p < 0.5f ? 1.0f : -1.0f) : o == 3 ? (float) (n * 7919 % 13) / 6.0f - 1.0f
                              : o == 4 ? fm_sinf ((float) n * 2.1f + p) : p < 0.5f ? 4.0f * p - 1.0f : 3.0f - 4.0f * p;
                trazar_columna (ui.trazo, w, y, h, i, 0.5f + 0.48f * c * v);
            }
            ui.nTrazo = 1;
            break;
        }
        case G_DRIVE:
        {
            const float g = 1.0f + P[P_FX_DIST_DRIVE] * 0.09f, n = fm_tanhf (g), m = P[P_FX_DIST_MEZCLA] * 0.01f;
            for (int i = 0; i < w; ++i)
            {
                const float e = 2.0f * (float) i / (float) w - 1.0f;
                trazar_columna (ui.trazo, w, y, h, i, 0.5f + 0.48f * (e * (1.0f - m) + m * fm_tanhf (e * g) / n));
            }
            ui.nTrazo = 1;
            break;
        }
        case G_CHORUS:
        {
            const float c = 1.0f + normal_de (P_FX_CHORUS_VEL) * 5.0f, a = P[P_FX_CHORUS_PROF] * 0.0042f;
            for (int i = 0; i < w; ++i)
            {
                const float s = a * fm_sinf (6.2831853f * c * (float) i / (float) w);
                trazar_columna (ui.trazo, w, y, h, i, 0.5f + s);
                trazar_columna (ui.trazo2, w, y, h, i, 0.5f - s);
            }
            ui.nTrazo = 2;
            break;
        }
        case G_REVERB:
        {
            const float larga = 1.0f + P[P_FX_REV_TAM] * 0.07f, am = P[P_FX_REV_AMORT] * 0.01f, nivel = 0.25f + 0.7f * P[P_FX_REV_MEZCLA] * 0.01f;
            for (int i = 0; i < w; ++i)
            {
                const float u = (float) i / (float) w;
                trazar_columna (ui.trazo, w, y, h, i, nivel * fm_expf (-u * (9.0f - larga) * (1.0f + am)) * (0.75f + 0.25f * fm_sinf (u * 90.0f)));
            }
            ui.nTrazo = 1;
            break;
        }
        case G_VOZ:
        {
            const float g = normal_de (P_GLIDE), dur = 0.02f + g * 0.6f;
            for (int i = 0; i < w; ++i)
            {
                const float u = (float) i / (float) w;
                trazar_columna (ui.trazo, w, y, h, i, u < 0.2f ? 0.2f : u < 0.2f + dur ? 0.2f + 0.6f * (u - 0.2f) / dur : 0.8f);
            }
            ui.nTrazo = 1;
            break;
        }
        default:
            break;
    }
}

static void dibujar_grafica (int tipo, int x, int y, int w, int h)
{
    for (int i = 1; i < 4; ++i) cv_rect (x + i * w / 4, y, 1, h, C_REJA);
    cv_rect (x, y + h - 1, w, 1, C_REJA);
    for (int t = 0; t < ui.nTrazo; ++t)
    {
        const int16_t* tr = t ? ui.trazo2 : ui.trazo;
        const uint16_t k = t ? C_LAVANDA2 : C_SEG;
        for (int i = 1; i < w; ++i)
        {
            const int a = tr[i - 1], b = tr[i], lo = a < b ? a : b, hi = a < b ? b : a;
            cv_rect (x + i, lo, 1, hi - lo + 2, k);
        }
    }
    if (tipo == G_MIXER)
    {
        static const char* const N[4] = { "OSC 1", "OSC 2", "NOISE", "FM" };
        const float v[4] = { ui.par[P_MIX_OSC1], ui.par[P_MIX_OSC2], ui.par[P_MIX_RUIDO], ui.par[P_OSC_FM] };
        for (int i = 0; i < 4; ++i)
        {
            const int cx = x + (2 * i + 1) * w / 8, a = (int) ((float) (h - 16) * v[i] * 0.01f);
            cv_rect (cx - 14, y + h - 14 - a, 28, a, C_SEG);
            centrado (&FONT_XS, N[i], cx, y + h - 15, C_LAVANDA);
        }
    }
    else if (tipo == G_DELAY)
    {
        const float fb = ui.par[P_FX_DELAY_FB] * 0.01f, an = ui.par[P_FX_DELAY_SYNC] > 0.5f ? (ui.par[P_FX_DELAY_DIV] + 2.0f) * 0.125f : normal_de (P_FX_DELAY_MS);
        const float paso = fm_maxf (8.0f, an * (float) w / 3.0f);
        float v = 1.0f;
        for (int i = 0; 6.0f + (float) i * paso < (float) w - 4.0f && v > 0.03f; ++i, v *= fb)
        {
            const int b = (int) ((float) (h - 6) * v * (i ? ui.par[P_FX_DELAY_MEZCLA] * 0.01f + 0.2f : 1.0f));
            cv_rect (x + 6 + (int) ((float) i * paso), y + h - 2 - b, 5, b, i ? C_SEG : C_WHITE);
        }
    }
    else if (tipo == G_ARP)
    {
        const int notas = 4 * ((int) (ui.par[P_ARP_RANGO] + 0.5f) + 1), modo = (int) (ui.par[P_ARP_MODO] + 0.5f), ciclo = 2 * notas - 2;
        const float an = (float) (w - 4) / 16.0f;
        for (int i = 0; i < 16; ++i)
        {
            const int c = i % ciclo, u = modo == 1 ? notas - 1 - i % notas : modo == 2 ? (c < notas ? c : ciclo - c) : modo == 3 ? i * 7919 % 13 % notas : i % notas;
            const int ancho = (int) fm_maxf (2.0f, (an - 1.0f) * ui.par[P_ARP_GATE] * 0.01f);
            cv_rect (x + 2 + (int) ((float) i * an), y + h - 7 - u * (h - 12) / (notas - 1), ancho, 4, C_SEG);
        }
    }
    else if (tipo == G_MORPH)
    {
        char t[24], *e = t;
        int asignadas = 0;
        for (int i = 0; i < P_NUM; ++i) if (ui.mor[i] != 0.0f) ++asignadas;
        cv_text (x, y + 2, &FONT_XS, "Mod Wheel", C_LAVANDA);
        poner (poner_entero (t, (int32_t) (ui.rueda * 100.0f + 0.5f)), "%");
        a_derecha (&FONT_XS, t, x + w, y + 2, C_SEG);
        cv_rect (x, y + 16, w, 8, C_REJA);
        cv_rect (x, y + 16, (int) ((float) w * ui.rueda), 8, C_SEG);
        e = poner_entero (t, asignadas);
        poner (e, " knobs in morph");
        cv_text (x, y + 30, &FONT_XS, t, C_LAVANDA);
        cv_text (x, y + 47, &FONT_XS, "Hold SEL, turn K1-K4", C_LAVANDA2);
    }
}

// ---------- Pantalla de arriba ----------
static void led_lista (int x, int y, int prendido, const char* nombre)
{
    cv_rect (x, y + 2, 7, 7, C_BLACK);
    cv_rect (x + 1, y + 3, 5, 5, prendido ? C_VERDE : C_LEDOFF);
    cv_text (x + 11, y - 1, &FONT_XS, nombre, prendido ? C_WHITE : C_LAVANDA2);
}

static int foco_vigente (void) { return ui.foco != NINGUNO && plat_ms() < ui.focoHasta; }

static int ancho_grafica (const vista_t* p)
{
    return p->lista != NINGUNO || ui.panel == 0 || (foco_vigente() && UI_PARAMS[ui.foco].tipo == UI_OPCION) ? W_PLACA - W_LISTA : W_PLACA;
}

static int param_destacado (const vista_t* p)
{
    if (foco_vigente()) return ui.foco;
    if (p->destacado != NINGUNO) return p->destacado;
    for (int i = 0; i < 4; ++i) if (p->k[i] != NINGUNO && UI_PARAMS[p->k[i]].tipo == UI_NUM) return p->k[i];
    return p->k[0];
}

static const char* const NOMBRE_NOTA[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
static void texto_nota (int n, char* d) { poner_entero (poner (d, NOMBRE_NOTA[n % 12]), n / 12 - 1); }

static void dibujar_placa_seq (void)
{
    char t[32];
    const paso_t* s = &ajustes.pasos[ui.cursor];
    const int tocando = seqTocando && transporte, paso = seqPaso;
    cv_rect (X_PLACA, Y_PLACA, W_PLACA, H_PLACA, C_DISPLAY);
    if (relojExterno)
    {
        // Diagnóstico del reloj de afuera: S Start, P Stop, C Continue, H el hueco más largo entre pulsos (ms), X relojes perdidos.
        char *d = t;
        d = poner_entero (poner (d, "S"), (int) diagStart);   d = poner_entero (poner (d, " P"), (int) diagStop);
        d = poner_entero (poner (d, " C"), (int) diagCont);   d = poner_entero (poner (d, " H"), (int) diagHueco);
        poner_entero (poner (d, " X"), (int) diagPerdido);
        cv_text (X_PLACA + 5, Y_PLACA + 6, &FONT_B, t, C_LAVANDA);
    }
    else
    {
        poner_entero (poner (t, "Step "), ui.cursor + 1);
        cv_text (X_PLACA + 5, Y_PLACA + 6, &FONT_B, t, C_LAVANDA);
    }
    if (ui.grabando) cv_text (X_PLACA + 130, Y_PLACA + 6, &FONT_B, "REC", C_SEG);
    if (s->on) texto_nota (s->nota, t); else poner (t, "Rest");
    a_derecha (&FONT_M, t, X_PLACA + W_PLACA - 5, Y_PLACA + 2, C_SEG);
    int baja = 36, alta = 60;
    for (int i = 0; i < N_PASOS; ++i)
        if (ajustes.pasos[i].on)
        {
            if (ajustes.pasos[i].nota < baja) baja = ajustes.pasos[i].nota;
            if (ajustes.pasos[i].nota > alta) alta = ajustes.pasos[i].nota;
        }
    const int x0 = X_PLACA + 4, y0 = Y_PLACA + 28, w = W_PLACA - 8, h = H_PLACA - 32, an = w / N_PASOS;
    for (int i = 0; i < N_PASOS; ++i)
    {
        const int x = x0 + i * an;
        const paso_t* e = &ajustes.pasos[i];
        if (i % 4 == 0) cv_rect (x, y0, 1, h, C_REJA);
        if (tocando && i == paso) cv_rect (x, y0, an, h, C_REJA);
        if (e->on)
        {
            const int yy = y0 + h - 10 - (e->nota - baja) * (h - 16) / (alta - baja), ancho = (an - 2) * e->gate / 80;
            cv_rect (x + 1, yy, ancho < 3 ? 3 : (ancho > an - 2 ? an - 2 : ancho), 5, tocando && i == paso ? C_WHITE : C_SEG);
        }
        if (i == ui.cursor) cv_rect (x + 1, y0 + h - 2, an - 2, 2, C_VERDE);
    }
}

static void dibujar_placa (void)
{
    if (ui.panel == PANEL_SEQ) { dibujar_placa_seq(); return; }
    const vista_t* p = &PANELES[ui.panel];
    const int ancho = ancho_grafica (p), id = param_destacado (p);
    char t[24];
    cv_rect (X_PLACA, Y_PLACA, W_PLACA, H_PLACA, C_DISPLAY);
    int lista = foco_vigente() && UI_PARAMS[ui.foco].tipo == UI_OPCION ? ui.foco : p->lista;
    const int xl = X_PLACA + W_PLACA - 68;
    if (lista != NINGUNO)
    {
        const ui_param_t* d = &UI_PARAMS[lista];
        const int paso = fm_minf (14.0f, (float) (H_PLACA - 8) / (float) d->nOpciones), y0 = Y_PLACA + (H_PLACA - paso * d->nOpciones) / 2;
        cv_rect (xl - 4, Y_PLACA + 4, 1, H_PLACA - 8, C_REJA);
        for (int i = 0; i < d->nOpciones; ++i) led_lista (xl, y0 + i * paso, i == (int) (ui.par[lista] + 0.5f), d->opciones[i]);
    }
    else if (ui.panel == 0)
    {
        static const char* const N[5] = { "ARP", "DRIVE", "CHORUS", "DELAY", "REVERB" };
        const int id5[5] = { P_ARP_ON, P_FX_DIST_ON, P_FX_CHORUS_ON, P_FX_DELAY_ON, P_FX_REV_ON };
        static const char* const VOZ[3] = { "POLY", "MONO", "LEGATO" };
        cv_rect (xl - 4, Y_PLACA + 4, 1, H_PLACA - 8, C_REJA);
        for (int i = 0; i < 5; ++i) led_lista (xl, Y_PLACA + 6 + i * 15, ui.par[id5[i]] > 0.5f, N[i]);
        cv_text (xl, Y_PLACA + 84, &FONT_XS, VOZ[(int) (ui.par[P_VOZ_MODO] + 0.5f) % 3], C_LAVANDA);
    }
    if (id != NINGUNO)
    {
        cv_text (X_PLACA + 5, Y_PLACA + 6, &FONT_B, UI_PARAMS[id].nombre, C_LAVANDA);
        texto_param (id, t);
        a_derecha (&FONT_M, t, X_PLACA + ancho - 5, Y_PLACA + 2, C_SEG);
    }
    dibujar_grafica (p->grafica, X_PLACA + 4, Y_PLACA + 28, ancho - 8, H_PLACA - 32);
}

// ---------- Módulo de abajo ----------
static void disco (int cx, int cy, int r, uint16_t k)
{
    for (int y = -r; y <= r; ++y)
    {
        int w = 0;
        while ((w + 1) * (w + 1) + y * y <= r * r) ++w;
        cv_rect (cx - w, cy + y, 2 * w + 1, 1, k);
    }
}

static void dibujar_perilla (int x, const char* nombre, const char* valor, float n, float cero, float m, int esBoton, int prendido, int enfoque)
{
    const int cx = x + 28, cy = Y_CELDA + 37;
    centrado (&FONT_B, valor, cx, Y_CELDA, C_MARINO);
    if (esBoton)
    {
        cv_rect (cx - 5, cy - 16, 10, 10, C_BLACK);
        cv_rect (cx - 4, cy - 15, 8, 8, prendido ? C_VERDE : C_LEDOFF);
        cv_rect (cx - 16, cy - 2, 32, 16, C_NEGRO);
        cv_rect (cx - 15, cy - 1, 30, 14, C_BOTON);
    }
    else
    {
        const float a0 = fm_minf (n, cero), a1 = fm_maxf (n, cero), m0 = fm_minf (n, n + m), m1 = fm_maxf (n, n + m);
        for (int i = 0; i <= 10; ++i)
            cv_line (cx + (int) (senoArco[i * 6] * 16.0f), cy - (int) (cosenoArco[i * 6] * 16.0f), cx + (int) (senoArco[i * 6] * 19.0f), cy - (int) (cosenoArco[i * 6] * 19.0f), C_MARINO);
        for (int i = 0; i <= 60; ++i)
        {
            const float u = (float) i / 60.0f;
            const int px = cx + (int) (senoArco[i] * 13.0f) - 1, py = cy - (int) (cosenoArco[i] * 13.0f) - 1;
            if (m != 0.0f && u >= m0 && u <= m1) cv_rect (px, py, 3, 3, C_ORO);
            else if (u >= a0 && u <= a1) cv_rect (px, py, 3, 3, C_ARCO);
        }
        disco (cx, cy, 10, C_NEGRO);
        const int k = (int) (n * 60.0f + 0.5f);
        for (int r = 3; r <= 10; ++r) cv_rect (cx + (int) (senoArco[k] * (float) r), cy - (int) (cosenoArco[k] * (float) r), 2, 2, C_WHITE);
    }
    const int yp = Y_CELDA + 59;
    cv_rect (x + 1, yp, 55, 17, enfoque ? C_MARINO : m != 0.0f ? C_ORO : C_PASTILLA);
    centrado (text_w (&FONT_B, nombre) <= 54 ? &FONT_B : text_w (&FONT_S, nombre) <= 54 ? &FONT_S : &FONT_XS, nombre, cx, yp + 1, C_WHITE);
}

static void dibujar_celda (int x, int id)
{
    const def_param_t* td = &TABLA_PARAMS[id];
    char t[24];
    texto_param (id, t);
    const float cero = td->min < 0.0f && td->max > 0.0f ? fm_clampf (pocket_a_normal (id, 0.0f), 0.0f, 1.0f) : 0.0f;
    dibujar_perilla (x, UI_PARAMS[id].nombre, t, normal_de (id), cero, ui.mor[id], UI_PARAMS[id].tipo == UI_BOOL, ui.par[id] > 0.5f, foco_vigente() && ui.foco == id);
}

static void dibujar_celdas_seq (void)
{
    char t[16];
    const paso_t* s = &ajustes.pasos[ui.cursor];
    const int f = plat_ms() < ui.focoHasta ? ui.focoSeq : 0;
    poner_entero (t, ui.cursor + 1);
    dibujar_perilla (X_PLACA, "Step", t, (float) ui.cursor / 15.0f, 0.0f, 0.0f, 0, 0, f == 1);
    if (s->on) texto_nota (s->nota, t); else poner (t, "Rest");
    dibujar_perilla (X_PLACA + 57, "Note", t, (float) (s->nota - 24) / 72.0f, 0.0f, 0.0f, 0, 0, f == 2);
    poner (poner_entero (t, s->gate), "%");
    dibujar_perilla (X_PLACA + 114, "Gate", t, (float) s->gate * 0.01f, 0.0f, 0.0f, 0, 0, f == 3);
    // Con reloj MIDI de afuera la perilla muestra el tempo que llega ("Clock") y la de Tempo se guarda para cuando se vaya.
    // Con reloj, el nombre de la celda es el compás y el tiempo contados desde el Start ("17.3"): si se atrasa contra el contador de
    // la MPC, se están perdiendo pulsos en el camino.
    const int externo = relojExterno;
    const int bpm = (int) (tempo_actual() + 0.5f);   // mientras no hay medida del reloj de afuera, el tempo propio
    char c[16];
    const uint32_t tiempos = (pulsosSeq ? pulsosSeq - 1u : 0u) / 24u;
    poner_entero (poner (poner_entero (c, (int) (tiempos / 4u + 1u)), "."), (int) (tiempos % 4u + 1u));
    poner_entero (t, bpm);
    dibujar_perilla (X_PLACA + 171, externo ? c : "Tempo", t, (float) (bpm - 60) / 140.0f, 0.0f, 0.0f, 0, 0, f == 4);
}

static void dibujar_modulo (void)
{
    const vista_t* p = &PANELES[ui.panel];
    cv_rect (X_PLACA, Y_MODULO, W_PLACA, 96, C_LAVANDA);
    cv_rect (X_PLACA, Y_MODULO, W_PLACA, 16, C_BANDA);
    cv_rect (X_PLACA, Y_MODULO + 15, W_PLACA, 1, C_BANDA2);
    cv_text (X_PLACA + 5, Y_MODULO, &FONT_B, p->titulo, C_WHITE);
    int primero = 0, cuantos = 1;
    for (int f = 0; f < N_FAMILIAS; ++f)
        if (ui.panel >= FAMILIAS[f].primero && ui.panel < FAMILIAS[f].primero + FAMILIAS[f].cuantos) { primero = FAMILIAS[f].primero; cuantos = FAMILIAS[f].cuantos; }
    if (cuantos > 1)
        for (int i = 0; i < cuantos; ++i)
            cv_rect (X_PLACA + W_PLACA - 5 - (cuantos - i) * 7, Y_MODULO + 6, 5, 5, primero + i == ui.panel ? C_WHITE : C_BANDA2);
    if (p->led != NINGUNO)
    {
        const int x = X_PLACA + W_PLACA - 5 - cuantos * 7 - 12;
        cv_rect (x, Y_MODULO + 4, 9, 9, C_BLACK);
        cv_rect (x + 1, Y_MODULO + 5, 7, 7, ui.par[p->led] > 0.5f ? C_VERDE : C_LEDOFF);
    }
    if (ui.panel == PANEL_SEQ) dibujar_celdas_seq();
    else for (int i = 0; i < 4; ++i) if (p->k[i] != NINGUNO) dibujar_celda (X_PLACA + i * 57, p->k[i]);
}

// Dibuja una zona por franjas: cada función pinta en coordenadas de pantalla y el lienzo recorta.
static void franjas (int y0, int h, void (*fn) (void), uint16_t fondo)
{
    const int alto = (int) (APP_CANVAS_PX / 240u);
    for (int y = y0; y < y0 + h; y += alto)
    {
        const int hh = y + alto > y0 + h ? y0 + h - y : alto;
        cv_begin (240, (uint32_t) hh, fondo);
        cv_oy = -y;
        fn();
        cv_oy = 0;
        cv_blit (0, (uint32_t) y);
    }
}

// ---------- Portada ----------
// Al prender: el chasis rojo con la pantalla de 7 segmentos (la versión), NERD POCKET grande y un osciloscopio con una sierra
// que se va abriendo como un filtro. app_splash la dibuja quieta mientras arranca el aparato; en los primeros segundos de
// app_frame se anima (display y onda) y cualquier botón la salta.
#define PORTADA_MS 2400u

static void digito_grande (int i, int32_t x, int32_t y, int prendido_todo, int punto)
{
    const int32_t a = 22, h = 34, t = 5, m = (h - t) / 2;
    const int32_t s[7][4] = { { x + t, y, a - 2 * t, t }, { x + a - t, y + t, t, m - t }, { x + a - t, y + m + t, t, m - t }, { x + t, y + h - t, a - 2 * t, t },
                              { x, y + m + t, t, m - t }, { x, y + t, t, m - t }, { x + t, y + m, a - 2 * t, t } };
    const char* on = i >= 0 ? SEGMENTOS[i] : "";
    for (int k = 0; k < 7; ++k)
    {
        int prendido = prendido_todo;
        for (const char* c = on; *c; ++c) if (*c == 'a' + k) prendido = 1;
        cv_rect (s[k][0], s[k][1], s[k][2], s[k][3], prendido ? C_SEG : C_SEGOFF);
    }
    cv_rect (x + a + 3, y + h - t, t, t, punto ? C_SEG : C_SEGOFF);
}

static void dibujar_portada (void)
{
    const float f = ui.introFase;
    // Display de 7 segmentos con la versión ("0.3"); al principio prende todos los segmentos, como un aparato que se prueba.
    cv_rect (60, 12, 120, 50, C_NEGRO);
    cv_rect (63, 15, 114, 44, C_DISPLAY);
    int d[3] = { -1, -1, -1 }, nd = 0, punto = -1;
    for (const char* v = APP_VERSION; *v && nd < 3; ++v)
    {
        if (*v >= '0' && *v <= '9') d[nd++] = *v - '0';
        else if (*v == '.' && nd > 0 && punto < 0) punto = nd - 1;
    }
    if (nd == 0) { d[0] = d[1] = d[2] = 8; nd = 3; }
    const int prueba = f > 0.0f && f < 0.25f, x0 = 120 - (nd * 32) / 2 + 2;
    for (int i = 0; i < nd; ++i) digito_grande (prueba ? 8 : d[i], x0 + i * 32, 20, 0, prueba || i == punto);

    centrado (&FONT_L, "NERD", 120, 70, C_WHITE);
    centrado (&FONT_L, "POCKET", 120, 100, C_LAVANDA);

    // Osciloscopio: una sierra que se abre de 1 a 14 armónicos.
    const int ox = 14, oy = 142, ow = 212, oh = 52;
    cv_rect (ox - 3, oy - 3, ow + 6, oh + 6, C_NEGRO);
    cv_rect (ox, oy, ow, oh, C_DISPLAY);
    for (int g = 1; g < 4; ++g) cv_rect (ox + g * ow / 4, oy + 2, 1, oh - 4, C_REJA);
    cv_rect (ox + 2, oy + oh / 2, ow - 4, 1, C_REJA);
    const int armonicos = 1 + (int) (fm_minf (1.0f, f * 1.6f) * 13.0f);
    int py = 0;
    for (int x = 0; x < ow - 6; ++x)
    {
        const float ph = (float) x / (float) (ow - 6) * 3.0f + f * 2.0f;
        float v = 0.0f;
        for (int k = 1; k <= armonicos; ++k) v += fm_sinf (6.2831853f * (float) k * ph) / (float) k;
        const int y = oy + oh / 2 - (int) (v * 0.55f * (float) (oh / 2 - 5));
        if (x > 0) cv_line (ox + 2 + x, py, ox + 3 + x, y, C_SEG);
        py = y;
    }

    centrado (&FONT_XS, "the Nerd Lead synth on the FM-1", 120, 208, C_LAVANDA);
    for (int i = 0; i < 4; ++i)
    {
        const int encendido = f >= 0.6f + 0.08f * (float) i || f == 0.0f;
        cv_rect (96 + i * 14, 226, 8, 8, C_NEGRO);
        cv_rect (97 + i * 14, 227, 6, 6, encendido ? C_VERDE : C_LEDOFF);
    }
}

void app_splash (void)
{
    ui.introFase = 0.0f;
    lcd_fill (0, 0, 240, 240, C_ROJO);
    franjas (0, 240, dibujar_portada, C_ROJO);
}

static int portada_animar (uint32_t ahora)
{
    if (! ui.intro) return 0;
    const uint32_t t = ahora - ui.introDesde;
    if (t >= PORTADA_MS || ui.botones)
    {
        ui.intro = 0;
        lcd_fill (0, 0, 240, 240, C_ROJO);
        ui.sucioCab = ui.sucioPlaca = ui.sucioMod = 1;
        return 0;
    }
    if (ahora >= ui.proximoIntro)
    {
        const int primera = ui.proximoIntro == 0u;
        ui.proximoIntro = ahora + 33u;
        ui.introFase = (float) t / (float) PORTADA_MS;
        if (primera) { franjas (0, 240, dibujar_portada, C_ROJO); return 1; }   // (el simulador no llama a app_splash)
        franjas (12, 50, dibujar_portada, C_ROJO);
        franjas (139, 58, dibujar_portada, C_ROJO);
        franjas (224, 12, dibujar_portada, C_ROJO);
    }
    return 1;
}

// ---------- Controles ----------
static void marcar_todo (void) { ui.sucioCab = ui.sucioPlaca = ui.sucioMod = 1; }

static void recargar_parametros (void)
{
    for (int i = 0; i < P_NUM; ++i) { ui.par[i] = pocket_leer_param (i); ui.mor[i] = pocket_leer_morph (i); }
    marcar_todo();
}

static void ir_a (int p) { ui.panel = (p + N_PANELES) % N_PANELES; ui.foco = NINGUNO; marcar_todo(); }

static void girar_seq (int k, int32_t pasos)
{
    paso_t* p = &ajustes.pasos[ui.cursor];
    const int signo = pasos > 0 ? 1 : -1;
    if (k == 0) ui.cursor = (ui.cursor + signo + N_PASOS) % N_PASOS;
    else if (k == 1)
    {
        const int n = (int) p->nota + pasos;
        p->nota = (uint8_t) (n < 24 ? 24 : n > 96 ? 96 : n);
        p->on = 1;
    }
    else if (k == 2)
    {
        const int g = (int) p->gate + 5 * pasos;
        p->gate = (uint8_t) (g < 5 ? 5 : g > 100 ? 100 : g);
    }
    else
    {
        const int b = (int) ajustes.bpm + pasos;
        ajustes.bpm = (uint8_t) (b < 60 ? 60 : b > 200 ? 200 : b);
        pedir (CMD_BPM, 0, (float) ajustes.bpm);
    }
    ui.focoSeq = (uint8_t) (k + 1);
    ui.focoHasta = plat_ms() + 2000u;
    ui.sucio = 1;
    ui.sucioPlaca = ui.sucioMod = 1;
}

static void girar_perilla (int k, int32_t pasos)
{
    if (ui.panel == PANEL_SEQ) { girar_seq (k, pasos); return; }
    const int id = PANELES[ui.panel].k[k];
    if (id == NINGUNO) return;
    const def_param_t* d = &TABLA_PARAMS[id];
    ui.foco = (int16_t) id;
    ui.focoHasta = plat_ms() + 2000u;
    ui.sucioPlaca = ui.sucioMod = 1;
    if (! ui.editado) ui.sucioCab = 1;
    ui.editado = 1;
    ui.sucio = 1;
    if ((ui.botones >> B_SEL & 1u) && pocket_tiene_morph (id))
    {
        ui.mor[id] = fm_clampf (ui.mor[id] + (float) pasos / 64.0f, -1.0f, 1.0f);
        pedir (CMD_MORPH, (uint8_t) id, ui.mor[id]);
        return;
    }
    if (UI_PARAMS[id].tipo == UI_NUM)
        ui.par[id] = pocket_de_normal (id, pocket_a_normal (id, ui.par[id]) + (float) pasos / 127.0f);
    else
        ui.par[id] = fm_clampf ((float) ((int) (ui.par[id] + 0.5f) + pasos), d->min, d->max);
    pedir (CMD_PARAM, (uint8_t) id, ui.par[id]);
}

static void guardar (void)
{
    int fallo = ajustes_guardar() != 0;
    if (ui.editado)
    {
        static sonido_t s;
        s.magia = MAGIA_SONIDO;
        s.programa = ajustes.programa;
        for (int i = 0; i < P_NUM; ++i) s.par[i] = ui.par[i];
        for (int i = 0; i < NUM_MORPH; ++i) s.morph[i] = ui.mor[TABLA_MORPH[i]];
        fallo |= sonido_escribir (ajustes.programa, &s) != 0;
    }
    if (fallo) { app_say ("SAVE", "FAILED"); return; }
    ui.sucio = 0;
    ui.editado = 0;
    app_say ("SAVED", "");
}

// SEL + SAVE: el preset vuelve a como viene de fábrica.
static void restaurar_fabrica (void)
{
    if (sonido_guardado (ajustes.programa) && sonido_escribir (ajustes.programa, 0)) { app_say ("SAVE", "FAILED"); return; }
    pedir (CMD_PROGRAMA, ajustes.programa, 0.0f);
    ui.editado = 0;
    ui.recargar = 1;
    ui.foco = NINGUNO;
    app_say ("FACTORY", "");
}

static void entrada (void)
{
    touch_poll();
    const uint32_t teclas = touch_keys(), botones = plat_buttons();
    for (uint32_t k = 0; k < NKEYS; ++k)
    {
        const uint32_t bit = 1u << k;
        if ((teclas & bit) && ! (ui.teclas & bit))
        {
            // Se guarda la nota de cada tecla: si la octava cambia con la tecla abajo, el note off sigue siendo el suyo.
            const int nota = 53 + (int) k + 12 * ajustes.octava;
            ui.nota[k] = (uint8_t) nota;
            pedir (CMD_ON, ui.nota[k], (float) touch_vel (k) / 127.0f);
            if (ui.grabando)   // REC: la tecla escribe el paso y avanza
            {
                ajustes.pasos[ui.cursor].on = 1;
                ajustes.pasos[ui.cursor].nota = (uint8_t) (nota < 24 ? 24 : nota > 96 ? 96 : nota);
                ui.cursor = (ui.cursor + 1) % N_PASOS;
                ui.sucio = 1;
                ui.sucioPlaca = ui.sucioMod = 1;
            }
            plat_midi_send (0x09u | 0x90u << 8 | (uint32_t) nota << 16 | touch_vel (k) << 24, TO_USB);
        }
        else if (! (teclas & bit) && (ui.teclas & bit))
        {
            pedir (CMD_OFF, ui.nota[k], 0.0f);
            plat_midi_send (0x08u | 0x80u << 8 | (uint32_t) ui.nota[k] << 16, TO_USB);
        }
    }
    const uint32_t nuevos = botones & ~ui.botones;
    ui.botones = botones;
    ui.teclas = teclas;
    if ((nuevos >> B_OCTDN & 1u) && ajustes.octava > -2) { --ajustes.octava; ui.sucio = 1; }
    if ((nuevos >> B_OCTUP & 1u) && ajustes.octava < 2)  { ++ajustes.octava; ui.sucio = 1; }
    if (nuevos >> B_PLAY & 1u)
    {
        if (! seqTocando) seqPedirArranque = 1;
        seqTocando = ! seqTocando;
        ui.sucioPlaca = 1;
    }
    if (nuevos >> B_REC & 1u) { ui.grabando = ! ui.grabando; ui.sucioPlaca = 1; }
    if (ui.panel == PANEL_SEQ && (nuevos >> B_SEL & 1u))
    {
        ajustes.pasos[ui.cursor].on = ! ajustes.pasos[ui.cursor].on;
        ui.sucio = 1;
        ui.sucioPlaca = ui.sucioMod = 1;
    }
    if (nuevos >> B_SAVE & 1u)
    {
        if ((botones >> B_SEL & 1u) && ui.panel != PANEL_SEQ) restaurar_fabrica();
        else guardar();
    }
    for (int f = 0; f < N_FAMILIAS; ++f)
        if (nuevos >> FAMILIAS[f].boton & 1u)
        {
            const int enFamilia = ui.panel >= FAMILIAS[f].primero && ui.panel < FAMILIAS[f].primero + FAMILIAS[f].cuantos;
            ir_a (enFamilia ? FAMILIAS[f].primero + (ui.panel - FAMILIAS[f].primero + 1) % FAMILIAS[f].cuantos : FAMILIAS[f].primero);
        }
    const int32_t paginas = plat_enc (EN_SELECT);
    if (paginas) ir_a (ui.panel + (paginas > 0 ? 1 : -1));
    const int32_t programas = plat_enc (EN_PRESET);
    const uint32_t cambiosMidi = midiCambios;
    if (cambiosMidi != ui.cambiosMidi)
    {
        // Program Change por MIDI: el motor ya cambió; la interfaz lo sigue igual que con la perilla.
        ui.cambiosMidi = cambiosMidi;
        ajustes.programa = (uint8_t) midiPrograma;
        ui.sucio = 1;
        ui.editado = 0;
        ui.recargar = 1;
        ui.foco = NINGUNO;
        ui.sucioCab = 1;
    }
    if (programas)
    {
        const int total = pocket_num_programas();
        ajustes.programa = (uint8_t) (((int) ajustes.programa + programas % total + total) % total);
        pedir (CMD_PROGRAMA, ajustes.programa, 0.0f);
        ui.sucio = 1;
        ui.editado = 0;
        ui.recargar = 1;
        ui.foco = NINGUNO;
        ui.sucioCab = 1;
    }
    const int32_t rueda = plat_enc (EN_ALGO);
    if (rueda)
    {
        ui.rueda = fm_clampf (ui.rueda + (float) rueda / 127.0f, 0.0f, 1.0f);
        pedir (CMD_RUEDA, 0, ui.rueda);
        if (PANELES[ui.panel].grafica == G_MORPH) ui.sucioPlaca = 1;
    }
    for (int k = 0; k < 4; ++k)
    {
        const int32_t p = plat_enc (EN_K1 + k);
        if (p) girar_perilla (k, p);
    }
    uint32_t leds = botones | (ui.sucio ? 1u << B_SAVE : 0u) | (ui.par[P_ARP_ON] > 0.5f ? 1u << B_ARP : 0u) | (seqTocando ? 1u << B_PLAY : 0u) | (ui.grabando ? 1u << B_REC : 0u);
    for (int f = 0; f < N_FAMILIAS; ++f)
        if (ui.panel >= FAMILIAS[f].primero && ui.panel < FAMILIAS[f].primero + FAMILIAS[f].cuantos) leds |= 1u << FAMILIAS[f].boton;
    plat_glow (1);
    plat_leds (leds, teclas);
}

void app_input_only (void) { entrada(); }

void app_say (const char* a, const char* b)
{
    uint32_t i = 0;
    for (; a && *a && i < 23u; ++a) ui.aviso[i++] = *a;
    if (b && *b && i < 22u) ui.aviso[i++] = ' ';
    for (; b && *b && i < 23u; ++b) ui.aviso[i++] = *b;
    ui.aviso[i] = 0;
    ui.avisoHasta = plat_ms() + 1300u;
    ui.sucioCab = 1;
    if (ann_on()) ann_say (ui.aviso, 0, ANN_NOW);
}

void app_ui_init (void)
{
    touch_on (1);
    for (int i = 0; i <= 60; ++i)
    {
        const float a = (-135.0f + 4.5f * (float) i) * 0.0174532925f;
        senoArco[i] = fm_sinf (a);
        cosenoArco[i] = fm_cosf (a);
    }
    ui.intro = 1;                    // la portada sigue en pantalla unos segundos, animada (portada_animar)
    ui.introDesde = plat_ms();
    ui.proximoIntro = 0;
    ui.foco = NINGUNO;
    ui.recargar = 1;
    ui.cursor = 0;
    ui.grabando = 0;
    ui.editado = 0;
}

static void dibujar_lo_sucio (void)
{
    const vista_t* p = &PANELES[ui.panel];
    const uint32_t ahora = plat_ms();
    if (p->grafica == G_ONDA && ahora >= ui.proximoOnda)   // la onda en vivo, a 30 cuadros por segundo
    {
        ui.proximoOnda = ahora + 33u;
        ui.sucioPlaca = 1;
    }
    if (ui.sucioCab) { ui.sucioCab = 0; franjas (0, 25, dibujar_cabecera, C_ROJO); ++ui.cuadros; }
    if (ui.sucioPlaca)
    {
        ui.sucioPlaca = 0;
        const int ancho = ancho_grafica (p);
        calcular_grafica (p->grafica, Y_PLACA + 28, ancho - 8, H_PLACA - 32);
        franjas (Y_PLACA, H_PLACA, dibujar_placa, C_DISPLAY);
        ++ui.cuadros;
    }
    if (ui.sucioMod) { ui.sucioMod = 0; franjas (Y_MODULO, 96, dibujar_modulo, C_LAVANDA); ++ui.cuadros; }
}

void app_frame (void)
{
    const uint32_t ahora = plat_ms();
    entrada();
    if (ann_on() && ann_where()) decir_medidas();
    ann_tick (ahora);
    if (portada_animar (ahora)) return;
    if (ui.recargar && colaLee == colaEscribe) { ui.recargar = 0; recargar_parametros(); }
    if (ui.avisoHasta && ahora >= ui.avisoHasta) { ui.avisoHasta = 0; ui.sucioCab = 1; }
    if (ui.foco != NINGUNO && ahora >= ui.focoHasta) { ui.foco = NINGUNO; ui.sucioPlaca = ui.sucioMod = 1; }
    if (ui.focoSeq && ahora >= ui.focoHasta) { ui.focoSeq = 0; ui.sucioMod = 1; }
    if (ui.panel == PANEL_SEQ && seqPaso != ui.pasoVisto) { ui.pasoVisto = seqPaso; ui.sucioPlaca = 1; }
    {
        const uint32_t p = pulsosSeq ? pulsosSeq - 1u : 0u;
        const uint32_t estado = (uint32_t) (seqTocando && transporte) | (uint32_t) relojExterno << 1 | (uint32_t) (p % 24u < 6u) << 2 | (uint32_t) (p % 96u < 6u) << 3;
        if (estado != ui.estadoVisto) { ui.estadoVisto = estado; ui.sucioCab = 1; }
    }
    if (ui.panel == PANEL_SEQ && relojExterno && pulsosSeq / 24u != ui.tiempoVisto) { ui.tiempoVisto = pulsosSeq / 24u; ui.sucioMod = ui.sucioPlaca = 1; }
    if (ahora - ui.marcaFps >= 1000u) { ui.fps = ui.cuadros; ui.cuadros = 0; ui.marcaFps = ahora; }
    dibujar_lo_sucio();
}
