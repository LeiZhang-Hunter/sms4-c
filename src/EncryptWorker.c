#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "Bool.h"
#include "EncryptWorker.h"

/**
 * 初始化worker程序
 * @param worker
 * @return
 */
bool encryptWorkerInit(struct EncryptWorker *worker) {
    //初始化命令行程序
    worker->command = malloc(sizeof(struct Command));
    commandInit(worker->command);

    //初始化加密库
    worker->encrypt = malloc(sizeof(struct SMS4Encrypt));
    SMS4EncryptInit(worker->encrypt);

    worker->sms4Password = NULL;
    worker->sms4StrInitVec = NULL;
    worker->inputFile = NULL;
    worker->outputFile = NULL;
    worker->operation = NULL;
    worker->encryptWorkerInit = encryptWorkerInit;
    worker->encryptWorkerDestroy = encryptWorkerDestroy;
    worker->setCommandArgc = setCommandArgc;
    worker->setCommandArgv = setCommandArgv;
    worker->setInputFile = setInputFile;
    worker->setOutputFile = setOutputFile;
    worker->setPassword = setPassword;
    worker->setVector = setVector;
    worker->setOperation = setOperation;
    worker->run = run;
    return true;
}

bool setCommandArgc(struct EncryptWorker *worker, int argc) {
    worker->command->setCmdArgC(worker->command, argc);
    return true;
}

bool setCommandArgv(struct EncryptWorker *worker, char **argv) {
    worker->command->setCmdArgV(worker->command, argv);
    return true;
}

bool setInputFile(void *worker, int argc, char **argv, char cmd) {
    if (!((struct EncryptWorker*)worker)->inputFile) {
        ((struct EncryptWorker *) worker)->inputFile = malloc(strlen(argv[optind]) + 1);
        memcpy(((struct EncryptWorker *) worker)->inputFile, argv[optind], strlen(argv[optind]));
        ((struct EncryptWorker *) worker)->inputFile[strlen(argv[optind]) + 1] = '\0';
    }
    return true;
}

bool setOutputFile(void *worker, int argc, char **argv, char cmd) {
    struct EncryptWorker* handle = (struct EncryptWorker*)worker;
    if (!handle->outputFile) {
        handle->outputFile = malloc(strlen(argv[optind]) + 1);
        memcpy(handle->outputFile, argv[optind], strlen(argv[optind]));
        handle->outputFile[strlen(argv[optind]) + 1] = '\0';
    }
    return true;
}

bool setPassword(void *worker, int argc, char **argv, char cmd) {
    if (!((struct EncryptWorker*)worker)->sms4Password) {
        ((struct EncryptWorker *) worker)->sms4Password = malloc(strlen(argv[optind]) + 1);
        memcpy(((struct EncryptWorker *) worker)->sms4Password, argv[optind], strlen(argv[optind]));
        ((struct EncryptWorker *) worker)->sms4Password[strlen(argv[optind]) + 1] = '\0';
    }
    return true;
}

bool setVector(void *worker, int argc, char **argv, char cmd) {
    if (!((struct EncryptWorker*)worker)->sms4StrInitVec) {
        ((struct EncryptWorker *) worker)->sms4StrInitVec = malloc(strlen(argv[optind]) + 1);
        memcpy(((struct EncryptWorker *) worker)->sms4StrInitVec, argv[optind], strlen(argv[optind]));
        ((struct EncryptWorker *) worker)->sms4StrInitVec[strlen(argv[optind]) + 1] = '\0';
    }
    return true;
}

bool setOperation(void *worker, int argc, char **argv, char cmd) {
    if (!((struct EncryptWorker*)worker)->operation) {
        ((struct EncryptWorker *) worker)->operation = malloc(strlen(argv[optind]) + 1);
        memcpy(((struct EncryptWorker *) worker)->operation, argv[optind], strlen(argv[optind]));
        ((struct EncryptWorker *) worker)->operation[strlen(argv[optind]) + 1] = '\0';
    }
    return true;
}

