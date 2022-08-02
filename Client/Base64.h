#ifndef BASE64_H
#define BASE64_H

#include <iostream>
#include <boost/beast/core/detail/base64.hpp>

class Base64
{
public:
    std::string Base64Encode(std::string);
    std::string Base64Decode(std::string);
};

#endif // BASE64_H
