#!/bin/sh
# SPDX-License-Identifier: MPL-2.0
# Copyright (C) 2026 Jorge Conde
# Compila la versión X.Y y deja en build/publicar/ el paquete con sus licencias. Uso: empaquetar_release.sh 0.1
set -e
V="${1:?falta la versión}"
cd "$(dirname "$0")/.."
PY="${PYTHON:-build/py/bin/python}"
JIELI_TOOLCHAIN="${JIELI_TOOLCHAIN:-$HOME/.jieli/toolchain}" AC79_SDK="${AC79_SDK:-$HOME/.jieli/ac79-sdk}" "$PY" fm1-base/tools/build.py firmware --release "$V"
D="build/publicar/nerd-pocket-$V"
rm -rf "$D"
mkdir -p "$D/LICENSES"
cp "firmware/build/nerdpocket-$V.fwsc" "$D/"
cp LICENSE "$D/LICENSE-MPL-2.0.txt"
cp fm1-base/LICENSES/Apache-2.0.txt "$D/LICENSES/"
cp fm1-base/assets/fonts/Barlow-OFL.txt "$D/LICENSES/"
cp herramientas/LEEME-release.txt "$D/LEEME.txt"
ls -la "$D"
