#include "Mysql.h"

void Mysql::start(const char *host, const char *user, const char *passwd, const char *db, unsigned int port)
{
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, host, user, passwd, db, port, NULL, 0))
    {
        std::cout << "[INFO]Db: Connect error! -> " << mysql_error(&mysql) << std::endl;
        return;
    }
    std::cout << "[INFO]Db: Connect successed!" << std::endl;
}

int Mysql::Login(const char *username, const char *password)
{
    std::string query_sql = "select id,password from User where email = '" + (std::string)username + "'";

    mysql_query(&mysql, query_sql.c_str());
    MYSQL_RES *res = mysql_store_result(&mysql);

    if (res->row_count == 0 || strcmp(mysql_fetch_row(res)[1], password) != 0)
    {
        std::cout << "[INFO]Login: Username or Password error!" << std::endl;
        mysql_free_result(res);
        return -1;
    }
    std::cout << "[INFO]Login: Login successed!" << std::endl;

    mysql_free_result(res);
    return 1;
}

int Mysql::Register(char *username, char *password)
{
    std::string query_sql = "select * from User where email = '" + (std::string)username + "'";
    mysql_query(&mysql, query_sql.c_str());
    MYSQL_RES *res = mysql_store_result(&mysql);
    if (res->row_count > 0)
    {
        std::cout << "[INFO]Register: User exists!" << std::endl;
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);

    std::string insert_sql = "insert into User values(NULL, '" + (std::string)username + "','" + (std::string)password + "')";
    if (mysql_query(&mysql, insert_sql.c_str()))
    {
        std::cout << "[INFO]Register: Register error!" << std::endl;
        return -2;
    }
    std::cout << "[INFO]Register: Register successed!" << std::endl;

    return 1;
}

int Mysql::Insert_Email(Redis redis, MyStruct::EMAIL email)
{
    //开启一个事务，保证操作1和操作2都完成
    mysql_query(&mysql, "START TRANSACTION");
    std::string insert_sql = "insert into Email values(NULL, '" + email.sender() + "','" + email.receiver() +
                             "','" + email.subject() + "','" + email.date() + "','" + email.content() + "')";

    std::cout << insert_sql << std::endl;
    //操作1-------------------
    if (mysql_query(&mysql, insert_sql.c_str()))
    {
        std::cout << "[INFO]Send_Email: Insert Mysql error!" << std::endl;
        return -1;
    }

    //操作2-------------------
    if (redis.PushEmail(email) < 0)
    {
        std::cout << "[INFO]Send_Email: Insert Redis error!" << std::endl;
        mysql_rollback(&mysql);
        return -1;
    }

    mysql_commit(&mysql);
    std::cout << "[INFO]Send_Email: Send Email Queue successful!" << std::endl;
    return 1;
}

int Mysql::QueryUser(char *username)
{
    std::string query_sql = "select id from User where email = '" + (std::string)username + "'";

    mysql_query(&mysql, query_sql.c_str());
    MYSQL_RES *res = mysql_store_result(&mysql);

    if (res->row_count == 0)
    {
        std::cout << "[INFO]Register: User is not exists!" << std::endl;
        mysql_free_result(res);
        return -1;
    }

    mysql_free_result(res);
    return 1;
}