/**
 * 内存销毁
 * @param worker
 * @return
 */
bool encryptWorkerDestroy(struct EncryptWorker *worker) {


    if (worker->sms4Password) {
        free(worker->sms4Password);
    }

    if (worker->sms4StrInitVec) {
        free(worker->sms4StrInitVec);
    }

    if (worker->inputFile) {
        free(worker->inputFile);
    }

    if (worker->outputFile) {
        free(worker->outputFile);
    }

    if (worker->operation) {
        free(worker->operation);
    }

    worker->command->commandDestroy(worker->command);
    worker->encrypt->destroy(worker->encrypt);
}

void run(struct EncryptWorker *worker) {
    //注入命令处理的回调函数

    worker->command->reg(worker->command, 'i', worker->setInputFile, (void *)worker);
    worker->command->reg(worker->command, 'o', worker->setOutputFile, (void *)worker);
    worker->command->reg(worker->command, 'p', worker->setPassword, (void *)worker);
    worker->command->reg(worker->command, 'v', worker->setVector, (void *)worker);
    worker->command->reg(worker->command, 'd', worker->setOperation, (void *)worker);
    worker->command->parse(worker->command);

    //输入参数检查
    if (!worker->inputFile) {
        printf("-i empty!\n");
        exit(-1);
    }

    if (!worker->outputFile) {
        printf("-o empty!\n");
        exit(-1);
    }

    if (!worker->sms4Password) {
        printf("-p empty!\n");
        exit(-1);
    }

    if (!worker->sms4StrInitVec) {
        printf("-v empty!\n");
        exit(-1);
    }

    worker->encrypt->setPassword(worker->encrypt, worker->sms4Password);
    worker->encrypt->setStrInitVec(worker->encrypt, worker->sms4StrInitVec);

    unlink(worker->outputFile);

    int writeFd;
    writeFd = open(worker->outputFile , O_CREAT | O_RDWR| O_APPEND, 0664);
    if (writeFd == -1) {
        printf("open outputFile error: %s ;error:%s", worker->outputFile, strerror(errno));
        exit(-1);
    }

    int readFd;
    readFd = open(worker->inputFile , O_CREAT | O_RDWR, 0664);
    if (readFd == -1) {
        printf("open inputFile error: %s ;error:%s", worker->inputFile, strerror(errno));
        exit(-1);
    }


    size_t readBytes;
    char *encryptString;
    char *decryptString;
    size_t encryptLength;

    //是否是加密
    bool isEncrypt = true;

    if (!strcmp(worker->operation, "de")) {
        isEncrypt = false;
    }

    size_t buffer_size = 0;
    if (isEncrypt) {
        buffer_size = BUFSIZ;
    } else {
        buffer_size += ((BUFSIZ / BLOCK_SIZE) + 1) * BLOCK_SIZE;
    }
    char buffer[buffer_size];

    //读取文件
    while (1) {
        readBytes = worker->command->readn(readFd, (void *)(buffer), buffer_size);

        if (readBytes == 0) {
            break;
        }

        if (readBytes < buffer_size) {
            buffer[readBytes] = '\0';
        }

        //加密的情况直接去读BUFSIZE 8192
        if (isEncrypt) {
            encryptString = NULL;
            encryptLength = worker->encrypt->encrypt(worker->encrypt, buffer, readBytes, &encryptString);
            worker->command->writen(writeFd, (void *)encryptString, encryptLength);
            if (encryptString) {
                free(encryptString);
            }
        } else {
            //确定读取的块大小
            decryptString = NULL;
            encryptLength = worker->encrypt->decrypt(worker->encrypt, buffer, readBytes, &decryptString);
            worker->command->writen(writeFd, (void *)decryptString, encryptLength);
            if (decryptString) {
                free(decryptString);
            }
        }
    }

    close(writeFd);
    close(readFd);
}