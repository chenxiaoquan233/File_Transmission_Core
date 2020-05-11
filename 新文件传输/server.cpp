#include "server.h"
using namespace std;
void work(char* argv1, char* argv2, char* argv3)
{

	/*��ʼ��*/
	{
		if (!judge_input(argv1, argv2, argv3)) { puts("�����������������������ݸ�ʽ"); return; }
		if (!read_folder_info(argv1)) { puts("�ļ���·�������ڣ�"); return; }
		filepath = argv1;
		if (!ini_socket(argv2, atoi(argv3))) { puts("socket��ʼ��ʧ�ܣ�"); return; }
		if (!listen_and_accept()) { puts("����client������ʧ�ܣ�"); return; }
	}

	/*����ʱ��*/
	clock_t start_t, end_t;
	start_t = clock();

	/*��ȡ�ļ����ƣ�ѹ���󣩣������Ѿ���ȡ���ֽ���(offset,��log�ж�ȡ),����ļ��ܳ���,(�����)����һ����С==�ܳ��ȵĻ����ļ�*/
	{
		if ((filename = recv_filename()).empty()) { puts("��ȡ�ļ���ʧ�ܣ�"); return; }
		logname = filename + ".tmp";
		if ((total_len = recv_total_len()) == -1) { puts("����ļ��ܳ�ʧ�ܣ�"); return; }
		if ((offset = read_logfile(filepath + logname))== -1) { puts("��ȡlog�ļ�ƫ����ʧ��"); return; }
		if (!(send_offset(offset))) { puts("�����ļ�ƫ����ʧ�ܣ�"); return; }
		if (!(check_local_file(filepath + filename,total_len))) { puts("��⻺���ļ�ʧ�ܣ�"); return; }
		if (!(send_ack())) { puts("�����ļ�ƫ����ʧ�ܣ�"); return; }	
	}
	/*��ȡ�����ļ�,ÿ�ӵ�һƬ��ͨ��ƫ������ָ��д��(���ǻ����ļ���),ͬʱ���±���log*/
	if (!recv_whole_file()) { end_transmission(); return; }

	if (!unzip(filepath + filename)) { puts("��ѹʧ�ܣ�"); return; }
	
	delete_logfile(filepath + logname);
	/*��������*/
	end_transmission();


	end_t = clock();
	printf("����ʱ: %f\n", ((double)(end_t - start_t) / CLOCKS_PER_SEC));
}
//-----------�ļ�����------------
bool judge_input(char* argv1, char* argv2, char* argv3)
{
	//�ж�argv1-3�Ƿ�Ϸ�

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
	//�ж�argv1�ǲ��Ǹ��ļ���
	if (GetFileAttributesA(argv1) & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}
bool check_local_file(string filename, long long filelen)
{
	//���������ף���鱾���Ƿ��и��ļ�,�����,��������ĺ���дһ��filelen��С�Ļ����ļ�,ͬʱ��ʼ��fileptr
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
	//���ļ��и���ƫ����д��data�ļ�������д�룩
	fsetpos(fileptr, static_cast<fpos_t*>(&offset));
	if (fwrite(data.c_str(), 1, data.length(), fileptr) != data.length())
		return false;
	return true;
}
long long read_logfile(string logname)
{
	//����������:��ȡ/д��log�ļ�,����offset��Ϣ
	//���ļ�����ʱ,��ʼ��logptr���ҷ����ļ���ֵ���ɣ����ļ�������ʱ,������log�ļ�,��ʼ��logptr����д��0,����0
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
	//ɾ��log�ļ�
	if (_access(logpath.c_str(), 0) != 0)
		return false;

	delete(logpath.c_str());
	return true;
}


//----------------zip----------------
bool unzip(string filepath,string filename)
{
	//��ѹ��Ȼ���ԭ����ѹ���ļ�����
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
		//���ܿͻ�������
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
	puts("��ʼ����");
	while (offset < total_len)
	{
		printf("%lld KB / %lld B , %.2lf \n", offset, total_len, (double)offset * 100 / total_len);
		string buf = recv_slice();
		if (!write_file(fileptr,offset,buf)) { puts("�ļ�д��ʧ��!"); return 0; }
		offset += buf.size();
		if (!write_logfile(logptr, offset)) { puts("log�ļ�д��ʧ��!"); return 0; }
		/*�����ǰ������Ϣ*/
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



