#include "client.h"
using namespace std;
void work(char* argv1, char* argv2, char* argv3)
{
	/*初始化*/
	{
		if (!judge_input(argv1, argv2, argv3)) { puts("输入数据有误，请检查输入数据格式"); return; }
		filepath = argv1;
		if (!ini_socket(argv2, atoi(argv3))) { puts("socket初始化失败！"); return; }
		if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1) { puts("连接失败！请检查server端"); return; }
		if ((filename = zip(argv1)).empty()) { puts("压缩失败！"); return; }
		if (!read_fileinfo(filepath + filename)) { puts("文件路径不存在！"); return; }
	}
	
	/*初始化文件信息：发送文件的长度,名字,获取偏移量和确认码*/
	{
		if(!send_filename(filename)){ puts("传递文件信息失败！"); return; }
		if (!send_total_len(total_len)) { puts("传递总文件长度失败！"); return; }
		if ((offset = get_offset()) == -1) { puts("获取文件偏移量失败！"); return; }
		if (!recv_ack()) { puts("获取确认码失败！"); return; }
	}
	/*传输所有文件*/
	if (!send_whole_file()) { end_transmission(); return; }

	/*结束连接*/
	end_transmission();
}
//-----------文件处理------------
bool read_fileinfo(string filename)
{
	//读入的已经是压缩之后的文件【名】,文件路径请调用argv1

	//需要更新total_len文件总长,初始化fileptr指针,之后为文件操作均由fileptr完成
	if ((fileptr = fopen(filename.c_str(), "rb")) == nullptr)
		return false;

	/*struct __stat64 st;
	if (__stat64(filename.c_str(), &st) != 0)
		return false;
	total_len = st.st_size;*/
	fseek(fileptr, 0, SEEK_END);
	fpos_t pos;
	if (fgetpos(fileptr, &pos) != 0)
		return false;
	total_len = static_cast<long long int>(pos);

	return true;
}
string get_slice(FILE* fileptr, long long offset, int pck_len)
{
	//根据offset在fileptr中读取pck_len长度的文件到string中并返回
	//注意：offset是【已经传输的文件长度】

	fsetpos(fileptr, (fpos_t*)&offset);

	char* buffer = new char[pck_len];
	fread(buffer, 1, pck_len, fileptr);

	return string(buffer);
}
bool judge_input(char* argv1, char* argv2, char* argv3)
{
	//判断argv1-3是否合法

	//argv1
	if ((_access(argv1, 0)) != -1)
		if ((_access(argv1, 6)) == -1)
			return false;

	//argv2
	int ip[4];
	int res = sscanf(argv2, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	if (res != 4 || ip[0] > 255 || ip[1] > 255 || ip[2] > 255 || ip[3] > 255)
		return false;

	//argv3
	int port;
	res = sscanf(argv3, "%d", &port);
	if (res != 1 || port > 65535)
		return false;

	return true;
}


//----------------zip----------------
std::string zip(char* argv1)
{
	string sub; return sub;
	//返回压缩后的文件【名】,非文件路径,换句话说压缩后文件的绝对路径为argv1 + 返回值
}

//----------------socket-------------
bool ini_socket(char* ipaddr, int port)
{
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
#ifdef __linux__
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ipaddr);
	serv_addr.sin_port = htons(port);
	return 1;
}
bool send_filename(string filename)
{
	return send(sock, filename.c_str(), sizeof(filename), 0)!=-1;
}
bool send_total_len(long long total_len)
{
	string buf = to_string(total_len);
	return send(sock, buf.c_str(), sizeof(buf), 0) != -1;
	//int pkt_num = ceil((double)(total_len - offset) / MAX_PACKET_LEN);
}
long long get_offset()
{
	long long ans = 0;
	char buf[MAX_PACKET_LEN + 5];
	memset(buf, 0, sizeof(buf));
	if(recv(sock, buf, MAX_PACKET_LEN, 0)==-1)return -1;
	for (int i = 0; buf[i] != '\0'; i++) { ans *= 10; ans += (int)buf[i] - '0'; }
	return ans;
}
bool recv_ack()
{
	char buf[1];
	if (recv(sock, buf, 1, 0) == -1)return 0;
	return buf[0] == '1';
}
bool send_whole_file()
{
	puts("开始传输");
	while (offset < total_len)
	{
		printf("%lld KB / %lld B , %.2lf \n",offset,total_len,(double)offset * 100 / total_len);
		string buf = get_slice(fileptr, offset, MAX_PACKET_LEN);
		offset += buf.size();
		if (!send_slice(buf)) { puts("文件传输失败!"); return 0; }
		/*清除当前传输信息*/
        #ifdef _WIN32
		printf("\r");
        #endif
        #ifdef __linux__
		printf("\r\033[k");
        #endif
	}
	printf("%lld KB / %lld B , %.2lf %\n", offset, total_len, (double)offset * 100 / total_len);
	return 1;
}
bool send_slice(string pkt)
{
	return send(sock, pkt.c_str(), sizeof(pkt), 0) != -1;
}

void end_transmission()
{
	closesocket(sock);
	fclose(fileptr);
}


