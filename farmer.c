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
#include "settings.h"
#include "common.h"

/*
 * Function to get the number of messages in a queue
 */
static long get_curr_msgs(mqd_t *q){
    struct mq_attr  attr;

    int res = mq_getattr(*q, &attr);
    if (res < 0) {
        exit(1);
    }

    return (attr.mq_curmsgs);
}

/*
 * Function to create child processes and execute worker
 */
int create_workers(int number_of_workers)
{
    pid_t pid;

    for (int i = 0; i < number_of_workers; i++ )
    {
        pid = fork(); //fork() creates the process

        if (pid < 0){ //If negative something went wrong
            return -1;
        }

        if (pid == 0) { //If child process, execute the worker job
            execlp("./worker", "worker", mq_name1, mq_name2, NULL);
        }
    }
    return pid;
}


int main (int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }

    mqd_t               mq_fd_request; //Request queue
    mqd_t               mq_fd_response; //Response queue
    MQ_REQUEST_MESSAGE  req; //Request message
    MQ_RESPONSE_MESSAGE rsp; //Response message
    struct mq_attr      attr;

    mq_fd_request = mq_open (mq_name1, O_RDONLY);
    mq_fd_response = mq_open (mq_name2, O_WRONLY);

    sprintf (mq_name1, "/mq_request_%s_%d", "Maciek Kufel 0944597", getpid()); //Change name of queue
    sprintf (mq_name2, "/mq_response_%s_%d", "Ahmed Ahres 0978238", getpid());

    attr.mq_maxmsg = MQ_MAX_MESSAGES; //Set max messages to MQ_MAX_MESSAGES
    attr.mq_msgsize = sizeof(MQ_REQUEST_MESSAGE); //Set message size to the size of req
    mq_fd_request = mq_open (mq_name1, O_WRONLY | O_CREAT | O_EXCL | O_NONBLOCK , 0600, &attr);

    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MQ_RESPONSE_MESSAGE);
    mq_fd_response = mq_open (mq_name2, O_RDONLY | O_CREAT | O_EXCL | O_NONBLOCK, 0600, &attr);

    pid_t processID = create_workers(NROF_WORKERS); //Create NROF_WORKERS children

    if (processID < 0) {
        perror("fork() failed");
        exit (1);
    } else {

        int listcounter = 0;
        while (listcounter < MD5_LIST_NROF) {  //iterate through the input list of md5 hashes

            req.startingChar = 'a' - 1;

            // For every character in the alphabet send a new job for a current hash
            for (int j = 0; j < ALPHABET_NROF_CHAR; ++j) {
                req.startingChar++;
                req.md5Request = md5_list[listcounter];

                if (get_curr_msgs(&mq_fd_request) < 10) { //if request queue not full, send a message
                    mq_send(mq_fd_request, (char *) &req, sizeof(req), 0);
                } else { //If request queue full, wait and repeat the same iteration
                    j--;
                    req.startingChar--;
                    sleep(8);
                    continue;
                }

            }
            listcounter++;
        }


        for (int i = 0; i < MD5_LIST_NROF; i++)
        {
            //get the current number of messages in the response queue
            int nr_msgs = get_curr_msgs(&mq_fd_response);
            if (nr_msgs == MD5_LIST_NROF) { //if responses for all sent jobs are present, start receiving
                for (int j = 0; j < MD5_LIST_NROF; j++)
                {
                    // Receive the decoded string
                    mq_receive(mq_fd_response, (char *) &rsp, sizeof(rsp), NULL);
                    printf("%s\n", rsp.decodedString);
                }
                break;
            } else { //if not all responses available yet, wait and repeat the same iteration
                sleep(5);
                i--;
            }
        }

        req.md5Request = 0; //command workers to stop

        for (int i = 0; i < NROF_WORKERS; i++) {
            mq_send(mq_fd_request, (char *) &req, sizeof(req), 0);
        }
            waitpid(processID, NULL, 0);   // Wait for the child

        mq_close(mq_fd_response); // Close response queue
        mq_close(mq_fd_request); // Close request queue
        mq_unlink(mq_name1); // Remove the request queue
        mq_unlink(mq_name2); // Remove the response queue
    }

    

    return (0);
}

