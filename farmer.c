/*
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * Maciek Kufel 0944597
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
#include <sys/wait.h>
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq
#include <stdbool.h>
#include "settings.h"
#include "common.h"
#include "md5s.h"

static long get_curr_msgs(mqd_t *q){
    struct mq_attr  attr;

    int res = mq_getattr(*q, &attr);
    if (res < 0) {
        exit(1);
    }

    return (attr.mq_curmsgs);
}


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


int create_workers(int number_of_workers)
{
    pid_t pid;

    for (int i = 0; i < number_of_workers; i++ )
    {
        pid = fork();

        if (pid < 0){
            return -1;
        }

        if (pid == 0) { //If child process, execute the worker job
            execlp("./worker", "worker", mq_name1, mq_name2, NULL);
        }
    }
    return pid; //if negative something went wrong
}


int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }

    mqd_t               mq_fd_request;
    mqd_t               mq_fd_response;
    MQ_REQUEST_MESSAGE  req;
    MQ_RESPONSE_MESSAGE rsp;
    struct mq_attr      attr;

    mq_fd_request = mq_open (mq_name1, O_RDONLY);
    mq_fd_response = mq_open (mq_name2, O_WRONLY);

    sprintf (mq_name1, "/mq_request_%s_%d", "Maciek Kufel 0944597", getpid());
    sprintf (mq_name2, "/mq_response_%s_%d", "Ahmed Ahres 0978238", getpid());

    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MQ_REQUEST_MESSAGE);
    mq_fd_request = mq_open (mq_name1, O_WRONLY | O_CREAT | O_EXCL | O_NONBLOCK , 0600, &attr);

    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MQ_RESPONSE_MESSAGE);
    mq_fd_response = mq_open (mq_name2, O_RDONLY | O_CREAT | O_EXCL | O_NONBLOCK, 0600, &attr);

//    getattr(mq_fd_request);
//    getattr(mq_fd_response);

    pid_t processID = create_workers(NROF_WORKERS);

    if (processID < 0) {
        perror("fork() failed");
        exit (1);
    } else {

        sleep(3);
        int listcounter = 0;
        while (listcounter < MD5_LIST_NROF) {
            req.startingChar = 'a' - 1;
            for (int j = 0; j < ALPHABET_NROF_CHAR; ++j) {
                req.startingChar++;
                req.md5Request = md5_list[listcounter];

                printf("parent: sending %llx, ", req.md5Request);
                printf("starting character: %c\n", req.startingChar);
                if (get_curr_msgs(&mq_fd_request) != 10) {
                    mq_send(mq_fd_request, (char *) &req, sizeof(req), 0);
                } else {
                    j--;
                    req.startingChar--;
                    sleep(8);
                    continue;
                }
//                sleep(3);
            }
            listcounter++;
        }

//        sleep(30);


        for (int i = 0; i < MD5_LIST_NROF; i++)
        {
            //read the result and store it in the response message
//            getattr(mq_fd_response);
//              getattr(mq_fd_response);
                int nr_msgs = get_curr_msgs(&mq_fd_response);
            //printf("Ready to receive, queue length: %d\n", nr_msgs);
            if (nr_msgs == MD5_LIST_NROF) {
                for (int j = 0; j < MD5_LIST_NROF; j++)
                {
                    mq_receive(mq_fd_response, (char *) &rsp, sizeof(rsp), NULL);
//                    sleep(3);
                    printf("%s\n", rsp.decodedString);
                }
                break;
            } else {
                sleep(5);
                i--;
            }

        }

//        sleep(5);

        req.md5Request = 0; //command workers to stop

        for (int i = 0; i < NROF_WORKERS; i++) {
            mq_send(mq_fd_request, (char *) &req, sizeof(req), 0);
        }
            waitpid(processID, NULL, 0);   // wait for the child


        mq_close(mq_fd_response);
        mq_close(mq_fd_request);
        mq_unlink(mq_name1);
        mq_unlink(mq_name2);
    }


    // TODO:
    ////  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming
    ////  * wait until the chilren have been stopped (see process_test())
    ////  * clean up the message queues (see message_queue_test())
    // Add a loop in the farmer to send jobs until all sent
    // Make sure passed messages make sense.
    // Add a method for breaking children

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)

    return (0);
}

