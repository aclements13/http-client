#!/usr/bin/env python3

import concurrent.futures
import os
import sys
import time

import requests

# Functions

def usage(status=0):
    progname = os.path.basename(sys.argv[0])
    print(f'''Usage: {progname} [-n HAMMERS] URL
    -n HAMMERS  How many times to hammer the URL
    ''')
    sys.exit(status)

def hammer(url):
    ''' Download and display the contents of the given URL. 

    Prints the bandwidth (MB/s) of the HTTP transaction.
    '''
    # Make HTTP Request
    start_time   = time.time()
    response     = requests.get(url, allow_redirects=False)
    result       = response.status_code == 200

    if result:
        # Compute elapsed time and bandwidth
        stop_time    = time.time()
        elapsed_time = stop_time - start_time
        bandwidth    = len(response.content) / (1<<20) / elapsed_time

        # Display HTTP body
        sys.stdout.write(response.content.decode())

        # Display Bandwidth (bytes / seconds)
        sys.stderr.write(f'Bandwidth: {bandwidth:0.2f} MB/s\n')

    # Return if HTTP Request was successful
    return result

def throw(url, hammers):
    start_time   = time.time()
    urls         = [url]*hammers

    # Throw all hammers concurrently using multiple processes
    with concurrent.futures.ProcessPoolExecutor(hammers) as executor:
        result   = all(executor.map(hammer, urls))

    stop_time    = time.time()
    elapsed_time = stop_time - start_time

    # Display elapsed time
    sys.stderr.write(f'Elapsed Time: {elapsed_time:0.2f} seconds\n')

    # Return if all HTTP Requests were successful
    return result

# Main Execution

def main():
    hammers   = 1
    arguments = sys.argv[1:]

    # Parse command line arguments
    while arguments and arguments[0].startswith('-'):
        argument = arguments.pop(0)
        if argument == '-n':
            hammers = int(arguments.pop(0))
        elif argument == '-h':
            usage(0)
        else:
            usage(1)
    
    if not arguments:
        usage(1)

    url = arguments.pop(0)
    if not url.startswith('http://'):
        url = 'http://' + url

    # Throw hammers at URL
    result = throw(url, hammers)
    sys.exit(not result)

if __name__ == '__main__':
    main()
