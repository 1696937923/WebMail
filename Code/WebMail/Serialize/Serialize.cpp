#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "../ProtoBuffer/msg.pb.h"

using namespace std;
class Serialize
{
public:
    template <typename T>
    string serialize(T);

    template <typename T>
    T unserialize(string);
};

template <typename T>
string Serialize::serialize(T object)
{
    string str_content;
    object.SerializeToString(&str_content);
    return str_content;
}

template <typename T>
T Serialize::unserialize(string str_content)
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
//     cout << "Serialize: " << content << endl;

//     MyStruct::SESSION msg2 = se.unserialize<MyStruct::SESSION>(content);
//     cout << msg2.socket_fd() << " " << msg2.login_flag() << endl;
// }