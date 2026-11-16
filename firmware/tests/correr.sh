#!/bin/sh
# SPDX-License-Identifier: MPL-2.0
# Copyright (C) 2026 Jorge Conde
# Compila Nerd Pocket para el simulador de fm1-base y corre sus comprobaciones. Necesita un Python con Pillow (raqm incluido)
# para las fuentes de fm1-base: PYTHON, o build/py en la raíz del repositorio.
set -e
cd "$(dirname "$0")/../.."
PY="${PYTHON:-build/py/bin/python}"
"$PY" fm1-base/tools/build.py firmware --host
BASICO=firmware/tests/basico.txt
# Versión pública (motor/tablas.h con 99 programas, generar_tablas.py --solo-a): la vuelta completa es de 99, se quitan las
# pruebas del banco B y se comprueba que el Bank Select 1 no cambie de programa.
if grep -q "^#define NUM_PROGRAMAS 99$" motor/tablas.h; then
    BASICO=firmware/build/host/basico_a.txt
    sed -e 's/^turn PRESET 198$/turn PRESET 99/' -e '/^# Banco B:/,$d' firmware/tests/basico.txt | cat - firmware/tests/solo_a.txt > "$BASICO"
fi
firmware/build/host/nerdpocket_host "$BASICO" firmware/build/host
firmware/build/host/nerdpocket_host firmware/tests/reloj.txt firmware/build/host
