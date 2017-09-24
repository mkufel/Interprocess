/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
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
#include <string.h>
#include "settings.h"
#include "common.h"
#include "md5s.h"


static void
getattr (mqd_t mq_fd)  //not sure if we have to use it
{
    struct mq_attr      attr;
    int                 rtnval;

    rtnval = mq_getattr (mq_fd, &attr);
    if (rtnval == -1)
    {
        perror ("mq_getattr() failed");
        exit (1);
    }
    fprintf (stderr, "%d: mqdes=%d max=%ld size=%ld nrof=%ld\n",
             getpid(),
             mq_fd, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
}

//Copied from the internet, either write a new one or add a reference
int inc(char *c) {
    if(c[0]==0) return 0;
    if(c[0]=='z'){
        c[0]='a';
        return inc(c+sizeof(char));
    }
    c[0]++;
    return 1;
}



char * generateStrings(uint128_t expectedHash, char startingChar)
{
    int n = MAX_MESSAGE_LENGTH-1;
    int i, j;
    char *c = malloc((n+1)*sizeof(char));


    for (i = 0; i <= n; i++) {
        for (j = 0; j < i; j++) c[j] = 'a';
        c[i] = 0;
        do {
            char *string = malloc(sizeof(c)+1);

            string[0] = startingChar;
            for (int k = 1; k < sizeof(string); ++k) {
                string[k] = c[k-1];
            }
            uint128_t result = md5s(string, sizeof(string));
            if (result == expectedHash) {
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

    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;

    mq_fd_request = mq_open(argv[1], O_RDONLY);
    mq_fd_response = mq_open(argv[2], O_WRONLY);



    while(true)
    {
        ssize_t receivedResult;
//        printf("                                   child: receiving...\n");
        receivedResult = mq_receive(mq_fd_request, (char *) &req, sizeof(req), NULL);

        if (req.md5Request == 0)
        {
            break;
        }

        rsleep(10000);
//        printf("                                   child: received: %llx ", req.md5Request);
//        printf("with a startingChar %c\n", req.startingChar);

        char *initialString; // string initially hashed into the md5 value of a request
        initialString = generateStrings(req.md5Request, req.startingChar);

        if (initialString == NULL)
        {
            continue;
        }

//        printf("                                   String decoded: %s\n", initialString);

        rsleep(10000);
        // send the response
        for(int j = 0; j < sizeof(initialString); j++)
        {
            rsp.decodedString[j] = initialString[j];
        }
        rsp.md5Response = req.md5Request;
//        printf("                                   child: sending...\n");
        mq_send(mq_fd_response, (char *) &rsp, sizeof(rsp), 0);
        sleep(3);
    }

    mq_close (mq_fd_response);
    mq_close (mq_fd_request);
    mq_unlink(argv[1]);
    mq_unlink(argv[2]);


    // TODO:
    //// (see message_queue_test() in interprocess_basic.c)
    ////  * open the two message queues (whose names are provided in the arguments)
    ////  * repeatingly: (just everything a "true" while)
    ////      - read from a message queue the new job to do
    ////     - wait a random amount of time (e.g. rsleep(10000);)
    ////      - do that job (Generare string, hash them, compare to the result. If same, send a response)
    ////      - write the results to a message queue
    ////    until there are no more tasks to do (just a break if -1)
    ////  * close the message queues
    
    return (0);
}




