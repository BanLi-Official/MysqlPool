#include <iostream>
#include <string>
#include <mysql/mysql.h>   //在usr/include中
#include <chrono> //时间库

using namespace std;
using namespace chrono;

class MysqlConn
{
public:
    //创建一个数据库连接池
    MysqlConn();
    //释放连接池的内存
    ~MysqlConn();
    //连接数据库
    bool mysqlConnect(string ip ,  string user , string psw , string dbName,unsigned short port = 3306 );
    //更新数据库 增、删、改
    bool update(string sql);
    //查询数据库
    bool query(string sql);
    //遍历查询得到的数据集
    bool next();
    //获取字段值
    string getValue(int index);
    //事务操作
    bool transAction();
    //事务提交
    bool commit();
    //事务回滚
    bool rollBack();
    //刷新时间戳（建立链接或者手动刷新后更新）
    void setStartTime();
    //计算时间差
    long long getAliveTime();
    //获取行数
    int getRow();
    //获取列数
    int getColumn();
    //遍历整个表
    void showTable(string table);
private:
    void freeResult();
    MYSQL* conn=nullptr;
    MYSQL_RES *sqlRes=nullptr;
    MYSQL_ROW row=nullptr;
    steady_clock::time_point StartTime; //刷新后的时间戳

};