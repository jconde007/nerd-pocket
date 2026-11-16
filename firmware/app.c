// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2026 Jorge Conde
// Nerd Pocket: el motor de motor/ sobre fm1-base. Las teclas y el MIDI tocan el motor, PRESETS cambia de programa,
// ALGORITHM hace de rueda de modulación y SAVE guarda el programa y la octava. La interfaz está en ui.c.
// Va en el unity build, después de la plataforma: gfx.c y announce.c están a la vista.
#include "fastmath.h"
#include "../motor/pocket.h"

#define MAGIA 0x4E504B32u                // "NPK2": otra magia carga los valores iniciales
#define N_PASOS 16
typedef struct { uint8_t on, nota, gate; } paso_t;   // gate en %
typedef struct { uint32_t magia; uint8_t programa; int8_t octava; uint8_t bpm; uint8_t rsv; paso_t pasos[N_PASOS]; } ajustes_t_app;
static ajustes_t_app ajustes;
#define AJUSTES_FLASH 0x96000u           // PLAT_XDATA0, que Felucca no usa. Un corte al guardar solo regresa a los valores iniciales

static int ajustes_guardar (void)
{
    if (plat_data_erase (AJUSTES_FLASH)) return -1;
    return plat_data_write (AJUSTES_FLASH, &ajustes, sizeof ajustes);
}

static void ajustes_cargar (void)
{
    const uint8_t* p = plat_data (AJUSTES_FLASH);
    for (uint32_t i = 0; p && i < sizeof ajustes; ++i) ((uint8_t*) &ajustes)[i] = p[i];
}

// ---------- Sonidos editados ----------
// Cada programa puede tener su versión guardada, en PLAT_XDATA1, que Felucca no usa arriba de 0xE8000. Los bytes 0xF00 a 0xF07
// de cada sector se quedan borrados, porque el cargador podría tomarlos por un registro de actualización.
// Programas 0-107: como en la 0.1 (valores en float), nueve por sector en 0xF0000-0xFBFFF; los que ya estaban guardados no se
// mueven. Del 108 en adelante (b10-b99) ya no caben así: van en 16 bits (posición de perilla y Morph), 17 por sector en 0xE8000.
#define SONIDOS_FLASH 0xF0000u
#define SONIDOS_POR_SECTOR 9u
#define SONIDOS_LARGOS 108u
#define CORTOS_FLASH 0xE8000u
#define CORTOS_POR_SECTOR 17u
#define SONIDO_USADO 0xF00u
#define MAGIA_SONIDO 0x4E504B53u         // "NPKS"
#define MAGIA_CORTO 0x4E504B32u          // "NPK2"
typedef struct { uint32_t magia, programa; float par[P_NUM], morph[NUM_MORPH]; } sonido_t;
typedef struct { uint32_t magia; uint16_t programa; uint16_t par[P_NUM]; int16_t morph[NUM_MORPH]; } sonido_corto_t;
_Static_assert (sizeof (sonido_t) * SONIDOS_POR_SECTOR <= SONIDO_USADO, "los sonidos no caben en el sector");
_Static_assert (sizeof (sonido_corto_t) * CORTOS_POR_SECTOR <= SONIDO_USADO, "los sonidos cortos no caben en el sector");
_Static_assert (SONIDOS_FLASH + (SONIDOS_LARGOS + SONIDOS_POR_SECTOR - 1u) / SONIDOS_POR_SECTOR * 0x1000u <= PLAT_XDATA1_HI, "los sonidos pasan de PLAT_XDATA1");
_Static_assert (CORTOS_FLASH + (NUM_PROGRAMAS - SONIDOS_LARGOS + CORTOS_POR_SECTOR - 1u) / CORTOS_POR_SECTOR * 0x1000u <= SONIDOS_FLASH,
                "los sonidos cortos se encimarían con los largos");

static int sonido_largo (uint32_t programa) { return programa < SONIDOS_LARGOS; }

static uint32_t sonido_sector (uint32_t programa)
{
    return sonido_largo (programa) ? SONIDOS_FLASH + programa / SONIDOS_POR_SECTOR * 0x1000u
                                   : CORTOS_FLASH + (programa - SONIDOS_LARGOS) / CORTOS_POR_SECTOR * 0x1000u;
}

static uint32_t sonido_lugar (uint32_t programa)
{
    return sonido_largo (programa) ? programa % SONIDOS_POR_SECTOR * (uint32_t) sizeof (sonido_t)
                                   : (programa - SONIDOS_LARGOS) % CORTOS_POR_SECTOR * (uint32_t) sizeof (sonido_corto_t);
}

