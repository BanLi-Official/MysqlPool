cmake_minimum_required(VERSION 3.28.2)
project(MysqlPoolTest)

set(outputLib ${CMAKE_CURRENT_SOURCE_DIR}/lib)   #设置数据库连接池的库的输出位置
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/output)


add_subdirectory(Pool) #生成数据库连接池的静态库
add_subdirectory(ThreadPool) #生成多线程连接池的静态库



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Pool/headFile)  #指定数据库连接池的头文件库
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ThreadPool)
link_directories(${outputLib})                                  #指定数据库连接池静态库的位置
link_libraries(MysqlPoolLib)                                    #指定数据库连接池
link_libraries(MyThreadPoolLib)                                 #指定多线程连接池
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} source)        #查找源文件


add_executable(main ${source})
