#include "mysql_pool.h"
#include <iostream>

int main(){
    std::cout << sylar::Mysql_pool::GetInstance()->GetFreeConn() << std::endl;
    sylar::Mysql_pool::GetInstance()->Insert((char*)"insert into users(id, name, password) values(2, 'liwentao', '666')");
    std::cout << sylar::Mysql_pool::GetInstance()->GetFreeConn() << std::endl;
    return 0;
}