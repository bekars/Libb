/*
 * =====================================================================================
 *
 *       Filename:  redis_go.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/12/2013 04:25:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  baiyu (bekars), bekars@gmail.com
 *        Company:  BW
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <hiredis/hiredis.h>


void 
doTest(void)
{
    int timeout = 10000;
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = timeout * 1000;
    redisContext* c = redisConnectWithTimeout("127.0.0.1", 6379, tv);
    if (c->err) {
        redisFree(c);
        return;
    }
    const char* command1 = "set stest1 value1";
    redisReply* r = (redisReply*)redisCommand(c,command1);
    if (NULL == r) {
        redisFree(c);
        return;
    }
    
    if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK") == 0)) {
        printf("Failed to execute command[%s].\n",command1);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    freeReplyObject(r);
    printf("Succeed to execute command[%s].\n",command1);

    const char* command2 = "strlen stest1";
    r = (redisReply*)redisCommand(c,command2);
    if (r->type != REDIS_REPLY_INTEGER) {
        printf("Failed to execute command[%s].\n",command2);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    int length = r->integer;
    freeReplyObject(r);
    printf("The length of 'stest1' is %d.\n",length);
    printf("Succeed to execute command[%s].\n",command2);

    const char* command3 = "get stest1";
    r = (redisReply*)redisCommand(c,command3);
    if (r->type != REDIS_REPLY_STRING) {
        printf("Failed to execute command[%s].\n",command3);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    printf("The value of 'stest1' is %s.\n",r->str);
    freeReplyObject(r);
    printf("Succeed to execute command[%s].\n",command3);

    const char* command4 = "get stest2";
    r = (redisReply*)redisCommand(c,command4);
    if (r->type != REDIS_REPLY_NIL) {
        printf("Failed to execute command[%s].\n",command4);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    freeReplyObject(r);
    printf("Succeed to execute command[%s].\n",command4);

    const char* command5 = "mget stest1 stest2";
    r = (redisReply*)redisCommand(c,command5);
    if (r->type != REDIS_REPLY_ARRAY) {
        printf("Failed to execute command[%s].\n",command5);
        freeReplyObject(r);
        redisFree(c);
        assert(2 == r->elements);
        return;
    }
    for (int i = 0; i < r->elements; ++i) {
        redisReply* childReply = r->element[i];
        if (childReply->type == REDIS_REPLY_STRING)
            printf("The value is %s.\n",childReply->str);
    }
    freeReplyObject(r);
    printf("Succeed to execute command[%s].\n",command5);

    printf("Begin to test pipeline.\n");
    if (REDIS_OK != redisAppendCommand(c,command1)
        || REDIS_OK != redisAppendCommand(c,command2)
        || REDIS_OK != redisAppendCommand(c,command3)
        || REDIS_OK != redisAppendCommand(c,command4)
        || REDIS_OK != redisAppendCommand(c,command5)) {
        redisFree(c);
        return;
    }

    redisReply* reply = NULL;
    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
        printf("Failed to execute command[%s] with Pipeline.\n",command1);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command1);

    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
        printf("Failed to execute command[%s] with Pipeline.\n",command2);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command2);

    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
        printf("Failed to execute command[%s] with Pipeline.\n",command3);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command3);

    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
        printf("Failed to execute command[%s] with Pipeline.\n",command4);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command4);

    if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
        printf("Failed to execute command[%s] with Pipeline.\n",command5);
        freeReplyObject(reply);
        redisFree(c);
    }
    freeReplyObject(reply);
    printf("Succeed to execute command[%s] with Pipeline.\n",command5);
    redisFree(c);
    return;
}

int
main(int argc, char **argv)
{
    doTest();
    exit(0);
}

