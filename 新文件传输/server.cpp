#include "server.h"
using namespace std;
void work(char* argv1, char* argv2, char* argv3)
{

	/*初始化*/
	{
		if (!judge_input(argv1, argv2, argv3)) { puts("输入数据有误，请检查输入数据格式"); return; }
		if (!read_folder_info(argv1)) { puts("文件夹路径不存在！"); return; }
		if (!ini_socket(argv2, atoi(argv3))) { puts("socket初始化失败！"); return; }
		if (!listen_and_accept()) { puts("侦听client端连接失败！"); return; }
	}

	/*计算时间*/
	clock_t start_t, end_t;
	start_t = clock();

	/*获取文件名称（压缩后），发送已经读取的字节数(offset,从log中读取),获得文件总长度,(如果无)创建一个大小==总长度的缓存文件*/
	{
		if ((filename = recv_filename()).empty()) { puts("获取文件名失败！"); return; }
		if ((total_len = recv_total_len()) == -1) { puts("获得文件总长失败！"); return; }
		if ((offset = read_logfile())== -1) { puts("获取log文件偏移量失败"); return; }
		if (!(send_offset())) { puts("发送文件偏移量失败！"); return; }
		
	}
	/*获取所有文件,每接到一片就通过偏移量用指针写入(覆盖缓存文件中),同时更新本地log*/
	recv_whole_file();

	if (!unzip()) { puts("解压失败！"); return; }
	
	/*结束连接*/
	end_transmission();


	end_t = clock();
	printf("总用时: %f\n", ((double)(end_t - start_t) / CLOCKS_PER_SEC));
}
//-----------文件处理------------
bool judge_input(char* argv1, char* argv2, char* argv3)
{
	//判断argv1-3是否合法
}
bool read_folder_info(char* argv1)
{
	//判断argv1是不是个文件夹
}
bool check_local_file(string filename,string logname, long long filelen)
{
	//和下面配套：检查本地是否有该文件,如果无,调用下面的函数写一个filelen大小的缓存文件
}
bool create_new_file(string filename, long long filelen)
{

}
bool write_file(string filename,long long offset,string data)
{
	//在文件中根据偏移量写入data文件（覆盖写入）
}
long long read_logfile(string filename)
{
	//和下面配套:读取/写入log文件,传递offset信息
}
bool write_logfile(string filename,long long offset)
{

}
bool delete_logfile(string filename)
{
	//删除log文件
}

//----------------zip----------------
bool unzip(string filename)
{
	//解压，然后把原来的压缩文件扬了
}
//---------------socket--------------
void recv_whole_file()
{

}

bool ini_socket(char* ipaddr, int port)
{

}
bool listen_and_accept()
{

}
std::string recv_filename()
{

}
int recv_total_packnum()
{

}


void end_transmission()
{

}

long long recv_total_len()
{

}