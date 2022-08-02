#include <openssl/sha.h>
#include <string>
#include <iostream>

class SHA
{
public:
    // sha256 hash
    std::string Hash(std::string);
};

std::string SHA::Hash(std::string data)
{
    unsigned char sha_res[SHA256_DIGEST_LENGTH];
    char hex_res[SHA256_DIGEST_LENGTH * 2];

    SHA256((const unsigned char *)data.c_str(), data.length(), sha_res);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(hex_res + i * 2, "%x%x", (sha_res[i] >> 4) & 0xf, sha_res[i] & 0xf);
    }

    return (std::string)hex_res;
}