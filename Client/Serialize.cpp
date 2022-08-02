#include "msg.pb.h"

class Serialize
{
public:
    template <typename T>
    std::string serialize(T);

    template <typename T>
    T unserialize(std::string);
};

template <typename T>
std::string Serialize::serialize(T object)
{
    std::string str_content;
    object.SerializeToString(&str_content);
    return str_content;
}

template <typename T>
T Serialize::unserialize(std::string str_content)
{
    T object;
    object.ParseFromString(str_content);
    return object;
}
