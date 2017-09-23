/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * STUDENT_NAME_1 (STUDENT_NR_1)
 * STUDENT_NAME_2 (STUDENT_NR_2)
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
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include <time.h>           // for time()
#include <complex.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq
#include "settings.h"
#include "common.h"
#include "md5s.h"
#include "common.h"


//static void rsleep (int t);

static void
getattr (mqd_t mq_fd)
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


int inc(char *c) {
    if(c[0]==0) return 0;
    if(c[0]=='z'){
        c[0]='a';
        return inc(c+sizeof(char));
    }
    c[0]++;
    return 1;
}

void generateStrings(char *array, uint128_t req, char startingPoint) {
    int n = 5;
    int i, j;
    char *c = malloc((n+1)*sizeof(char));
    for (i = 1; i <= n; i++) {
        for (j = 0; j < i; j++) c[j] = 'a';
        c[i] = 0;
        do {
            //printf("%s\n",c);
            uint128_t result = md5s(c, sizeof(c));
            //printf("hashed value: %llx\n", result);
            if (result == req) {
                printf("Generated string is %s ", c);
                *array = *c;

                printf("array with pointer %c ", *array);
                printf("array without pointer %s ", array);
                break;
            }
        } while (inc(c));
    }
    free(c);
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

    printf ("                                   child: receiving...\n");0
    mq_receive (mq_fd_request, (char *) &req, sizeof(req), NULL);
    rsleep(10000);
    printf ("                                   child: received: %llx \n",
            req.md5);

    printf("                                    and starting point %c \n",
           req.startingPoint);

    char *resultString;
    generateStrings(&resultString, req.md5, req.startingPoint);
    printf("found %c", resultString);
//    rsp.result = md5s(req.md5, 1);
//    sprintf (rsp.hashedValue, resultString);
    //printf(" Hashed value: %c",rsp.hashedValue);
    rsleep (10000);
    // send the response
    printf ("                                   child: sending...\n");
    mq_send (mq_fd_response, (char *) &rsp, sizeof (rsp), 0);

    mq_close (mq_fd_response);
    mq_close (mq_fd_request);
    mq_unlink(argv[1]);
    mq_unlink(argv[2]);


    // TODO:
    //// (see message_queue_test() in interprocess_basic.c)
    ////  * open the two message queues (whose names are provided in the arguments)
    //  * repeatingly: (just everything a "true" while)
    ////      - read from a message queue the new job to do
    ////     - wait a random amount of time (e.g. rsleep(10000);)
    //      - do that job (Generare string, hash them, compare to the result. If same, send a response)
    ////      - write the results to a message queue
    //    until there are no more tasks to do (just a break if -1)
    ////  * close the message queues
    
    return (0);
}




