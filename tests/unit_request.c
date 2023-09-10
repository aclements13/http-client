/* unit_request.c */

#include "request.h"
#include "macros.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Structures */

typedef struct {
    char   *url;
    Request request;
} RequestTestCase;

/* Constants */

RequestTestCase TEST_CASES[] = {
    {"test.tld"                       , {.host = "test.tld", .port = "80", .path = ""}},
    {"test.tld:25"                    , {.host = "test.tld", .port = "25", .path = ""}},
    {"test.tld/"                      , {.host = "test.tld", .port = "80", .path = ""}},
    {"test.tld/data"                  , {.host = "test.tld", .port = "80", .path = "data"}},
    {"test.tld/path/to/data"          , {.host = "test.tld", .port = "80", .path = "path/to/data"}},
    {"test.tld:25/data"               , {.host = "test.tld", .port = "25", .path = "data"}},
    {"test.tld:25/path/to/data"       , {.host = "test.tld", .port = "25", .path = "path/to/data"}},
    {"http://test.tld"                , {.host = "test.tld", .port = "80", .path = ""}},
    {"http://test.tld:25"             , {.host = "test.tld", .port = "25", .path = ""}},
    {"http://test.tld/"               , {.host = "test.tld", .port = "80", .path = ""}},
    {"http://test.tld/data"           , {.host = "test.tld", .port = "80", .path = "data"}},
    {"http://test.tld/path/to/data"   , {.host = "test.tld", .port = "80", .path = "path/to/data"}},
    {"http://test.tld:25/data"        , {.host = "test.tld", .port = "25", .path = "data"}},
    {"http://test.tld:25/path/to/data", {.host = "test.tld", .port = "25", .path = "path/to/data"}},
    {NULL}
};

const char *GOOD_URLS[] = {
    "example.com",
    "httpforever.com:80",
    "http://h4x0r.space:9898",
    "h4x0r.space:9898/walden.txt",
    "http://h4x0r.space:9898/gatsby.txt",
    NULL
};

const char *BAD_URLS[] = {
    "google.com",
    "localhost:1000",
    "http://h4x0r.space/404",
};

/* Test Cases */

int test_00_request_create() {
    for (RequestTestCase *tc = TEST_CASES; tc->url; tc++) {
        Request *request= request_create(tc->url);

        assert(request);
        assert(streq(request->host, tc->request.host));
        assert(streq(request->port, tc->request.port));
        assert(streq(request->path, tc->request.path));

        free(request->host);
        free(request->port);
        free(request->path);
        free(request);
    }

    return EXIT_SUCCESS;
}

int test_01_request_delete() {
    for (RequestTestCase *tc = TEST_CASES; tc->url; tc++) {
        Request *request= request_create(tc->url);

        assert(request);
        assert(streq(request->host, tc->request.host));
        assert(streq(request->port, tc->request.port));
        assert(streq(request->path, tc->request.path));

        request_delete(request);
    }
    return EXIT_SUCCESS;
}

int test_02_request_stream_success() {
    for (const char **url = GOOD_URLS; *url; url++) {
        Request *request = request_create(*url);
        assert(request);

        char temp_path[] = "test.XXXXXX";
        int  temp_fd = mkstemp(temp_path);
        if (temp_fd < 0) {
            fprintf(stderr, "mkstemp: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        FILE* temp_file = fdopen(temp_fd, "w");
        if (!temp_file) {
            fprintf(stderr, "fdopen: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        ssize_t bytes_written = request_stream(request, temp_file);
        fclose(temp_file);

        if (bytes_written < 0) {
            unlink(temp_path);
            assert(bytes_written >= 0);
        }

        char command[BUFSIZ];
        sprintf(command, "curl -s %s | diff - %s", *url, temp_path);
        int status = WEXITSTATUS(system(command));
        unlink(temp_path);
        assert(status == 0);

        request_delete(request);
    }

    return EXIT_SUCCESS;
}

int test_03_request_stream_failure() {
    for (const char **url = BAD_URLS; *url; url++) {
        Request *request = request_create(*url);
        assert(request);

        assert(request_stream(request, stdout) < 0);

        request_delete(request);
    }

    return EXIT_SUCCESS;
}

/* Main Execution */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s NUMBER\n\n", argv[0]);
        fprintf(stderr, "Where NUMBER is right of the following:\n");
        fprintf(stderr, "    0  Test request_create\n");
        fprintf(stderr, "    1  Test request_delete\n");
        fprintf(stderr, "    2  Test request_stream_success\n");
        fprintf(stderr, "    3  Test request_stream_failure\n");
        return EXIT_FAILURE;
    }

    int number = atoi(argv[1]);
    int status = EXIT_FAILURE;

    switch (number) {
        case 0:  status = test_00_request_create(); break;
        case 1:  status = test_01_request_delete(); break;
        case 2:  status = test_02_request_stream_success(); break;
        case 3:  status = test_03_request_stream_failure(); break;
        default: fprintf(stderr, "Unknown NUMBER: %d\n", number); break;
    }

    return status;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
