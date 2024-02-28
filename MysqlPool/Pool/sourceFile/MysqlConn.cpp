// #include "../headFile/MysqlConn.hpp"
#include <iostream>
#include "MysqlConn.hpp"

using namespace std;

MysqlConn::MysqlConn()
{
    conn = mysql_init(nullptr); // 初始化空间
    if (conn == nullptr)
    {
        cout << "数据库连接空间初始化失败！" << endl;
        return;
    }
    mysql_set_character_set(conn, "utf8"); // 设置为utf-8

    return;
}

MysqlConn::~MysqlConn()
{
    if (conn != nullptr)
    {
        mysql_close(conn);
    }
    freeResult();
}

bool MysqlConn::mysqlConnect(string ip, string user, string psw, string dbName, unsigned short port)
{
    /*
    mysql_real_connect返回值:
        成功: 返回MYSQL*连接句柄, 对于成功的连接，返回值与第1个参数的值相同。返回值指向的内存和第一个参数指针指向的内存一样
        失败，返回NULL。
        句柄: 是windows中的一个概念, 句柄可以理解为一个实例(或者对象)
        参数中需要放入的是char数组地址，所以使用c_str()转化一下
    */
    MYSQL *ptr = mysql_real_connect(conn, ip.c_str(), user.c_str(), psw.c_str(), dbName.c_str(), port, NULL, 0);
    if (ptr == nullptr)
    {
        cout << "数据库连接失败！" << endl;
        return false;
    }

    return true;
}

bool MysqlConn::update(string sql)
{
    int res = mysql_query(conn, sql.c_str()); // 增删查改都可以用，成功返回0，失败返回-1
    if (res != 0)
    {
        cout << "数据库更新失败！" << endl;
        return false;
    }
    return true;
}

bool MysqlConn::query(string sql)
{
    freeResult();
    int res = mysql_query(conn, sql.c_str());
    if (res != 0)
    {
        cout << "数据库查找失败！" << endl;
        return false;
    }
    //将结果保存到客户端
    /*
        将结果集从 mysql(参数) 对象中取出
        MYSQL_RES 对应一块内存, 里边保存着这个查询之后得到的结果集
        如何将行和列的数据从结果集中取出, 需要使用其他函数
        返回值: 具有多个结果的MYSQL_RES结果集合。如果出现错误，返回NULL。
    */
    sqlRes=mysql_store_result(conn);
    if(sqlRes == NULL)
    {
        cout << "数据库结果保存失败！" << endl;
        return false;
    }
    return true;
}

bool MysqlConn::next()
{
    if (sqlRes == nullptr)
    {
        cout<<"结果集为空，无法获取next"<<endl;
        return false;
    }
    
    /*
        mysql_fetch_row
        参数: 
            - result: 通过查询得到的结果集
        返回值: 
            - 成功: 得到了当前记录中每个字段的值
            - 失败: NULL, 说明数据已经读完了
    */
    row=mysql_fetch_row(sqlRes);
    if (row == nullptr)
    {
        cout << "数据库获取next结果行失败！" << endl;
        return false;
    }
    
    return true;
}

string MysqlConn::getValue(int index)
{
    if (row == nullptr)
    {
        cout << "当前行数据为空！" << endl;
        return nullptr;
    }
    //获取结果集的列数，判断index是否合理
    int rowCount=mysql_num_fields(sqlRes);
    if(index<=0 || index>rowCount)
    {
        cout << "当前列index不合理！" << endl;
        return nullptr;
    }
    //获取当前行某一列的内容
    char * str=row[index];

    //利用 string(val, length)将其string化。
    /*
        获取指定列的长度
        unsigned long *mysql_fetch_lengths(MYSQL_RES *result);
        参数: 
            - result: 通过查询得到的结果集
        返回值:
            - 无符号长整数的数组表示各列的大小。如果出现错误，返回NULL。
    */
    unsigned long length=mysql_fetch_lengths(sqlRes)[index];
    return string(str,length);

}

bool MysqlConn::transAction()
{
    //禁用自动提交
    return mysql_autocommit(conn,false);
}

bool MysqlConn::commit()
{
    return mysql_commit(conn);
}

bool MysqlConn::rollBack()
{
    return mysql_rollback(conn);
}

void MysqlConn::freeResult()
{
    if(sqlRes != nullptr)
    {
        mysql_free_result(sqlRes);
        sqlRes = nullptr;
    }
    return ;
}
