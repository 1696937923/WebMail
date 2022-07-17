
struct SESSION;

template <typename T>
void boost::serialization::serialize(T &ar, SESSION &session, const unsigned int version)
{
    ar &session.login_flag;
    ar &session.socket_fd;
}

struct SESSION
{
    short login_flag;
    int socket_fd;
};