//
// Created by zhanglei on 2021/4/9.
//

#ifndef SMS4_SMS4ENCRYPT_H
#define SMS4_SMS4ENCRYPT_H

//#include <string>
//#include <vector>
#include "sms4.h"
#include "Bool.h"

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 16
#endif

struct SMS4Encrypt
{
    char* sms4Password;
    char* sms4StrInitVec;

    //加密
    size_t (*encrypt)(struct SMS4Encrypt* smsLib, const char* context, size_t contentLength, char** encryptBody);

    //解密
    size_t (*decrypt)(struct SMS4Encrypt* smsLib, const char* enContext, size_t enContextLength, char** context);

    void (*init)(struct SMS4Encrypt* smsLib);

    void (*destroy)(struct SMS4Encrypt* smsLib);

    bool (*setPassword)(struct SMS4Encrypt* smsLib, const char* password);

    bool (*setStrInitVec)(struct SMS4Encrypt* smsLib, const char* vector);
};

//加密
size_t SMS4EncryptEncrypt(struct SMS4Encrypt* smsLib, const char* context, size_t contentLength, char** encryptBody);

//解密
size_t SMS4EncryptDecrypt(struct SMS4Encrypt* smsLib, const char* enContext, size_t enContextLength, char** context);

bool setSMS4Password(struct SMS4Encrypt* smsLib, const char* password);

bool setSMS4StrInitVec(struct SMS4Encrypt* smsLib, const char* password);

void SMS4EncryptInit(struct SMS4Encrypt* smsLib);

void SMS4EncryptDestroy(struct SMS4Encrypt* smsLib);

#endif //SMS4_SMS4ENCRYPT_H