static const void* sonido_registro (uint32_t programa)
{
    const uint8_t* p = plat_data (sonido_sector (programa));
    return p ? p + sonido_lugar (programa) : 0;
}

// 1 si este programa tiene un sonido guardado.
static int sonido_guardado (uint32_t programa)
{
    const void* r = sonido_registro (programa);
    if (! r || programa >= NUM_PROGRAMAS) return 0;
    if (sonido_largo (programa)) return ((const sonido_t*) r)->magia == MAGIA_SONIDO && ((const sonido_t*) r)->programa == programa;
    return ((const sonido_corto_t*) r)->magia == MAGIA_CORTO && ((const sonido_corto_t*) r)->programa == programa;
}

// Reescribe el sector con este sonido, o sin él si s es 0. El sonido se detiene un momento mientras se borra.
static int sonido_escribir (uint32_t programa, const sonido_t* s)
{
    static uint8_t sector[SONIDO_USADO];
    static sonido_corto_t c;
    const void* datos = s;
    uint32_t largo = (uint32_t) sizeof (sonido_t);
    if (! sonido_largo (programa))
    {
        largo = (uint32_t) sizeof (sonido_corto_t);
        if (s)
        {
            c.magia = MAGIA_CORTO; c.programa = (uint16_t) programa;
            for (int i = 0; i < P_NUM; ++i) c.par[i] = (uint16_t) (fm_clampf (pocket_a_normal (i, s->par[i]), 0.0f, 1.0f) * 65535.0f + 0.5f);
            for (int i = 0; i < NUM_MORPH; ++i)
            {
                const float m = fm_clampf (s->morph[i], -1.0f, 1.0f) * 32767.0f;
                c.morph[i] = (int16_t) (m + (m < 0.0f ? -0.5f : 0.5f));
            }
            datos = &c;
        }
    }
    const uint32_t base = sonido_sector (programa);
    const uint8_t* p = plat_data (base);
    if (! p || programa >= NUM_PROGRAMAS) return -1;
    for (uint32_t i = 0; i < SONIDO_USADO; ++i) sector[i] = p[i];
    uint8_t* d = sector + sonido_lugar (programa);
    for (uint32_t i = 0; i < largo; ++i) d[i] = datos ? ((const uint8_t*) datos)[i] : 0xFFu;
    if (plat_data_erase (base)) return -1;
    return plat_data_write (base, sector, SONIDO_USADO);
}

// Lo pone en el motor (desde el audio, como pocket_programa); lee la flash en su lugar.
static void sonido_aplicar (uint32_t programa)
{
    if (! sonido_guardado (programa)) return;
    const void* r = sonido_registro (programa);
    if (sonido_largo (programa))
    {
        const sonido_t* s = (const sonido_t*) r;
        for (int i = 0; i < P_NUM; ++i) pocket_param (i, s->par[i]);
        for (int i = 0; i < NUM_MORPH; ++i) pocket_poner_morph (TABLA_MORPH[i], s->morph[i]);
        return;
    }
    const sonido_corto_t* c = (const sonido_corto_t*) r;
    for (int i = 0; i < P_NUM; ++i) pocket_param (i, pocket_de_normal (i, (float) c->par[i] * (1.0f / 65535.0f)));
    for (int i = 0; i < NUM_MORPH; ++i) pocket_poner_morph (TABLA_MORPH[i], (float) c->morph[i] * (1.0f / 32767.0f));
}

// Programas del banco B que no caben en el FM-1 tal como vienen: con FM o sync, sus osciladores van a 1× en vez de a 2×
// (pocket_ligero). Medidos en el aparato el 2026-11-11 con 8 notas (docs/plan.md, paso 11): los polifónicos que se quedaban en
// 3 voces o menos, los que tenían 5 cortes o más con menos de 6 voces, los mono arriba de 85 % de CPU y los que con 4-5 voces
// seguían cortando (b18 daba clics al tocarlo), si el recorte les ahorra algo (b06 y b45 no tienen FM ni sync). En el 2X emulado
// estos 44 se alejan 17.64 → 17.82 y el banco 16.73 → 16.81 (b83 y b86 se alejaban mucho y se quitaron).
static const uint8_t LIGEROS[] = { 102, 108, 110, 111, 112, 113, 114, 115, 116, 117, 120, 124, 125, 127, 128, 131, 132, 134, 135, 137, 140, 145, 147, 149, 151, 152, 153, 155, 158, 159, 163, 167, 170, 173, 175, 176, 179, 182, 185, 191, 193, 194, 196, 197 };

