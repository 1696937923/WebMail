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
        std::cout << "[INFO]Redis: Connect error!" << std::endl;
        return;
    }
    std::cout << "[INFO]Redis: Connect successed!" << std::endl;
}

int Redis::PushEmail(MyStruct::EMAIL email) //去publish到相应频道， 增加id到队列中
{
    Base64 bs64;
    //序列化邮件
    std::string content = se.serialize(email);
    std::string bs_content = bs64.Base64Encode(content);
    std::string push_command = "rpush " + email.receiver() + " " + bs_content + "";

    redisReply *reply = (redisReply *)redisCommand(conn, push_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "[INFO]Redis: Push Email error!" << std::endl;
        freeReplyObject(reply);
        return -1;
    }

    freeReplyObject(reply);
    return 1;
}

std::string Redis::PullEmail(const char *user)
{
    std::string pull_command = "lpop " + (std::string)user;

    redisReply *reply = (redisReply *)redisCommand(conn, pull_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "[INFO]Redis: Pull Email error!" << std::endl;
        freeReplyObject(reply);
        return "";
    }

    std::string email = reply->str;

    freeReplyObject(reply);
    return email;
}

int Redis::SetSession(const char *user, MyStruct::SESSION session)
{
    //序列化
    std::string content = se.serialize(session);
    std::string setsession_command = "hset Session " + (std::string)user + " " + content;

    redisReply *reply = (redisReply *)redisCommand(conn, setsession_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "[INFO]Redis: Set Session error!" << std::endl;
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
    if (reply->type == REDIS_REPLY_ERROR || reply->str == "")
    {
        std::cout << "[INFO]Redis: Get Session error!" << std::endl;
        freeReplyObject(reply);
        return session;
    }

    //反序列化
    std::string session_str = reply->str;
    session = se.unserialize<MyStruct::SESSION>(session_str);

    freeReplyObject(reply);
    return session;
}

int Redis::QueryEmailNum(const char *user)
{
    std::string query_command = "llen " + (std::string)user;
    redisReply *reply = (redisReply *)redisCommand(conn, query_command.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        std::cout << "[INFO]Redis: Query Email error!" << std::endl;
        freeReplyObject(reply);
        return -1;
    }

    int unread_num = reply->integer;

    freeReplyObject(reply);
    return unread_num;
}
