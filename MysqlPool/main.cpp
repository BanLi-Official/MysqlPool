#include "ConnPool.hpp"
#include "ThreadPool.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

using namespace chrono;
using namespace std;

struct arg1
{
    int start;
    int end;
} arg1;

struct arg2
{
    ConnPool *pool;
    int start;
    int end;
} arg2;

bool query_table()
{
    MysqlConn *conn = new MysqlConn();
    bool res = conn->mysqlConnect("192.168.200.129", "root", "12345678", "scott", 3306);
    if (!res)
    {
        cout << "函数错误，返回！" << endl;
        return false;
    }

    res = conn->update("insert into dept value(2,'hello','world')");
    if (!res)
    {
        cout << "数据库更新错误，返回！" << endl;
        return false;
    }

    conn->showTable("dept");
    delete conn;

    return true;
}

bool clear_table()
{
    MysqlConn *conn = new MysqlConn();
    bool res = conn->mysqlConnect("192.168.200.129", "root", "12345678", "scott", 3306);
    if (!res)
    {
        cout << "函数错误，返回！" << endl;
        return false;
    }

    res = conn->update("delete from dept");
    if (!res)
    {
        cout << "数据库更新错误，返回！" << endl;
        return false;
    }

    // conn->showTable("dept");
    delete conn;

    return true;
}

/*
    写两个测试函数创建5000条插入
    1.单线程：手动创建数据库的连接 / 使用数据库连接池
    1.多线程：手动创建数据库的连接 / 使用数据库连接池
*/

void insertByHand(int begin, int end)
{
    for (int i = begin; i < end; i++)
    {

        MysqlConn *conn = new MysqlConn();
        conn->mysqlConnect("192.168.200.129", "root", "12345678", "scott", 3306);
        char str[1024] = {};
        sprintf(str, "insert into dept value(%d,'hello','world')", i);
        conn->update(str);
        delete conn;
    }
}

void insertByPool(ConnPool *mysqlPool, int begin, int end)
{

    for (int i = begin; i < end; i++)
    {

        shared_ptr<MysqlConn> conn = mysqlPool->getConn();
        char str[1024] = {};
        sprintf(str, "insert into dept value(%d,'hello','world')", i);
        conn->update(str);
    }
}

void insertByHand_ThreadPool(void *arg)
{
    struct arg1 *args=(struct arg1 *)arg;
    for (int i = args->start; i < args->end; i++)
    {

        MysqlConn *conn = new MysqlConn();
        conn->mysqlConnect("192.168.200.129", "root", "12345678", "scott", 3306);
        char str[1024] = {};
        sprintf(str, "insert into dept value(%d,'hello','world')", i);
        conn->update(str);
        delete conn;
    }
}

void insertByPool_ThreadPool(void *arg)
{
    struct arg2 *args=(struct arg2 *)arg;
    for (int i = args->start; i < args->end; i++)
    {
        shared_ptr<MysqlConn> conn = args->pool->getConn();
        char str[1024] = {};
        sprintf(str, "insert into dept value(%d,'hello','world')", i);
        conn->update(str);
    }
}

void test01()
{
#if 0 // 手动创建
    //手动创建5000条插入数据花费时间：15297104751纳秒，合计：15秒
    steady_clock::time_point biginTime=steady_clock::now();
    insertByHand(0,5000);
    steady_clock::time_point endTime=steady_clock::now();

    auto length = endTime - biginTime;
    cout<<"手动创建5000条插入数据花费时间："<<length.count()<<"纳秒，合计："<<length.count()/1000000000<<"秒"<<endl;

#else // 数据库连接池
    // 利用数据库连接池创建5000条插入数据花费时间：4385226311纳秒，合计：4秒
    ConnPool *mysqlPool = ConnPool::getConnPool();
    steady_clock::time_point biginTime = steady_clock::now();
    insertByPool(mysqlPool, 0, 5000);
    steady_clock::time_point endTime = steady_clock::now();

    auto length = endTime - biginTime;
    cout << "利用数据库连接池创建5000条插入数据花费时间：" << length.count() << "纳秒，合计：" << length.count() / 1000000000 << "秒" << endl;
    delete mysqlPool;
#endif
}

