#include <iostream>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
// #include <hiredis/adapters/libevent.h>
#include <boost/function.hpp>

#include "Serialize.cpp"
#include "Base64.cpp"

class Redis
{
public:
    //构造一个函数模板，用于回调
    typedef std::function<void(const char *, const char *, void *)> NotifyMessageFn;

    //开启Redis
    void start(const char *Host, unsigned int Port);

    // push邮件到邮件队列
    int PushEmail(MyStruct::EMAIL);

    // 从邮件队列中获取邮件，并且删除
    std::string PullEmail(const char *);

    //设置用户session
    int SetSession(const char *, MyStruct::SESSION);

    //获得用户session
    MyStruct::SESSION GetSession(const char *);

    //查询未读邮件数量
    int QueryEmailNum(const char *);

private:
    redisContext *conn;
    redisAsyncContext *AsyncConn;
    // event_base *pBase;
    NotifyMessageFn _notify_message_fn;
    Serialize se;

    void connect(const char *, unsigned int);
};