static int programa_ligero (uint32_t programa)
{
    for (uint32_t i = 0; i < sizeof LIGEROS; ++i) if (LIGEROS[i] == programa) return 1;
    return 0;
}

// Carga el programa en el motor con su recorte y, si hay, su sonido guardado (desde el audio).
static void programa_cargar (uint32_t programa)
{
    pocket_programa ((int) programa);
    pocket_ligero (programa_ligero (programa));
    sonido_aplicar (programa);
}

// ---------- Cola de la interfaz al motor ----------
// El motor solo se toca desde la interrupción de audio. El bucle principal escribe y la interrupción lee.
enum { CMD_ON, CMD_OFF, CMD_PROGRAMA, CMD_RUEDA, CMD_PARAM, CMD_MORPH, CMD_BPM };
typedef struct { uint8_t tipo; uint8_t nota; float valor; } comando_t;
#define TAM_COLA 64u
static comando_t cola[TAM_COLA];
static volatile uint32_t colaEscribe, colaLee;

static void pedir (uint8_t tipo, uint8_t nota, float valor)
{
    const uint32_t e = colaEscribe;
    if (e - colaLee >= TAM_COLA) return;
    cola[e % TAM_COLA] = (comando_t) { tipo, nota, valor };
    colaEscribe = e + 1u;
}

static void atender_cola (void)
{
    while (colaLee != colaEscribe)
    {
        const comando_t c = cola[colaLee % TAM_COLA];
        if (c.tipo == CMD_ON) pocket_nota_on (c.nota, c.valor);
        else if (c.tipo == CMD_OFF) pocket_nota_off (c.nota);
        else if (c.tipo == CMD_PROGRAMA) programa_cargar (c.nota);
        else if (c.tipo == CMD_PARAM) pocket_param (c.nota, c.valor);
        else if (c.tipo == CMD_MORPH) pocket_poner_morph (c.nota, c.valor);
        else if (c.tipo == CMD_BPM) pocket_bpm (c.valor);
        else pocket_rueda (c.valor);
        colaLee = colaLee + 1u;
    }
}

// ---------- Audio ----------
static volatile uint32_t midiVisto, vocesVistas;
static volatile uint8_t callado;
static float gananciaActual;
static float ondaViva[APP_BLOCK];        // el último bloque, para la gráfica de HOME

// El secuenciador (más abajo) y el reloj de afuera comparten estos.
static volatile uint8_t seqTocando, seqPedirArranque;
static volatile int seqPaso;

// ---------- Reloj MIDI de afuera (USB o TRS): tempo, Start, Stop y Continue ----------
// Llegan al principio de cada bloque, así que cada pulso se fecha con la muestra en que empieza (hasta 5.8 ms tarde).
#define PULSOS_MEDIDA 24u                // el tempo se mide sobre una negra
#define SIN_RELOJ (SR_ENTERO / 2u)       // medio segundo sin pulsos: de vuelta al tempo propio
#define SR_ENTERO 44100u
static volatile uint8_t relojExterno, transporte = 1, seqPasoPendiente;
static volatile float bpmExterno;
static uint32_t muestrasTotales, ultimoPulso, pulsoEn[PULSOS_MEDIDA + 1u], nPulsos, pulsosSeq;
// Diagnóstico del reloj (panel SEQ): Start, Stop y Continue recibidos, el hueco más largo entre pulsos desde el último Start
// (en ms) y las veces que se dio el reloj por perdido.
static volatile uint32_t diagStart, diagStop, diagCont, diagHueco, diagPerdido;

static float tempo_actual (void) { return relojExterno && bpmExterno > 0.0f ? bpmExterno : (float) ajustes.bpm; }

