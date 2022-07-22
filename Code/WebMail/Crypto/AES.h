#include <string>
#include <iostream>
#include <openssl/aes.h>

class AES
{
public:
    std::string Encrypt(const char *, char *);
    std::string Decrypt(const char *, char *);

private:
    int HexToInt(char);
    AES_KEY AES_set_key(char *, int);
};
