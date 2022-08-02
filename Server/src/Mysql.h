#include <mysql/mysql.h>
#include <iostream>
#include <string.h>

#include "Redis.h"

class Mysql
{
public:
    void start(const char *, const char *, const char *, const char *, unsigned int);
    int Login(const char *, const char *);
    int Register(char *, char *);
    int Insert_Email(Redis, MyStruct::EMAIL);
    int QueryUser(char *username);

private:
    MYSQL mysql;
};