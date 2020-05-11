#include "server.h"
using namespace std;
void work(char* argv1, char* argv2, char* argv3)
{

	/*初始化*/
	{
		if (!judge_input(argv1, argv2, argv3)) { puts("输入数据有误，请检查输入数据格式"); return; }
		if (!read_folder_info(argv1)) { puts("文件夹路径不存在！"); return; }
		filepath = argv1;
		if (!ini_socket(argv2, atoi(argv3))) { puts("socket初始化失败！"); return; }
		if (!listen_and_accept()) { puts("侦听client端连接失败！"); return; }
	}

	/*计算时间*/
	clock_t start_t, end_t;
	start_t = clock();

	/*获取文件名称（压缩后），发送已经读取的字节数(offset,从log中读取),获得文件总长度,(如果无)创建一个大小==总长度的缓存文件*/
	{
		if ((filename = recv_filename()).empty()) { puts("获取文件名失败！"); return; }
		logname = filename + ".tmp";
		if ((total_len = recv_total_len()) == -1) { puts("获得文件总长失败！"); return; }
		if ((offset = read_logfile(filepath + logname))== -1) { puts("获取log文件偏移量失败"); return; }
		if (!(send_offset(offset))) { puts("发送文件偏移量失败！"); return; }
		if (!(check_local_file(filepath + filename,total_len))) { puts("检测缓存文件失败！"); return; }
		if (!(send_ack())) { puts("发送文件偏移量失败！"); return; }	
	}
	/*获取所有文件,每接到一片就通过偏移量用指针写入(覆盖缓存文件中),同时更新本地log*/
	if (!recv_whole_file()) { end_transmission(); return; }

	if (!unzip(filepath + filename)) { puts("解压失败！"); return; }
	
	delete_logfile(filepath + logname);
	/*结束连接*/
	end_transmission();


	end_t = clock();
	printf("总用时: %f\n", ((double)(end_t - start_t) / CLOCKS_PER_SEC));
}
//-----------文件处理------------
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
bool read_folder_info(char* argv1)
{
	//判断argv1是不是个文件夹
	if (GetFileAttributesA(argv1) & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}
bool check_local_file(string filename, long long filelen)
{
	//和下面配套：检查本地是否有该文件,如果无,调用下面的函数写一个filelen大小的缓存文件,同时初始化fileptr
	if ((fileptr = fopen(filename.c_str(), "rb+")) == nullptr)
	{
		fileptr = fopen(filename.c_str(), "wb");
		bool res = create_new_file(filename, filelen);
		if (!res) return false;
		fclose(fileptr);
		fileptr = fopen(filename.c_str(), "rb+");
	}
	return true;
}
bool create_new_file(string filename, long long filelen)
{
	if (fwrite(0, 1, filelen, fileptr) != filelen)
		return false;
	return true;
}
bool write_file(FILE* fileptr, long long offset, string data)
{
	//在文件中根据偏移量写入data文件（覆盖写入）
	fsetpos(fileptr, static_cast<fpos_t*>(&offset));
	if (fwrite(data.c_str(), 1, data.length(), fileptr) != data.length())
		return false;
	return true;
}
long long read_logfile(string logname)
{
	//和下面配套:读取/写入log文件,传递offset信息
	//当文件存在时,初始化logptr并且返回文件内值即可，当文件不存在时,创建新log文件,初始化logptr并且写入0,返回0
	logptr = fopen(logname.c_str(), "rb");
	long long offset = 0;
	if (fread(&offset, sizeof(long long), 1, logptr) == 1)
	{
		fclose(logptr);
		return offset;
	}
	else
	{
		fclose(logptr);
		logptr = fopen(logname.c_str(), "wb");
		offset = 0;
		fwrite(&offset, sizeof(long long), 1, logptr);
		fclose(logptr);
		return 0;
	}
}
bool write_logfile(FILE* logptr, long long offset)
{
	int res = fwrite(&offset, sizeof(long long), 1, logptr);
	return res == 1;
}
bool delete_logfile(string logpath)
{
	//删除log文件
	if (_access(logpath.c_str(), 0) != 0)
		return false;

	delete(logpath.c_str());
	return true;
}


//----------------zip----------------
bool unzip(string filepath,string filename)
{
	//解压，然后把原来的压缩文件扬了
	return 1;
}
//---------------socket--------------
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
	if (bind(sock, (struct sockaddr*) & serv_addr, sizeof(sockaddr)) == -1){return false;}
	return 1;
}
bool listen_and_accept()
{
	listen(sock, 5);
	while (1)
	{
		struct sockaddr_in addr;
		int len = sizeof(SOCKADDR);
		//接受客户端连接
		if ((client_sock = accept(sock, (struct sockaddr*) & addr, &len)) != -1)break;
	}
	return 1;
}
string recv_filename()
{
	string ans;
	char buf[MAX_PACKET_LEN + 5];
	memset(buf, 0, sizeof(buf));
	if (recv(client_sock, buf, MAX_PACKET_LEN, 0) == -1)return ans;
	ans = buf;
	return ans;
}
long long recv_total_len()
{
	long long ans = 0;
	char buf[MAX_PACKET_LEN + 5];
	memset(buf, 0, sizeof(buf));
	if (recv(client_sock, buf, MAX_PACKET_LEN, 0) == -1)return -1;
	for (int i = 0; buf[i] != '\0'; i++) { ans *= 10; ans += (int)buf[i] - '0'; }
	return ans;
}
bool send_offset(long long offs)
{
	string buf = to_string(offs);
	return send(client_sock, buf.c_str(), sizeof(buf), 0) != -1;
}
bool send_ack()
{
	string buf = to_string(1);
	return send(client_sock, buf.c_str(), sizeof(buf), 0) != -1;
}
bool recv_whole_file()
{
	puts("开始传输");
	while (offset < total_len)
	{
		printf("%lld KB / %lld B , %.2lf \n", offset, total_len, (double)offset * 100 / total_len);
		string buf = recv_slice();
		if (!write_file(fileptr,offset,buf)) { puts("文件写入失败!"); return 0; }
		offset += buf.size();
		if (!write_logfile(logptr, offset)) { puts("log文件写入失败!"); return 0; }
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
string recv_slice()
{
	string ans;
	char buf[MAX_PACKET_LEN + 5];
	memset(buf, 0, sizeof(buf));
	if (recv(client_sock, buf, MAX_PACKET_LEN, 0) == -1)return ans;
	ans = buf;
	return ans;
}
void end_transmission()
{
	closesocket(sock);
	closesocket(client_sock);
	fclose(fileptr);
	fclose(logptr);
}