static void reloj_pulso (void)
{
    if (! relojExterno) { relojExterno = 1; nPulsos = 0; pocket_reloj_externo (1); }
    else if (transporte)
    {
        const uint32_t ms = (muestrasTotales - ultimoPulso) * 1000u / SR_ENTERO;
        if (ms > diagHueco) diagHueco = ms;
    }
    ultimoPulso = muestrasTotales;
    for (uint32_t i = PULSOS_MEDIDA; i > 0u; --i) pulsoEn[i] = pulsoEn[i - 1u];
    pulsoEn[0] = muestrasTotales;
    if (nPulsos < PULSOS_MEDIDA) ++nPulsos;
    if (nPulsos >= 6u)
    {
        const uint32_t muestras = pulsoEn[0] - pulsoEn[nPulsos];
        if (muestras > 0u)
        {
            const float b = 60.0f * (float) SR_ENTERO * (float) nPulsos / (24.0f * (float) muestras);
            if (b >= 30.0f && b <= 300.0f)
            {
                bpmExterno = bpmExterno > 0.0f ? bpmExterno + (b - bpmExterno) * 0.25f : b;
                pocket_bpm (bpmExterno);
            }
        }
    }
    pocket_reloj_pulso();
    if (transporte && pulsosSeq++ % 6u == 0u) seqPasoPendiente = 1;   // semicorcheas: 6 pulsos; con Stop no cuenta, como la MPC
}

static void reloj_inicio (void)
{
    // Un Start trae reloj detrás: se pasa a externo ya, para que el secuenciador espere el primer pulso en vez de dar un paso
    // con el tempo propio (antes, si el reloj se había dado por perdido, el patrón arrancaba un paso adelantado).
    if (! relojExterno) { relojExterno = 1; nPulsos = 0; pocket_reloj_externo (1); }
    ultimoPulso = muestrasTotales;
    ++diagStart;
    diagHueco = 0;
    transporte = 1;
    pulsosSeq = 0;
    seqPasoPendiente = 0;
    seqPedirArranque = 1;
    pocket_reloj_inicio();
}

static void reloj_revisar (void)
{
    if (! relojExterno || muestrasTotales - ultimoPulso < SIN_RELOJ) return;
    relojExterno = 0;
    ++diagPerdido;
    bpmExterno = 0.0f;
    pocket_reloj_externo (0);
    pocket_bpm ((float) ajustes.bpm);
    if (! transporte) seqTocando = 0;    // se paró afuera: no arranca solo con el tempo propio
    transporte = 1;
}

// Program Change 0-98 dentro del banco que eligió el Bank Select (CC 0: 0 = A, 1 = B; sin Bank Select, A), como la Teensy. El
// motor cambia aquí mismo; la interfaz se entera por midiPrograma y midiCambios (cuántos van).
static uint8_t bancoMidi;
static volatile uint32_t midiPrograma, midiCambios;

static void midi_entrada (void)
{
    uint32_t pkt;
    int puerto;
    while (plat_midi_in (&pkt, &puerto))
    {
        const uint32_t st = (pkt >> 8) & 0xFFu, d1 = (pkt >> 16) & 0x7Fu, d2 = pkt >> 24;
        if (st == 0xF8u) { reloj_pulso(); continue; }
        if (st == 0xFAu) { reloj_inicio(); continue; }
        if (st == 0xFBu) { ++diagCont; transporte = 1; continue; }
        if (st == 0xFCu) { ++diagStop; transporte = 0; continue; }
        if (st == 0xFEu) continue;
        ++midiVisto;
        if ((st & 0xF0u) == 0x90u && d2) pocket_nota_on ((int) d1, (float) d2 / 127.0f);
        else if ((st & 0xF0u) == 0x80u || (st & 0xF0u) == 0x90u) pocket_nota_off ((int) d1);
        else if ((st & 0xF0u) == 0xB0u && d1 == 1u) pocket_rueda ((float) d2 / 127.0f);
        else if ((st & 0xF0u) == 0xB0u && d1 == 0u) bancoMidi = (uint8_t) d2;   // se aplica en el próximo Program Change
        else if ((st & 0xF0u) == 0xC0u && bancoMidi <= 1u && d1 < PROGRAMAS_POR_BANCO)
        {
            const uint32_t prog = bancoMidi * PROGRAMAS_POR_BANCO + d1;
            if (prog >= NUM_PROGRAMAS) continue;     // la versión pública no trae el banco B
            programa_cargar (prog);
            midiPrograma = prog;
            midiCambios = midiCambios + 1u;
        }
        else if ((st & 0xF0u) == 0xE0u) pocket_bend ((float) ((int) (d2 << 7 | d1) - 8192) / 8192.0f);
    }
}

// ---------- Secuenciador: corre en el audio y cae en la muestra exacta, partiendo el bloque en cada evento ----------
static uint32_t seqFalta, seqResto;      // muestras hasta el siguiente evento, y las que quedan del paso tras soltar la nota
static int seqNotaSonando = -1;

