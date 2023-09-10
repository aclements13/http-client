/* hammer.h */

#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Functions */

bool	hammer(const char *url, FILE *stream);
bool    throw(const char *url, size_t hammers, FILE *stream);

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
