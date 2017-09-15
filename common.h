/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions which are commonly used by the farmer and the workers
 *
 */

#ifndef COMMON_H
#define COMMON_H


// maximum size for any message in the tests
#define MAX_MESSAGE_LENGTH	6
 

// TODO: put your definitions of the datastructures here
// two message queues
static char    mq_name1[MQ_MAX_MESSAGES];
static char    mq_name2[MQ_MAX_MESSAGES];

typedef struct
{
    // a data structure with 2 members
    char    startingPoint;
    char    md5[32];
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // a data structure with 2 members
    char    hashedValue[32];
    char    md5[32];
} MQ_RESPONSE_MESSAGE;

#endif

