#include "Redis.h"

void Redis::start(const char *Host, unsigned int Port)
{
    connect(Host, Port); //建立redis连接
}

void Redis::connect(const char *Host, unsigned int Port)
{
    conn = redisConnect(Host, Port);           //建立redis连接
    AsyncConn = redisAsyncConnect(Host, Port); //建立异步连接

    if (conn->err || AsyncConn->err)
    {
        std::cout << "(INFO)Redis: Connect error!" << std::endl;
        return;
    }
    std::cout << "(INFO)Redis: Connect successed!" << std::endl;
}

int Redis::PushEmail(const char *receiver, const char *email_id) //去publish到相应频道， 增加id到队列中
{
    std::string add_command = "rpush " + (std::string)receiver + " " + (std::string)email_id;

    redisReply *reply = (redisReply *)redisCommand(conn, add_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "(INFO)Redis: Add email error!" << std::endl;
        freeReplyObject(reply);
        return -1;
    }

    freeReplyObject(reply);
    return 1;
}

int Redis::SetSession(const char *user, MyStruct::SESSION session)
{
    //序列化
    std::string content = se.serialize(session);
    std::string setsession_command = "hset Session " + (std::string)user + " " + content;

    redisReply *reply = (redisReply *)redisCommand(conn, setsession_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "(INFO)Redis: AddSession error!" << std::endl;
        freeReplyObject(reply);
        return -1;
    }

    freeReplyObject(reply);
    return 1;
}

MyStruct::SESSION Redis::GetSession(const char *user)
{
    MyStruct::SESSION session;
    std::string getsession_command = "hget Session " + (std::string)user;

    redisReply *reply = (redisReply *)redisCommand(conn, getsession_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "(INFO)Redis: SetSession error!" << std::endl;
        freeReplyObject(reply);
        return session;
    }

    //反序列化
    session = se.unserialize<MyStruct::SESSION>(reply->str);

    freeReplyObject(reply);
    return session;
}

int Redis::QueryMessageNum(const char *user)
{
    std::string query_command = "llen " + (std::string)user;
    redisReply *reply = (redisReply *)redisCommand(conn, query_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "(INFO)Redis: Query error!" << std::endl;
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