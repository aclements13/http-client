/* hammer.c: Make HTTP request and measure bandwidth */

#include "hammer.h"
#include "macros.h"
#include "request.h"
#include "timestamp.h"

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Constants */

#define MEGABYTE    (double)(1<<20)

/* Functions */

/**
 * Perform a hammer through by making a HTTP request to the specified URL and
 * printing the contents of the response to the given stream.
 * Function prints the bandwidth of the HTTP transaction if it is successful to
 * stderr.
 *
 * @param   url         Make HTTP request to this URL.
 * @param   stream      Write the contents of each response to this stream.
 *
 * @return  Whether or not the HTTP request was successful.
 **/
bool	hammer(const char *url, FILE *stream) {
    /* TODO: Perform HTTP request */
    Request * r = request_create(url);
    
    if(!r){
        request_delete(r);
        return false;
    }
    
    double start_time = timestamp();
    ssize_t bytes_written = request_stream(r, stream);
    double end_time = timestamp();

    if(bytes_written == -1){
        request_delete(r);
        return false;
    }
    
    /* TODO: Compute and display bandwidth to stderr if HTTP request was
     * successful */

    double bandwidth = bytes_written / 1048576.0 / (end_time - start_time);
    fprintf(stderr, "Bandwidth: %0.2lf MB/s\n", bandwidth);
 
    request_delete(r);
    return true;
}

/**
 * Make hammers HTTP requests to the specified url, while writing the contents
 * of the request to the specified stream.
 *
 * Each HTTP request must be performed by a separate child process.
 *
 * Function prints the total elapsed time for all throws to stderr at the end.
 *
 * @param   url         Make HTTP requests to this URL.
 * @param   hammers     The number of HTTP requests to make.
 * @param   stream      Write the contents of each response to this stream.
 *
 * @return  Whether or not all hammer throws were successful.
 **/
bool	throw(const char *url, size_t hammers, FILE *stream) {
    /* TODO: Throw all hammers by spawning child processes that call hammer and
     * exit success if the hammer throw was successful. */
    double start_time = timestamp();
    
    for (size_t i = 0; i < hammers; i++){
        
        pid_t pid = fork();

        if (pid < 0){           //Parent - Failure
            return false;
        }
        else if (pid == 0){     //Child
            bool result = hammer(url, stream);
            exit(!result); 
        }

    }
    
    /* TODO: Collect results of throws by waiting for all children */  
    int sum = 0; 
    int status = EXIT_SUCCESS;

    for (size_t i = 0; i < hammers; i++){
        while(wait(&status) < 0);
        sum += status;
    }

    /* TODO: Print elapsed time to stderr */
    double end_time = timestamp();
    fprintf(stderr, "Elapsed Time: %0.2lf seconds\n", end_time - start_time);

    if (sum == 0){
        return true;
    }

    return false;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
