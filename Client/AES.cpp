#include "AES.h"

int AES::HexToInt(char c)
{
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    return 0;
}

AES_KEY AES::AES_set_key(const char *key, int enc)
{
    AES_KEY aes_key;
    if (enc == 1)
        AES_set_encrypt_key((const unsigned char *)key, AES_BLOCK_SIZE * 8, &aes_key);
    else if (enc == 0)
        AES_set_decrypt_key((const unsigned char *)key, AES_BLOCK_SIZE * 8, &aes_key);

    return aes_key;
}

std::string AES::Encrypt(const char *plain, const char *key)
{
    unsigned char iv[AES_BLOCK_SIZE] = {0};
    int len = (strlen(plain) - 1) / AES_BLOCK_SIZE + 1;

    AES_KEY aes_key = AES_set_key(key, AES_ENCRYPT);

    char secret[AES_BLOCK_SIZE * len];
    for (int i = 0; i < len; i++) //循环分组加密
    {
        AES_cbc_encrypt((const unsigned char *)(plain + i * AES_BLOCK_SIZE), (unsigned char *)(secret + i * AES_BLOCK_SIZE),
                        AES_BLOCK_SIZE, &aes_key, iv, AES_ENCRYPT);
    }

    char out[2 * len * AES_BLOCK_SIZE + 1];
    for (int i = 0; i < len * AES_BLOCK_SIZE; i++) //将加密得到的二级制内容转换为16进制字符串
    {
        sprintf(out + i * 2, "%x%x", (secret[i] >> 4) & 0xf, secret[i] & 0xf);
    }

    return (std::string)out;
}

std::string AES::Decrypt(const char *secret, const char *key)
{
    int len = strlen(secret) / 32;
    unsigned char iv[AES_BLOCK_SIZE] = {0};

    char str[AES_BLOCK_SIZE * len];
    for (int i = 0; i < len * AES_BLOCK_SIZE * 2; i += 2) //将需要解密的16进制内容先转化成字符格式
    {
        sprintf(str + i / 2, "%c", HexToInt(secret[i]) << 4 | HexToInt(secret[i + 1]));
    }

    AES_KEY aes_key = AES_set_key(key, AES_DECRYPT);

    char out[len * AES_BLOCK_SIZE + 1];
    for (int i = 0; i < len; i++) //循环分组解密
    {
        AES_cbc_encrypt((const unsigned char *)(str + i * AES_BLOCK_SIZE), (unsigned char *)(out + i * AES_BLOCK_SIZE),
                        AES_BLOCK_SIZE, &aes_key, iv, AES_DECRYPT);
    }

    return (std::string)out;
}
