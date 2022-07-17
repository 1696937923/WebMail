#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <thread>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>
#include <boost/function.hpp>
#include "../Serialize/Serialize.cpp"
using namespace std;

class Redis
{
public:
    typedef function<void(const char *, const char *, void *)> NotifyMessageFn; // 回调函数对象类型，当接收到消息后调用回调把消息发送出去
    void start(const char *Host, unsigned int Port);
    int PushEmail(const char *, const char *);
    int SetSession(const char *, MyStruct::SESSION);
    MyStruct::SESSION GetSession(const char *);
    int QueryMessageNum(const char *);

private:
    redisContext *conn;
    redisAsyncContext *AsyncConn;
    event_base *pBase;
    NotifyMessageFn _notify_message_fn;
    Serialize se;

    void connect(const char *, unsigned int);
};

void Redis::start(const char *Host, unsigned int Port)
{
    connect(Host, Port); //建立redis连接
}

void Redis::connect(const char *Host, unsigned int Port)
{
    conn = redisConnect(Host, Port); //建立redis连接
    AsyncConn = redisAsyncConnect(Host, Port);

    if (conn->err || AsyncConn->err)
    {
        cout << "(INFO)Redis: Connect error!" << endl;
        return;
    }
    cout << "(INFO)Redis: Connect successed!" << endl;
}

int Redis::PushEmail(const char *receiver, const char *email_id) //去publish到相应频道， 增加id到队列中
{
    string pub_command = "publish " + (string)receiver + " " + (string)email_id;
    string add_command = "rpush " + (string)receiver + " " + (string)email_id;
    string add_command_rollback = "rpop " + (string)receiver;

    redisReply *reply1 = (redisReply *)redisCommand(conn, add_command.c_str());
    if (reply1->type == REDIS_REPLY_ERROR)
    {
        cout << "(INFO)Redis: Add email error!" << endl;
        freeReplyObject(reply1);
        return -1;
    }
    freeReplyObject(reply1);
    cout << "Redis Add email OK" << endl; //

    return 1;
}

int Redis::SetSession(const char *user, MyStruct::SESSION session)
{
    string content = se.serialize(session);
    string setsession_command = "hset Session " + (string)user + " " + content;

    redisReply *reply = (redisReply *)redisCommand(conn, setsession_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        cout << "(INFO)Redis: AddSession error!" << endl;
        freeReplyObject(reply);
        return -1;
    }
    freeReplyObject(reply);
    return 1;
}

MyStruct::SESSION Redis::GetSession(const char *user)
{
    MyStruct::SESSION session;
    string getsession_command = "hget Session " + (string)user;

    redisReply *reply = (redisReply *)redisCommand(conn, getsession_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        cout << "(INFO)Redis: SetSession error!" << endl;
        freeReplyObject(reply);
        return session;
    }
    session = se.unserialize<MyStruct::SESSION>(reply->str);
    cout << "reply type:: " << reply->type << endl;

    freeReplyObject(reply);
    return session;
}

int Redis::QueryMessageNum(const char *user)
{
    string query_command = "llen " + (string)user;
    redisReply *reply = (redisReply *)redisCommand(conn, query_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        cout << "(INFO)Redis: Query error!" << endl;
        freeReplyObject(reply);
        return -1;
    }
    int unread_num = reply->integer;
    freeReplyObject(reply);
    return unread_num;
}

// int main()
// {
// Redis redis;
// redis.start("127.0.0.1", 6379, ListeningEmail, NULL);

// MyStruct::SESSION session;
// session.set_socket_fd(2);
// session.set_login_flag(1);

// string content;
// session.SerializeToString(&content);
// cout << "Serialize: " << content << endl;
// redis.Subscribe(content.c_str());
// while (true)
// {
//     sleep(1);
// }
// }