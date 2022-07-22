#include "Serialize.h"

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

// int main()
// {
//     Serialize se;
//     MyStruct::SESSION session;
//     session.set_socket_fd(2);
//     session.set_login_flag(1);

//     string content = se.serialize(session);
//     cout << "Serialize: " << content.length() << endl;

//     MyStruct::SESSION msg2 = se.unserialize<MyStruct::SESSION>(content);
//     cout << msg2.socket_fd() << " " << msg2.login_flag() << endl;
// }