void test02()
{
#if 0
    // 利用多线程来手动插入数据库
    //使用多线程手动创建5000条插入数据花费时间：10954892637纳秒，合计：10秒
    MysqlConn *conn = new MysqlConn(); //先连接一次数据库，防止因为多线程同时连接数据库导致拒绝连接
    conn->mysqlConnect("192.168.200.129","root","12345678","scott",3306);
    steady_clock::time_point biginTime=steady_clock::now();
    thread thread1(insertByHand,0,1000);
    thread thread2(insertByHand,1000,2000);
    thread thread3(insertByHand,2000,3000);
    thread thread4(insertByHand,3000,4000);
    thread thread5(insertByHand,4000,5000);
    thread1.join();//使用join可以让整个程序等待线程结束后关闭
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    steady_clock::time_point endTime=steady_clock::now();
    auto length = endTime - biginTime;
    cout<<"使用多线程手动创建5000条插入数据花费时间："<<length.count()<<"纳秒，合计："<<length.count()/1000000000<<"秒"<<endl;
    delete conn;

#else
    // 利用多线程和数据库连接池数据库
    // 使用多线程和数据库连接池创建5000条插入数据花费时间：1382335018纳秒，合计：1秒
    ConnPool *pool = ConnPool::getConnPool();
    steady_clock::time_point biginTime = steady_clock::now();
    thread thread1(insertByPool, pool, 0, 1000);
    thread thread2(insertByPool, pool, 1000, 2000);
    thread thread3(insertByPool, pool, 2000, 3000);
    thread thread4(insertByPool, pool, 3000, 4000);
    thread thread5(insertByPool, pool, 4000, 5000);
    thread1.join(); // 使用join可以让整个程序等待线程结束后关闭
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    steady_clock::time_point endTime = steady_clock::now();
    auto length = endTime - biginTime;
    cout << "使用多线程和数据库连接池创建5000条插入数据花费时间：" << length.count() << "纳秒，合计：" << length.count() / 1000000000 << "秒" << endl;
    delete pool;
#endif
}

void test03() // 使用多线程连接池完成test02的内容
{
#if 0
    // 利用多线程池来手动插入数据库
    // 使用多线程池手动创建5000条插入数据花费时间：05074976564纳秒，合计：5秒
    MysqlConn *conn = new MysqlConn(); // 先连接一次数据库，防止因为多线程同时连接数据库导致拒绝连接
    conn->mysqlConnect("192.168.200.129", "root", "12345678", "scott", 3306);
    ThreadPool *threadPool = new ThreadPool(2, 8);
    steady_clock::time_point biginTime = steady_clock::now();
    for (int i = 0; i <= 4000; i = i + 1000)
    {
        Task task;
        task.function = &insertByHand_ThreadPool;
        struct arg1 args;
        args.start=i;
        args.end=i+1000;
        task.arg=&args;
        threadPool->addTask(task);
    }
    sleep(1);
    while (threadPool->getBusyNumber()!=0)
    {
        continue;
    }
    steady_clock::time_point endTime = steady_clock::now();
    auto length = endTime - biginTime;
    cout << "使用多线程手动创建5000条插入数据花费时间：" << length.count() << "纳秒，合计：" << length.count() / 1000000000 << "秒" << endl;
    delete conn;
    delete threadPool;
#else
    // 利用多线程池和数据库连接池数据库
    // 使用多线程池和数据库连接池创建5000条插入数据花费时间：01502678605纳秒，合计：1秒
    ConnPool *pool = ConnPool::getConnPool();
    ThreadPool *threadPool = new ThreadPool(3,8);
    steady_clock::time_point biginTime = steady_clock::now();
    for(int i=0;i<=4000;i=i+1000)
    {
        Task task;
        task.function=&insertByPool_ThreadPool;
        struct arg2 args;
        args.pool=pool;
        args.start=i;
        args.end=i+1000;
        task.arg=&args;

        threadPool->addTask(task);

    }
    sleep(0.5);
    while(threadPool->getBusyNumber()!=0)
    {
        continue;
    }

    steady_clock::time_point endTime = steady_clock::now();
    auto length = endTime - biginTime;
    cout << "使用多线程池和数据库连接池创建5000条插入数据花费时间：" << length.count() << "纳秒，合计：" << length.count() / 1000000000 << "秒" << endl;

#endif
}

int main()
{

    clear_table();

    test03();
    return 1;
}



/*
    单线程+手动插入  15s
    单线程+数据库池  4s
    多线程+手动输入  10s
    多线程+数据池    1s
    线程池+手动输入  5s
    线程池+数据库池  1s
*/