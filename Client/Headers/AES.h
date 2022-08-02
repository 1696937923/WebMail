#ifndef AES_H
#define AES_H
#include <iostream>
#include <string.h>
#include <string>
#include <openssl/aes.h>

class AES
{
public:
    std::string Encrypt(const char *, const char *);
    std::string Decrypt(const char *, const char *);

private:
    int HexToInt(char);
    AES_KEY AES_set_key(const char *, int);
};
#endif // AES_H
