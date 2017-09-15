/*
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * Maciek Kufel
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
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq

#include "settings.h"
#include "common.h"

//static void
//getattr (mqd_t mq_fd)
//{
//    struct mq_attr      attr;
//    int                 rtnval;
//
//    rtnval = mq_getattr (mq_fd, &attr);
//    if (rtnval == -1)
//    {
//        perror ("mq_getattr() failed");
//        exit (1);
//    }
//    fprintf (stderr, "%d: mqdes=%d max=%ld size=%ld nrof=%ld\n",
//             getpid(),
//             mq_fd, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
//}
static void
message_queue_child (void)
{
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;
//    struct mq_attr      attr;


    mq_fd_request = mq_open (mq_name1, O_RDONLY);
    mq_fd_response = mq_open (mq_name2, O_WRONLY);

//    attr.mq_maxmsg  = 10;
//    attr.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
//    mq_fd_request = mq_open (mq_name1, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);
//
//    attr.mq_maxmsg  = 10;
//    attr.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);
//    mq_fd_response = mq_open (mq_name2, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);



    // read the message queue and store it in the request message
    printf ("                                   child: receiving...\n");
    mq_receive (mq_fd_request, (char *) &req, sizeof (req), NULL);
    sleep (5);

    printf ("                                   child: received: %c, %c\n",
            req.md5[0], req.startingPoint);

    // fill response message
    // e   contains the length of f[]
    // f[] contains characters abcd
    // g[] contains characters ABCDEFGHIJ + \0-terminator (so it is a C-string)
    rsp.md5[0] = 'z';
    rsp.hashedValue[0] = 'd';

    sleep (3);
    // send the response
    printf ("                                   child: sending...\n");
    mq_send (mq_fd_response, (char *) &rsp, sizeof (rsp), 0);

    mq_close (mq_fd_response);
    mq_close (mq_fd_request);
}


static void
process_test (void)
{
    pid_t processID;      /* Process ID from fork() */
    printf ("Parent id: %d\n", getpid());

    processID = fork();
    if (processID < 0)
    {
        perror("fork() failed");
        exit (1);
    }
    else
    {
        if (processID == 0)
        {
            printf ("Child  pid:%d\n", getpid());
            // or try this one:
            //execlp ("./interprocess_basics", "my_own_name_for_argv0", "first_argument", NULL);
            message_queue_child();
        }
        // else: we are still the parent (which continues this program)

        waitpid (processID, NULL, 0);   // wait for the child
        printf ("child %d has been finished\n\n", processID);
    }
}

static void
message_queue_test (void)
{
    pid_t               processID;      /* Process ID from fork() */
    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;
    struct mq_attr      attr;

    mq_fd_request = mq_open (mq_name1, O_RDONLY);
    mq_fd_response = mq_open (mq_name2, O_WRONLY);

    sprintf (mq_name1, "/mq_request_%s_%d", "Maciek Kufel", getpid());
    sprintf (mq_name2, "/mq_response_%s_%d", "Ahmed Ahres", getpid());

    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = sizeof (MQ_REQUEST_MESSAGE);
    mq_fd_request = mq_open (mq_name1, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

    attr.mq_maxmsg  = 10;
    attr.mq_msgsize = sizeof (MQ_RESPONSE_MESSAGE);
    mq_fd_response = mq_open (mq_name2, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);


    for (int i = 0; i < NROF_WORKERS; ++i) {
        process_test();
    }
    processID = getpid();
    printf("pid cosidered in the if: %d\n", processID);
    if (processID < 0)
    {
        perror("fork() failed");
        exit (1);
    }
    else
    {
        if (processID == 0)
        {
            // child-stuff
            printf("Process id = 0, call the message_queue_child");
            message_queue_child ();
            exit (0);
        }
        else
        {
            printf("Process id != 0, do the parent stuff");
            // remaining of the parent stuff
            // fill request message
            req.md5[0] = 'a';
            req.startingPoint = 'b';

            sleep (3);
            // send the request
            printf ("parent: sending...\n");
            mq_send (mq_fd_request, (char *) &req, sizeof (req), 0);

            sleep (3);
            // read the result and store it in the response message
            printf ("parent: receiving...\n");
            mq_receive (mq_fd_response, (char *) &rsp, sizeof (rsp), NULL);

            sleep (3);
            printf ("parent: received: %c, %c \n", rsp.hashedValue[0], rsp.md5[0]);

            sleep (1);

            waitpid (processID, NULL, 0);   // wait for the child

            mq_close (mq_fd_response);
            mq_close (mq_fd_request);
            mq_unlink (mq_name1);
            mq_unlink (mq_name2);
        }
    }
}

int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }

    message_queue_test();

    // TODO:
    //  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming
    //  * wait until the chilren have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)

    return (0);
}