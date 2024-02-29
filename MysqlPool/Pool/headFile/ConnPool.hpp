#include "MysqlConn.hpp"
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

//使用单例模式实现数据库连接池
class ConnPool
{
public:
    static ConnPool * getConnPoll();  //通过一个专门的函数来统一获取实例
    //将复制构造函数给删除
    ConnPool(const ConnPool& obj) = delete; 
    //将重构的等于号给删除
    ConnPool & operator =(const ConnPool& obj) = delete;
private:
    ConnPool(); //将构造函数放入私有，防止被私自创建实例
    bool parseConfigJson();//解析数据库连接池的json配置文件
    bool createConn(); //创建一条连接
    void produceConn(); //创建连接
    void recycleConn(); //回收连接
    MysqlConn * getConn();//获取一条数据库连接

    //数据库连接的信息
    string ip;
    string user;
    string psw;
    string dbName;
    unsigned int port;
    //数据库连接池的属性
    int max;
    int min;
    int timeout;
    int maxldleTime;
    //数据库的连接队列与锁
    queue<MysqlConn *> ConnQ ;
    mutex mutexQ;
    condition_variable m_cond;


};