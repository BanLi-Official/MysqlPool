#include <ConnPool.hpp>
#include "json.h"
#include <fstream>  //读取文件要用

using namespace Json;

ConnPool *ConnPool::getConnPoll()
{
    static ConnPool *pool;
    return pool;
}

bool ConnPool::parseConfigJson()
{
    ifstream file("../json/configJson.json"); //将这个json文件读出来

    Reader reader;
    Value root;
    reader.parse(file,root);   //交给jsoncpp去解析

    if(root.isObject())
    {
        ip=root["ip"].asString();
        port=root["port"].asInt();
        user=root["user"].asString();
        psw=root["psw"].asString();
        dbName=root["dbName"].asString();
        max=root["max"].asInt();
        min=root["min"].asInt();
        timeout=root["timeout"].asInt();
        maxldleTime=root["maxldleTime"].asInt();
        return true;
    }

    return false;

}
