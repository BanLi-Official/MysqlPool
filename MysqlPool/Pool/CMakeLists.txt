cmake_minimum_required(VERSION 3.28.2)
project(MysqlPool)

link_directories(/usr/lib/x86_64-linux-gnu)   #mysql所需的头文件
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(lib mysqlclient jsoncpp)    #mysql所需的库文件


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headFile)   #数据库连接池的头文件
include_directories(/usr/include)   #数据库的头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/sourceFile source)  #数据库连接池的源文件

set(LIBRARY_OUTPUT_PATH ${outputLib} )

add_library(MysqlPoolLib STATIC ${source})
target_link_libraries(MysqlPoolLib ${lib})          #连接动态库