/*
    打印不同等级的日志
*/
#pragma once
#include <iostream>
#include <string>
#include <ctime>

#define NORMAL 1
#define WARNING 2
#define DEBUG 3
#define ERROR 4
#define FATAL 5

// #LEVEL 将LEVEL的内容原生显示，不替换为对应数据
#define LOG(LEVEL, MESSAGE) Log(#LEVEL, MESSAGE, __FILE__, __LINE__)

void Log(const std::string& level, const std::string& message, std::string file, int line) {
    std::cout << "[" << level << "]" << "[" << time(nullptr) << "]" << "[" << message << "]" << "[" << file << " : " << line << "]" << std::endl;
} 