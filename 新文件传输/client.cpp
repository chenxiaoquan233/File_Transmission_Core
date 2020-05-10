#include "client.h"
using namespace std;
void work(char* argv1, char* argv2, char* argv3)
{
	/*初始化*/
	{
		if (!judge_input(argv1, argv2, argv3)) { puts("输入数据有误，请检查输入数据格式"); return; }
		if (!read_fileinfo(argv1)) { puts("文件路径不存在！"); return; }
		if (!ini_socket(argv2, atoi(argv3))) { puts("socket初始化失败！"); return; }
		if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1) { puts("连接失败！请检查server端"); return; }
	}
	
	/*初始化文件信息：压缩文件（暂无），计算出文件的长度并发送*/
	{
		if((filename = zip()).empty()) { puts("压缩失败！"); return; }
		if(!send_filename(filename)){ puts("传递文件信息失败！"); return; }
		if (!send_total_len(total_len)) { puts("传递总文件长度失败！"); return; }
		if ((offset = get_offset()) == -1) { puts("获取文件偏移量失败！"); return; }
		if (!recv_ack()) { puts("获取确认码失败！"); return; }
	}
	/*传输所有文件*/
	send_whole_file();

	/*结束连接*/
	end_transmission();
}
//-----------文件处理------------
bool read_fileinfo(char* argv1)
{
	//需要更新total_len,filename,初始化fileptr
}
string get_nxt_slice(FILE*fileptr,long long offset,int pck_len)
{
	//根据offset在fileptr中读取pck_len长度的文件到string中并返回
	//注意：offset是【已经传输的文件长度】
}
bool judge_input(char* argv1, char* argv2, char* argv3)
{
    //判断argv1-3是否合法
}

//----------------zip----------------
std::string zip()
{
	//返回压缩后的文件名
}

//----------------socket-------------
void send_whole_file()
{

}
bool ini_socket(char* ipaddr, int port)
{

}
bool send_filename(string filename)
{

}
bool send_total_len(long long total_len)
{
	//int pkt_num = ceil((double)(total_len - offset) / MAX_PACKET_LEN);
}
long long get_offset()
{

}
void send_slice(string pkt)
{

}

void end_transmission()
{

}
bool recv_ack()
{

}