static void seq_soltar (void)
{
    if (seqNotaSonando >= 0) pocket_nota_off (seqNotaSonando);
    seqNotaSonando = -1;
}

#define ESPERA_PULSO 0x40000000u          // con reloj de afuera, el siguiente paso no llega por cuenta: lo trae un pulso

static void seq_evento (void)
{
    if (seqNotaSonando >= 0)
    {
        seq_soltar();
        seqFalta = relojExterno ? ESPERA_PULSO : seqResto;
        return;
    }
    seqPaso = (seqPaso + 1) % N_PASOS;
    const paso_t p = ajustes.pasos[seqPaso];
    const uint32_t dur = (uint32_t) (661500.0f / tempo_actual());   // 44100 · 60 / 4: semicorcheas
    if (! p.on) { seqFalta = relojExterno ? ESPERA_PULSO : dur; return; }
    uint32_t gate = dur * p.gate / 100u;
    if (gate < 1u) gate = 1u;
    pocket_nota_on (p.nota, 0.8f);
    seqNotaSonando = p.nota;
    seqFalta = gate;
    seqResto = dur - gate;
}

// Renderea n muestras, deteniéndose en cada evento del secuenciador.
static void render_con_seq (float* L, float* R, uint32_t n)
{
    if (seqPedirArranque) { seqPedirArranque = 0; seq_soltar(); seqPaso = -1; seqFalta = relojExterno ? ESPERA_PULSO : 0u; }
    if (! seqTocando || ! transporte) { seqPasoPendiente = 0; seq_soltar(); pocket_render (L, R, (int) n); return; }
    if (relojExterno && seqPasoPendiente) { seqPasoPendiente = 0; seq_soltar(); seqFalta = 0; }
    else if (! relojExterno && seqFalta >= ESPERA_PULSO) seqFalta = 0;   // se fue el reloj: sigue con el propio
    for (uint32_t hecho = 0; hecho < n; )
    {
        while (seqFalta == 0u) seq_evento();
        const uint32_t k = n - hecho < seqFalta ? n - hecho : seqFalta;
        pocket_render (L + hecho, R + hecho, (int) k);
        hecho += k;
        seqFalta -= k;
    }
}

void app_render (int32_t* lr, uint32_t n, uint32_t master_q12)
{
    static float L[APP_BLOCK], R[APP_BLOCK];
    const float meta = (float) master_q12 / 4096.0f * 8300000.0f;
    if (callado || n > APP_BLOCK)
    {
        for (uint32_t i = 0; i < 2u * n; ++i) lr[i] = 0;
        return;
    }
    atender_cola();
    midi_entrada();
    reloj_revisar();
    render_con_seq (L, R, n);
    muestrasTotales += n;
    vocesVistas = (uint32_t) pocket_voces_activas();
    for (uint32_t i = 0; i < n; ++i) ondaViva[i] = L[i];
    for (uint32_t i = 0; i < n; ++i)
    {
        gananciaActual += (meta - gananciaActual) * 0.004f;   // ~5 ms: girar MASTER no da chasquidos
        lr[2u * i]      = (int32_t) fm_clampf (L[i] * gananciaActual, -8388607.0f, 8388607.0f);
        lr[2u * i + 1u] = (int32_t) fm_clampf (R[i] * gananciaActual, -8388607.0f, 8388607.0f);
    }
}

void app_panic (void) { callado = 1; }

void app_overload (void) { pocket_aliviar(); }

void app_init (void)
{
    ajustes_cargar();
    if (ajustes.magia != MAGIA || ajustes.programa >= (uint8_t) pocket_num_programas() || ajustes.octava < -2 || ajustes.octava > 2
        || ajustes.bpm < 60u || ajustes.bpm > 200u)
    {
        static const uint8_t NOTAS[4] = { 48, 48, 55, 51 };
        ajustes = (ajustes_t_app) { .magia = MAGIA, .bpm = 120 };
        for (int i = 0; i < N_PASOS; ++i) ajustes.pasos[i] = (paso_t) { (uint8_t) (i % 4 == 0), i % 4 == 0 ? NOTAS[i / 4] : 48u, 50 };
    }
    pocket_iniciar();
    if (plat_cpu2_set (1) == 0) pocket_paralelo (plat_cpu2_run, plat_cpu2_wait);
    programa_cargar (ajustes.programa);
    pocket_bpm ((float) ajustes.bpm);
#ifdef PLAT_HOST
    pocket_fijar_fases (0.0f, 0.0f);    // en el simulador, repetible para compararlo con pruebas/render
#endif
}

