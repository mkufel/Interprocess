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
 

// two message queues
static char    mq_name1[80];
static char    mq_name2[80];

typedef struct
{
    // Request message data structure
    uint128_t md5Request; // Hash value
    char startingChar; // Starting character
} MQ_REQUEST_MESSAGE;

typedef struct
{
    // Response message data structure
    char      decodedString[MAX_MESSAGE_LENGTH]; // String corresponding to hash value
    uint128_t md5Response; // Hash value, same as md5Request
} MQ_RESPONSE_MESSAGE;

#endif

