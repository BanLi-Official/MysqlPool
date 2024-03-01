#include <ConnPool.hpp>
#include "json.h"
#include <fstream>  //读取文件要用
#include <iostream> 
#include <thread>   //C++11新特性
#include <chrono> //时间库


using namespace std;
using namespace Json;
using namespace chrono;


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

bool ConnPool::createConn()
{
    MysqlConn *conn=new MysqlConn();
    int res=conn->mysqlConnect(ip.c_str(),user.c_str(),psw.c_str(),dbName.c_str(),3306);
    conn->setStartTime();
    if(res==false)
    {
        cout<<"创建一条数据库连接失败"<<endl;
        return false;
    }
    sizeNow++;
    ConnQ.push(conn);
    return true;
}

void ConnPool::produceConn()   //生产者
{
    while (true) //不断循环生产，当不符合生产条件的时候就阻塞了
    {
        unique_lock<mutex> locker(mutexQ);
        while (ConnQ.size()>=min)
        {
            m_cond.wait(locker);
        }
        if(sizeNow < max)   //当前的数据库连接总数小于最大连接数则允许创建新的连接
        {
            createConn();
            m_cond.notify_all();
        }
    }
    return ;
}

void ConnPool::recycleConn()
{
    while (true)
    {

        this_thread::sleep_for(chrono::milliseconds(500)); //^^^^
        unique_lock<mutex> locker(mutexQ);
        while (ConnQ.size()>min)
        {
            MysqlConn *conn=ConnQ.front();
            if(conn->getAliveTime() > maxldleTime) //如果空闲时间大于最大空闲时间则关闭这个链接
            {
                ConnQ.pop();
                delete conn;
                sizeNow--;
            }
            else
            {
                break;
            }
        }
        
    }
    
    return ;
}

shared_ptr<MysqlConn> ConnPool::getConn() //消费者
{
    //首先要保证连接池里面有东西
    //如果为空则要阻塞，直到有生产者通知生产了一条连接，如果超时则继续卡在循环
    unique_lock<mutex> locker(mutexQ);
    while(ConnQ.empty())
    {
       if( cv_status::timeout == m_cond.wait_for(locker,chrono::milliseconds(timeout)))
       {
            if (ConnQ.empty())
            {
                continue;
            }
       }
    }

    //取出一条连接，并弹出池子通知生产者,利用智能指针修改并实现连接的回收,初始化参数中第二个参数是一个用lamba表达式写的匿名函数，也可以使用常规函数
    shared_ptr<MysqlConn> conn(ConnQ.front(),[this](MysqlConn * Conn){
        unique_lock<mutex> locker(mutexQ);
        Conn->setStartTime();  //重新设置起始时间
        ConnQ.push(Conn);
        m_cond.notify_all(); //通知被阻塞的消费者，归还了一个连接

    });
    ConnQ.pop();
    m_cond.notify_all();   //通知被阻塞的生产者

    
    return conn;
}

ConnPool::~ConnPool()  
{
    while (!ConnQ.empty())
    {
        MysqlConn *conn = ConnQ.front();
        ConnQ.pop();
        delete conn;
    }
    
}

ConnPool::ConnPool() // 构造函数
{

    if(!parseConfigJson())
    {
        cout<<"分析配置文件失败！"<<endl;
    }

    sizeNow=0;
    for(int i=0;i<min;i++)
    {
        createConn();
    }

    //分别整出两个线程，分别用来创建和删除数据库连接
    thread producer(&ConnPool::produceConn,this);
    thread recycler(&ConnPool::recycleConn,this);
    producer.detach();
    recycler.detach();



    
}
