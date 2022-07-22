#include <string>
#include "../ProtoBuffer/msg.pb.h"

class Serialize
{
public:
    template <typename T>
    std::string serialize(T);

    template <typename T>
    T unserialize(std::string);
};
