#include "WebServer.h"

int main()
{
    WebServer webserver;
    webserver.Run();
    std::cout << "(INFO)Main: Exit!" << std::endl;
    return 0;
}