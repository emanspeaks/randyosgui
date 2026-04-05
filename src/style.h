#ifndef RANDY_STYLE_H
#define RANDY_STYLE_H

/*
 * style.h Ã¢â‚¬â€ Internal style declarations.
 *
 * RandyColor and RandyStyle are defined in the public header.
 * This file adds the global instance and the internal function prototypes.
 */

#include "../include/randyosgui.h"

/* =========================================================================
 * Global active style Ã¢â‚¬â€ defined in style.c
 * ========================================================================= */

extern RandyStyle g_style;

/* =========================================================================
 * Internal helpers are now static in style.c.
 * Public API (randy_style_*) is declared in randyosgui.h.
 * ========================================================================= */

#endif /* RANDY_STYLE_H */
