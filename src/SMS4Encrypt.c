#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "SMS4Encrypt.h"

void SMS4EncryptInit(struct SMS4Encrypt* smsLib)
{
    if (!smsLib) {
        return;
    }
    bzero(smsLib, sizeof(struct SMS4Encrypt));
    smsLib->sms4Password = NULL;
    smsLib->sms4StrInitVec = NULL;
    smsLib->init = SMS4EncryptInit;
    smsLib->setPassword = setSMS4Password;
    smsLib->setStrInitVec = setSMS4StrInitVec;
    smsLib->encrypt = SMS4EncryptEncrypt;
    smsLib->decrypt = SMS4EncryptDecrypt;
    smsLib->destroy = SMS4EncryptDestroy;
}

bool setSMS4Password(struct SMS4Encrypt* smsLib, const char* password)
{
    if (!smsLib->sms4Password) {
        smsLib->sms4Password = malloc(strlen(password) + 1);
        memcpy((void *) smsLib->sms4Password, password, strlen(password) + 1);
        smsLib->sms4Password[strlen(password) + 1] = '\0';
    }
    return true;
}

bool setSMS4StrInitVec(struct SMS4Encrypt* smsLib, const char* vector)
{
    if (!smsLib->sms4StrInitVec) {
        smsLib->sms4StrInitVec = malloc(strlen(vector) + 1);
        memcpy((void *) smsLib->sms4StrInitVec, vector, strlen(vector) + 1);
        smsLib->sms4StrInitVec[strlen(vector) + 1] = '\0';
    }
    return true;
}

//加密
size_t SMS4EncryptEncrypt(struct SMS4Encrypt* smsLib, const char* context, size_t contentLength, char** encryptBody)
{
    //对数据进行加密
    size_t encryptDataLen = ((contentLength / BLOCK_SIZE) + 1) * BLOCK_SIZE;
    //加密的key
    sms4_key_t key;

    sms4_set_encrypt_key(&key, (const unsigned char *)smsLib->sms4Password);

    //填充的码
    char cPad = (char)((BLOCK_SIZE - contentLength % BLOCK_SIZE));
    //进行数据填充
    char inputBody[encryptDataLen + 1];
    memset(inputBody, cPad, encryptDataLen);
    strcpy((char *)inputBody, (void*)context);
    *encryptBody = calloc(encryptDataLen, sizeof(unsigned char));

    //初始化加密缓冲区
    sms4_cbc_encrypt((const unsigned char *)inputBody, (unsigned char *)*encryptBody, encryptDataLen, &key,
                     (unsigned char*)smsLib->sms4StrInitVec, 1);
    return encryptDataLen;
}

//解密
size_t SMS4EncryptDecrypt(struct SMS4Encrypt* smsLib, const char* enContext, size_t enContextLength, char** context)
{
    size_t decryptLen = 0;

    //解密的key
    sms4_key_t deKey;

    //对数据进行加密
    sms4_set_decrypt_key(&deKey, (const unsigned char *)smsLib->sms4Password);

    //重新声明缓冲区大小
    *context = calloc(enContextLength, sizeof(unsigned char));
    //解密
    sms4_cbc_encrypt((unsigned char *)(enContext), (unsigned char *)(*context), enContextLength, &deKey,
            (unsigned char *)smsLib->sms4StrInitVec, 0);
    int value = (*context)[enContextLength - 1];
    decryptLen = (enContextLength - value);
    return decryptLen;
}



void SMS4EncryptDestroy(struct SMS4Encrypt* smsLib)
{
    if (!smsLib) {
        return;
    }

    if (smsLib->sms4Password) {
        free(smsLib->sms4Password);
    }

    if (smsLib->sms4StrInitVec) {
        free(smsLib->sms4StrInitVec);
    }
    free(smsLib);
}
