//
// Created by zhanglei on 2021/4/9.
//

#ifndef SMS4_ENCRYPTWORKER_H
#define SMS4_ENCRYPTWORKER_H


#include "SMS4Encrypt.h"
#include "Command.h"


struct EncryptWorker {
    struct Command *command;

    struct SMS4Encrypt *encrypt;

    char *sms4Password;

    char *sms4StrInitVec;

    char *inputFile;

    char *outputFile;

    char *operation;

    int (*encryptWorkerInit)(struct EncryptWorker *worker);

    int (*setCommandArgc)(struct EncryptWorker *worker, int argc);

    int (*setCommandArgv)(struct EncryptWorker *worker, char **argv);

    int (*setInputFile)(void *worker, int argc, char **argv, const char cmd);

    int (*setOutputFile)(void *worker, int argc, char **argv, const char cmd);

    int (*setPassword)(void *worker, int argc, char **argv, const char cmd);

    int (*setVector)(void *worker, int argc, char **argv, const char cmd);

    int (*setOperation)(void *worker, int argc, char **argv, const char cmd);

    void (*run)(struct EncryptWorker *worker);

    int (*encryptWorkerDestroy)(struct EncryptWorker *worker);
};

int encryptWorkerInit(struct EncryptWorker *worker);

int setCommandArgc(struct EncryptWorker *worker, int argc);

int setCommandArgv(struct EncryptWorker *worker, char **argv);

int setInputFile(void *worker, int argc, char **argv, char cmd);

int setOutputFile(void *worker, int argc, char **argv, char cmd);

int setPassword(void *worker, int argc, char **argv, char cmd);

int setVector(void *worker, int argc, char **argv, char cmd);

int setOperation(void *worker, int argc, char **argv, char cmd);

void run(struct EncryptWorker *worker);

int encryptWorkerDestroy(struct EncryptWorker *worker);


#endif //SMS4_ENCRYPTWORKER_H
