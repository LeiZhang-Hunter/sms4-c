//
// Created by zhanglei on 2020/11/10.
//
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "Command.h"


void commandInit(struct Command *command) {
    bzero(command, sizeof(struct Command));
    bzero(command->cmdMap, sizeof(command->cmdMap));
    bzero(command->cmdPtrMap, sizeof(command->cmdPtrMap));

    command->cmdCount = 0;
    command->cmdArgC = 0;
    command->cmdArgV = NULL;
    command->commandInit = commandInit;
    command->setCmdArgC = setCmdArgC;
    command->setCmdArgV = setCmdArgV;
    command->getCmdArgv = getCmdArgv;
    command->getCmdArgC = getCmdArgC;
    command->reg = commandReg;
    command->readn = readn;
    command->writen = writen;
    command->parse = parse;
    command->commandDestroy = commandDestroy;
}

void setCmdArgC(struct Command *command, int argc) {
    command->cmdArgC = argc;
}

void setCmdArgV(struct Command *command, char **argv) {
    command->cmdArgV = argv;
}

void commandDestroy(struct Command *command) {
    free(command);
}

/**
 * 将注册的命令加入到Map中
 * @param cmd
 */
void commandReg(struct Command *command, char cmd, cmdClosureType closure, void *args) {
    command->cmdMap[cmd] = closure;
    command->cmdPtrMap[cmd] = args;
    command->cmdCount++;
}

char **getCmdArgv(struct Command *command) {
    return command->cmdArgV;
}

int getCmdArgC(struct Command *command) {
    return command->cmdArgC;
}

ssize_t readn(int fd, void *vptr, size_t n) {
    ssize_t nleft;
    ssize_t nread;
    char *ptr;
    ptr = (char *) vptr;

    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        } else if (nread == 0) {
            break;
        }

        nleft -= nread;
        ptr += nread;
    }

    return n - nleft;
}

ssize_t writen(int fd, void *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr = (const char *) vptr;
    nleft = n;

    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else {
                return -1;
            }


        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

int parse(struct Command *command) {

    //检查argc
    if (command->cmdArgC == 0) {
        printf("Please set argc\n");
        exit(-1);
    }


    if (!command->cmdArgV) {
        printf("Please set argv\n");
        exit(-1);
    }

    int i = 0;
    int cmdOptionCount = 0;
    command->cmdOptionString = malloc(command->cmdCount + 1);
    //申请指定的空间
    for(i=0; i < 255; i++) {
        if (command->cmdMap[i]) {
            command->cmdOptionString[cmdOptionCount] = (char)i;
            cmdOptionCount++;
        }
    }

    command->cmdOptionString[command->cmdCount + 1] = '\0';

    int opt = 0;
    char charOpt;
    while ((opt = getopt(command->cmdArgC, command->cmdArgV, command->cmdOptionString)) != -1) {
        charOpt = (char)(opt);
        cmdClosureType function = command->cmdMap[charOpt];
        if (function) {
            function(command->cmdPtrMap[charOpt], command->cmdArgC, command->cmdArgV, charOpt);
        }

    }

    free(command->cmdOptionString);
    command->cmdOptionString = NULL;
    return true;
}


