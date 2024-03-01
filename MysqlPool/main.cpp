#include "MysqlConn.hpp"
#include <iostream>

using namespace std;

bool query_table()
{
    MysqlConn * conn= new MysqlConn();
    bool res = conn->mysqlConnect("192.168.200.129","root","12345678","scott",3306);
    if(!res)
    {
        cout<<"函数错误，返回！"<<endl;
        return false;
    }

    res = conn->update("insert into dept value(2,'hello','world')");
    if(!res)
    {
        cout<<"数据库更新错误，返回！"<<endl;
        return false;
    }

    conn->showTable("dept");

    return true;

}

int main()
{
    query_table();
    return 1;
}