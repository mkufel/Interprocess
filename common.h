/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * Ahmed Ahres 0978238
 * Maciej Kufel 0944597
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
static char    mq_name1[80];
static char    mq_name2[80];

typedef struct
{
    // a data structure with 2 members
    uint128_t md5Request;
    char startingChar;
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // a data structure with 2 members
    char      decodedString[MAX_MESSAGE_LENGTH];
    uint128_t md5Response;
} MQ_RESPONSE_MESSAGE;

#endif

