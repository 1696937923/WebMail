#include "Base64.h"

// 编码
std::string Base64::Base64Encode(std::string input)
{
    std::string output;
    std::size_t len = input.size();

    output.resize(boost::beast::detail::base64::encoded_size(len));
    output.resize(boost::beast::detail::base64::encode(&output[0], input.c_str(), len));

    return output;
}

// 解码
std::string Base64::Base64Decode(std::string input)
{
    std::string output;
    std::size_t len = input.size();

    output.resize(boost::beast::detail::base64::decoded_size(len));
    auto result = boost::beast::detail::base64::decode(&output[0], input.data(), len);
    output.resize(result.first);

    return output;
}
