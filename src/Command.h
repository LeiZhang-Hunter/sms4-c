//
// Created by zhanglei on 2021/4/9.
//

#ifndef SMS4_COMMAND_H
#define SMS4_COMMAND_H

#include <getopt.h>
#include <unistd.h>
#include "Bool.h"
//#include <functional>
//#include <map>

typedef  bool (*cmdClosureType)(void* object, int argc, char **argv, const char);

struct Command
{
    int cmdArgC;

    char **cmdArgV;

    int isInit;

    void (*commandInit)(struct Command *command);

    void (*setCmdArgC)(struct Command *command, int argc);

    void (*setCmdArgV)(struct Command *command, char **argv);

    char **(*getCmdArgv)(struct Command *command);

    int (*getCmdArgC)(struct Command *command);

    ssize_t (*readn)(int fd, void *vptr, size_t n);

    ssize_t (*writen)(int fd, void *vptr, size_t n);

    void (*reg)(struct Command *command, char cmd, cmdClosureType closure, void* args);

    int (*parse)(struct Command *command);

    void  (*commandDestroy)(struct Command *command);

    //注册的命令和闭包map
    cmdClosureType cmdMap[255];

    //注册的命令和指针map
    void* cmdPtrMap[255];

    //注册的命令数目
    int cmdCount;

    //连续的命令
    char* cmdOptionString;
};

void commandInit(struct Command *command);

void setCmdArgC(struct Command *command, int argc);

void setCmdArgV(struct Command *command, char **argv);

void  commandDestroy(struct Command *command);

/**
 * 注册
 * @param cmd
 * @param closure
 * @param args
 */
void commandReg(struct Command *command, char cmd, cmdClosureType closure, void* args);

char **getCmdArgv(struct Command *command);

int getCmdArgC(struct Command *command);

ssize_t readn(int fd, void *vptr, size_t n);

ssize_t writen(int fd, void *vptr, size_t n);

int parse(struct Command *command);

//namespace src {
//    class Command {
//        typedef std::function<bool(int argc, char **argv, const char &)> cmdClosureType;
//        typedef std::map<char, cmdClosureType> cmdMapType;
//        typedef std::map<const char, const std::string &> cmdResultMapType;
//    public:
//        void setCmdArgC(int argc) {
//            cmdArgC = argc;
//        }
//
//        void setCmdArgV(char **argv) {
//            cmdArgV = argv;
//        }
//
//        /**
//         * 将注册的命令加入到Map中
//         * @param cmd
//         */
//        void reg(const char &cmd, const cmdClosureType &closure) {
//            cmdMap[cmd] = closure;
//        }
//
//        char **getCmdArgv() {
//            return cmdArgV;
//        }
//
//        int getCmdArgC() {
//            return cmdArgC;
//        }
//
//        ssize_t readn(int fd, void *vptr, size_t n);
//
//        ssize_t writen(int fd, void *vptr, size_t n);
//
//        bool parse();
//
//    private:
//        cmdMapType cmdMap;
//        cmdResultMapType cmdResultMap;
//        int cmdArgC = 0;
//        char **cmdArgV = nullptr;
//    };
//}

#endif //SMS4_COMMAND_H