// ---------- Medidas por USB ----------
static void numero (char* d, uint32_t v, uint32_t cifras)
{
    for (uint32_t i = cifras; i-- > 0u; v /= 10u) d[i] = (char) ('0' + v % 10u);
    d[cifras] = 0;
}
static uint32_t medidaFps (void);

#if NP_MEDIDAS
// Lo que lee tools/fm1_remote.py con "screen": herramientas/medir_fm1.py saca de aquí las medidas. Tiempos en microsegundos por bloque.
static void pegar (char** d, const char* s) { while (*s) *(*d)++ = *s++; }
static void pegar_numero (char** d, const char* nombre, uint32_t v)
{
    char t[8];
    pegar (d, nombre);
    numero (t, v, 6);
    pegar (d, t);
    pegar (d, " ");
}
static void decir_medidas (void)
{
    static const char* const PARTE[POCKET_PERFIL_PARTES] = { "LFO", "VOZ", "DST", "CHO", "DLY", "REV", "FIN" };
    char t[180], *d = t;
    pegar_numero (&d, "PRG", ajustes.programa + 1u);
    pegar_numero (&d, "CPU", plat_cpu_pct());
    pegar_numero (&d, "XRN", plat_xruns());
    pegar_numero (&d, "VOC", vocesVistas);
    pegar_numero (&d, "FPS", medidaFps());
    pegar_numero (&d, "NUC", plat_cpu2_on() ? 2u : 1u);
    pegar_numero (&d, "RIN", (uint32_t) plat_cpu2_gave_up());
    pegar_numero (&d, "PIL", plat_cpu2_stack());
    pegar_numero (&d, "ALV", pocket_alivios());
    for (uint32_t i = 0; i < POCKET_PERFIL_PARTES; ++i) pegar_numero (&d, PARTE[i], pocket_perfil[i] / PLAT_TICKS_PER_US);
    *d = 0;
    ann_say (t, 0, ANN_NOW);
}
#else
static void decir_medidas (void) { ann_say (pocket_nombre_programa (ajustes.programa), 0, ANN_NOW); }
#endif

#include "ui.c"

static uint32_t medidaFps (void) { return ui.fps; }

#ifdef PLAT_HOST
int app_expect (const char* que, int* valor)
{
    if (! strcmp (que, "programa")) *valor = ajustes.programa;
    else if (! strcmp (que, "octava")) *valor = ajustes.octava;
    else if (! strcmp (que, "voces")) *valor = (int) vocesVistas;
    else if (! strcmp (que, "midi")) *valor = (int) midiVisto;
    else if (! strcmp (que, "sucio")) *valor = ui.sucio;
    else if (! strcmp (que, "panel")) *valor = ui.panel;
    else if (! strcmp (que, "editado")) *valor = ui.editado;
    else if (! strcmp (que, "corte")) *valor = (int) ui.par[P_FILTRO_CORTE];
    else if (! strcmp (que, "motor_corte")) *valor = (int) pocket_leer_param (P_FILTRO_CORTE);
    else if (! strcmp (que, "guardado")) *valor = sonido_guardado (ajustes.programa);
    else if (! strcmp (que, "tocando")) *valor = seqTocando;
    else if (! strcmp (que, "cursor")) *valor = ui.cursor;
    else if (! strcmp (que, "grabando")) *valor = ui.grabando;
    else if (! strcmp (que, "paso_on")) *valor = ajustes.pasos[ui.cursor].on;
    else if (! strcmp (que, "paso_nota")) *valor = ajustes.pasos[ui.cursor].nota;
    else if (! strcmp (que, "bpm")) *valor = ajustes.bpm;
    else if (! strcmp (que, "externo")) *valor = relojExterno;
    else if (! strcmp (que, "bpm_externo")) *valor = (int) (bpmExterno + 0.5f);
    else if (! strcmp (que, "transporte")) *valor = transporte;
    else if (! strcmp (que, "paso")) *valor = seqPaso;
    else if (! strcmp (que, "hueco")) *valor = (int) diagHueco;
    else if (! strcmp (que, "starts")) *valor = (int) diagStart;
    else if (! strcmp (que, "estado")) *valor = (int) pocket_tam_estado();
    else return 0;
    return 1;
}
#endif
