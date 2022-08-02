#include "RSA.h"
#include <sys/socket.h>
#include <iostream>

class ExchangeKey
{
public:
    std::string ExchangeAsServer(int);
    std::string ExchangeAsClient(int);

private:
    RSA rsa;
    std::string GenRandomStr(int);

    const char seed[62] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
        'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
        'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
        'y', 'z'};
};

std::string ExchangeKey::ExchangeAsServer(int socket_fd)
{
    char buffer[1024] = {0};
    int ret = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (ret <= 0)
    {
        std::cout << "[INFO]Server: ExchangeKey error!" << std::endl;
        return "";
    }

    ret = send(socket_fd, "ok", 2, 0);
    if (ret <= 0)
    {
        std::cout << "[INFO]Server: ExchangeKey error!" << std::endl;
        return "";
    }

    std::string Aes_Key = rsa.Decrypt(buffer);
    std::cout << "[INFO]Server: ExchangeKey successful!" << std::endl;
    return Aes_Key;
}

std::string ExchangeKey::ExchangeAsClient(int socket_fd)
{
    std::string randomstr = GenRandomStr(16);
    std::string Aes_Key = rsa.Encrypt(randomstr);

    if (Aes_Key == "")
        return "";

    int ret = send(socket_fd, Aes_Key.c_str(), Aes_Key.length(), 0);
    if (ret <= 0)
    {
        std::cout << "[INFO]Client: ExchangeKey error!" << std::endl;
        return "";
    }

    char buffer[16] = {0};
    ret = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (ret <= 0 && strncmp("ok", buffer, strlen(buffer)) != 0)
    {
        std::cout << "[INFO]Client: ExchangeKey error!" << std::endl;
        return "";
    }

    std::cout << "[INFO]Client: ExchangeKey successful!" << std::endl;
    return randomstr;
}

std::string ExchangeKey::GenRandomStr(int len)
{
    std::string res;
    std::random_device rd; // 随机数种子
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 61);

    for (int i = 0; i < len; i++)
    {
        int val = dis(gen);
        res.push_back(seed[val]);
    }

    return res;
}
