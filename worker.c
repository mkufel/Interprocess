/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * Group 21
 *
 * Maciej Kufel 0944597
 * Ahmed Ahres 0978238
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8. 
 * ”Extra” steps can lead to higher marks because we want students to take the initiative. 
 * Extra steps can be, for example, in the form of measurements added to your code, a formal 
 * analysis of deadlock freeness etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include "settings.h"
#include "common.h"
#include "md5s.h"


/* Used the internet source
 * Reference: https://stackoverflow.com/questions/4764608/generate-all-strings-under-length-n-in-c
 */
int inc(char *c) {
    if(c[0]==0) return 0;
    if(c[0]=='z'){
        c[0]='a';
        return inc(c+sizeof(char));
    }
    c[0]++;
    return 1;
}

/* Since string generation is not a focus of this assignment, we used the internet source
 * Reference: https://stackoverflow.com/questions/4764608/generate-all-strings-under-length-n-in-c
 */
char * generateStrings(uint128_t expectedHash, char startingChar)
{
    int n = MAX_MESSAGE_LENGTH - 1; // -1 because the startingChar is not included
    int i, j;
    char *c = malloc((n+1)*sizeof(char));
    for (i = 0; i <= n; i++) {
        for (j = 0; j < i; j++) c[j] = 'a';
        c[i] = 0;
        do {
            char *string = malloc(sizeof(c)+1);

            string[0] = startingChar; // First character is startingChar
            for (int k = 1; k < sizeof(string); ++k) {
                string[k] = c[k-1];
            }
            uint128_t result = md5s(string, sizeof(string)); // Hash the generated string
            if (result == expectedHash) { // Check if there is a match
                return string;
            }

            free(string);
        } while (inc(c));
    }
    free(c);
    return NULL;
}


/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;

    if (first_call == true)
    {
        srandom (time(NULL) % getpid());
        first_call = false;
    }
    usleep (random () % t);
}


int main (int argc, char * argv[])
{

    mqd_t               mq_fd_request; // Request queue
    mqd_t               mq_fd_response; // Response queue
    MQ_REQUEST_MESSAGE  req; // Request message
    MQ_RESPONSE_MESSAGE rsp; // Response message

    mq_fd_request = mq_open(argv[1], O_RDONLY); // Open the request queue
    mq_fd_response = mq_open(argv[2], O_WRONLY); // Open the response queue

    while(true)
    {
        mq_receive(mq_fd_request, (char *) &req, sizeof(req), NULL);

        if (req.md5Request == 0) // 0 is received when workers have to stop
        {
            break;
        }

        rsleep(10000);

        char *initialString; // String initially hashed into the md5 value of a request
        // Generate the strings starting with startingChar and compare the hash to md5Request
        initialString = generateStrings(req.md5Request, req.startingChar);

        // If no match is found, continue and go to next erequest
        if (initialString == NULL)
        {
            continue;
        }

        rsleep(10000);
        // Send the response
        for(int j = 0; j < sizeof(initialString); j++)
        {
            rsp.decodedString[j] = initialString[j];
        }

        rsp.md5Response = req.md5Request;
        mq_send(mq_fd_response, (char *) &rsp, sizeof(rsp), 0);
        sleep(3);
    }

    mq_close (mq_fd_response); // Close the response queue
    mq_close (mq_fd_request); // Close the request queue
    mq_unlink(argv[1]); // Remove the request queue
    mq_unlink(argv[2]); // Remove the response queue
    
    return (0);
}




