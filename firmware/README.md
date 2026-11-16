# Firmware
La app de Nerd Pocket sobre `fm1-base`, fijado como submódulo en la raíz del repositorio. `app.c` conecta `motor/` con
las teclas, el MIDI y MASTER; `ui.c` dibuja la interfaz aprobada en `ui/prototipo.html`. `ui_datos.h` lo genera `herramientas/generar_datos_firmware.py`.

## Probar en el simulador
No necesita aparato, Docker ni toolchain. Las fuentes de `fm1-base` piden Pillow con raqm, que la primera vez se instala en una carpeta local:

```bash
git submodule update --init --recursive
```

```bash
/opt/homebrew/bin/python3.12 -m venv build/py && build/py/bin/pip install pillow
```

```bash
sh firmware/tests/correr.sh
```

Deja en `firmware/build/host/` las capturas y las tomas de audio. `pruebas/comparar_simulador.py` compara una toma con la misma nota de `build/render`.

## Compilar para el FM-1
Necesita Docker Desktop abierto, porque la toolchain de JieLi solo corre en Linux x86-64, y la toolchain y el SDK en `~/.jieli`, que se instalan con
`fm1-base/tools/get_toolchain.sh` y `fm1-base/bsp/tools/get_sdk.sh`. El SDK ya no está en la dirección de GitHub de ese script: se pasa `SDK_URL=https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git`, y el script revisa sus archivos contra los hashes probados.

```bash
JIELI_TOOLCHAIN=$HOME/.jieli/toolchain AC79_SDK=$HOME/.jieli/ac79-sdk build/py/bin/python fm1-base/tools/build.py firmware
```

## Publicar
`sh herramientas/empaquetar_release.sh 0.1` compila la versión y deja en `build/publicar/` el paquete con sus licencias y su LEEME.

Falta antes de subirlo:
- Publicar el código fuente: MPL-2.0 pide decir dónde está, y este repositorio no tiene remoto. Esa dirección va en `LEEME.txt` y en la línea `url` de `words.tsv`, que hoy es un marcador.
- `id_prefix` de `fm1.toml` se eligió sin consultar a nadie: cada firmware debe tener el suyo.

## Licencia
Los archivos propios son MPL-2.0 (`LICENSE`). `fm1-base` y `felucca-bsp` también, cada archivo con su encabezado. El paquete lleva además tres archivos del SDK de JieLi (Apache-2.0) y la fuente Barlow (SIL OFL 1.1).
