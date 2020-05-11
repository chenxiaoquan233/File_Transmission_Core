// 新文件传输.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "client.h"
int main(int argc, char** argv)
{
    if (argc == 4)
        work(argv[1], argv[2], argv[3]);
}

