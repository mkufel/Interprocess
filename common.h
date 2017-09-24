/* 
 * Operating Systems [2INCO] Practical Assignment
 * Interprocess Communication
 *
 * Group 21
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

// two message queues
static char    mq_name1[80];
static char    mq_name2[80];

typedef struct
{
    // Request message data structure with 2 members
    uint128_t md5Request; // The requested hash
    char startingChar; // The starting character of the decoded string
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // Response message data structure with 2 members
    char      decodedString[MAX_MESSAGE_LENGTH]; // The decoded string corresponding to the hash
    uint128_t md5Response; // Same as md5Request, just to return the decoded string with what it corresponds to
} MQ_RESPONSE_MESSAGE;

#endif

