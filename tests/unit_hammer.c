/* unit_hammer.c */

#include "hammer.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Constants */

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

int test_00_hammer_success() {
    for (const char **url = GOOD_URLS; *url; url++) {
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

        bool result = hammer(*url, temp_file);
        fclose(temp_file);

        if (!result) {
            unlink(temp_path);
            assert(result);
        }

        char command[BUFSIZ];
        sprintf(command, "curl -s %s | diff - %s", *url, temp_path);
        int status = WEXITSTATUS(system(command));
        unlink(temp_path);
        assert(status == 0);
    }

    return EXIT_SUCCESS;
}

int test_01_hammer_failure() {
    for (const char **url = BAD_URLS; *url; url++) {
        assert(!hammer(*url, stdout));
    }
    
    return EXIT_SUCCESS;
}

int test_02_throw_success() {
    for (const char **url = GOOD_URLS; *url; url++) {
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

        bool result = throw(*url, 1, temp_file);
        fclose(temp_file);

        if (!result) {
            unlink(temp_path);
            assert(result);
        }

        char command[BUFSIZ];
        sprintf(command, "curl -s %s | diff - %s", *url, temp_path);
        int status = WEXITSTATUS(system(command));
        unlink(temp_path);
        assert(status == 0);
    }

    return EXIT_SUCCESS;
}

int test_03_throw_failure() {
    for (const char **url = BAD_URLS; *url; url++) {
        assert(!throw(*url, 1, stdout));
    }

    return EXIT_SUCCESS;
}

/* Main Execution */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s NUMBER\n\n", argv[0]);
        fprintf(stderr, "Where NUMBER is right of the following:\n");
        fprintf(stderr, "    0  Test hammer_success\n");
        fprintf(stderr, "    1  Test hammer_failure\n");
        fprintf(stderr, "    2  Test throw_success\n");
        fprintf(stderr, "    3  Test throw_failure\n");
        return EXIT_FAILURE;
    }   

    int number = atoi(argv[1]);
    int status = EXIT_FAILURE;

    switch (number) {
        case 0:  status = test_00_hammer_success(); break;
        case 1:  status = test_01_hammer_failure(); break;
        case 2:  status = test_02_throw_success(); break;
        case 3:  status = test_03_throw_failure(); break;
        default: fprintf(stderr, "Unknown NUMBER: %d\n", number); break;
    }
    
    return status;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
