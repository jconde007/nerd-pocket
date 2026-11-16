// SPDX-License-Identifier: MPL-2.0
// Copyright (C) 2026 Jorge Conde
// La toolchain de JieLi no trae este encabezado; las funciones están en bsp/chip/libc.c.
#pragma once
void* memset (void* d, int c, unsigned n);
void* memcpy (void* d, const void* s, unsigned n);
