#include "openssl/sms4.h"
#include "EncryptWorker.h"
#include <stdlib.h>
#include <stdio.h>

#define BLOCK_SIZE 16
int main(int argc,char* argv[])
{
    struct EncryptWorker worker;
    encryptWorkerInit(&worker);
    worker.setCommandArgc(&worker, argc);
    worker.setCommandArgv(&worker, argv);
    worker.run(&worker);
    worker.encryptWorkerDestroy(&worker);
}