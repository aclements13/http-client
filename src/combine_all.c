/* thor.c */

#include "hammer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <macros.h>

/* Functions */

void usage(const char *progname, int status) {
    fprintf(stderr, "Usage: %s [options] URL\n", progname);
    fprintf(stderr, "    -n HAMMERS How many times to hammer the URL\n");
    exit(status);
}

/* Main Execution */

int main(int argc, char *argv[]) {
    /* TODO: Parse command line arguments */
    
    int num_hammers = 1;
    char *program_name = argv[0];
    char *url;
    
    if (argc == 1){
        usage(program_name, 1);
    } 


    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-'){
            if (argv[i][1] == 'h'){
                usage(program_name, 0);
            }
            else if (argv[i][1] == 'n'){
                num_hammers = atoi(argv[i + 1]);
                i++;
            }
            else{
                usage(program_name, 1);
            }
        }
        else{
            url = argv[i];
        }
    }
    
    //printf("URL: %s\n", url);
    //printf("num hammers: %d\n", num_hammers);

    /* TODO: Throw Hammers */
    bool result = throw(url, num_hammers, stdout);
    
    return !result; 
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
