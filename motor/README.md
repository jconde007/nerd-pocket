# Synth engine

The synth engine (`pocket.c`, `tablas.c`) is **not open source** and is not in this repository: it is the engine of
Nerd Lead, a synth plugin, and stays closed. `pocket.h` and `tablas.h` are its interface, as the app uses it. The
release packages and the browser emulator include it compiled.

Without it the firmware does not build. The app (`firmware/`) is MPL-2.0, like fm1-base.
