#include <openssl/sha.h>
#include <string>

class SHA
{
public:
    // sha256 hash
    std::string Hash(std::string);
};
