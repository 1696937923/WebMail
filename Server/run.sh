#!/bin/bash

workdir=$(
    cd $(dirname $0)
    pwd
)
libdir=''$workdir'/lib/'

g++ -fPIC -shared -w ./src/RSA.cpp -o ./lib/libRSA.so
g++ -fPIC -shared -w ./src/Redis.cpp -o ./lib/libRedis.so
g++ -fPIC -shared -w ./src/Mysql.cpp -o ./lib/libMysql.so
g++ -fPIC -shared -w ./src/SmtpServer.cpp -o ./lib/libSMTP.so
g++ -fPIC -shared -w ./src/WebServer.cpp -o ./lib/libWebServer.so

g++ ./src/Main.cpp ./src/msg.pb.cc -w -L $libdir \
    -lWebServer -lRedis -lSMTP -lMysql -lRSA \
    -lprotobuf -lcrypto -lmysqlclient -lpthread -lhiredis \
    -lboost_system -DBOOST_BIND_GLOBAL_PLACEHOLDERS -o Server

LD_LIBRARY_PATH=$libdir
export LD_LIBRARY_PATH

chmod +x Server
./Server
