/* request.h */

#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Structures */

typedef struct {
    char  *host;    /* URL host */
    char  *port;    /* URL port */
    char  *path;    /* URL path */
} Request;

/* Functions */

Request *   request_create(const char *url);
void	    request_delete(Request *request);

ssize_t     request_stream(Request *request, FILE *stream);

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
