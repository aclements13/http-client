/* request.c: Make HTTP Requests */

#include "request.h"

#include "macros.h"
#include "socket.h"

#include <stdlib.h>
#include <string.h>

/* Constants */

#define HOST_DELIMITER  "://"
#define PATH_DELIMITER  '/'
#define PORT_DELIMITER  ':'

/* Functions */

/**
 * Construct Request structure by parsing URL string into separate host, port,
 * and path components.
 *
 * @param   url         URL string to parse.
 *
 * @return  Request structure.
 **/
Request *   request_create(const char *url) {
    /* TODO: Copy data to local buffer */
    
    //const char *test_url = "google.com:9999/data";
    char buf [BUFSIZ];
    strcpy(buf,url); 
    
    //debug("url: %s\n", test_url);

    /* TODO: Skip scheme to host */

    char *host1 = strstr(buf, HOST_DELIMITER);
    
    if (host1){
         host1 += strlen(HOST_DELIMITER);
    }
    else{
        
        //case when there is no http://
        host1 = buf;
    }

   
    /* TODO: Split host:port from path */
    char *path1 = strchr(host1, PATH_DELIMITER);

    if(!path1){
        path1 = "";
    }
    else{
        *path1 = 0;
        path1++;
    }

    
    /* TODO: Split host and port */
    char *port1 = strchr(host1, PORT_DELIMITER);
    if(!port1){
        port1 = "80";
    }
    else{
        *port1 = 0;
        port1++;
    }
    
    /*
    debug("host: %s", host);
    debug("port: %s", port);
    debug("path: %s", path);
    */
 
    /* TODO: Allocate Request structure */
    Request *r = malloc(sizeof(Request));

    /* TODO: Copy components to URL structure */

    r->host = strdup(host1);
    r->port = strdup(port1);
    r->path = strdup(path1);
    

    return r;
}

/**
 * Deallocate Request structure.
 *
 * @param   request     Request structure to deallocate.
 **/
void	    request_delete(Request *request) {

    free(request->host);
    free(request->port);
    free(request->path);
    
    free(request);
}

/**
 * Make a HTTP request and write the contents of the response to the specified
 * stream.
 *
 * @param   url         Make a HTTP request to this URL.
 * @param   stream      Write the contents of response to this stream.
 *
 * @return  -1 on error, otherwise the number of bytes written.
 **/
ssize_t     request_stream(Request *request, FILE *stream) {
    /* TODO: Connect to remote host and port */
    FILE *client_stream = socket_dial(request->host, request->port);
    if(!client_stream){
        return -1;
    }
    

    /* TODO: Send request to server */
    fprintf(client_stream, "GET /%s HTTP/1.0\r\n", request->path);
    fprintf(client_stream, "Host: %s\r\n", request->host);
    fprintf(client_stream, "\r\n");


    /* TODO: Read response status from server */
    char buffer[BUFSIZ];
    
    if(!fgets(buffer, BUFSIZ, client_stream)){
         fclose(client_stream);
        return -1;

    }
           
    //debug("%s\n", buffer);
    
    if (!strstr(buffer, "200 OK")){
        fclose(client_stream);
        return -1;
    }



    /* TODO: Read response headers from server */
    size_t  content_length = 0;
    
    while (fgets(buffer, BUFSIZ, client_stream) && !streq(buffer, "\r\n")) {
        sscanf(buffer, "Content-Length: %lu", &content_length);
    }

    //debug("content length is: %lu bytes\n", content_length);

    /* TODO: Read response body from server */
    
    size_t count = 0;
    size_t read = 0;
    while((read = fread(buffer, 1,BUFSIZ, client_stream)) > 0){
        count += fwrite(buffer, 1,read, stream);
        
    }

    //debug("count: %lu", count);
    /* TODO: Close connection */

    fclose(client_stream);

    /* TODO: Return number of bytes written and check if it matches Content-Length */
    
    if (content_length && content_length!=count){
        return -1;
    }
    else{
        return count;
    }
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
