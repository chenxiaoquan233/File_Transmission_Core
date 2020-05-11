#include "client.h"
using namespace std;
void work(char* argv1, char* argv2, char* argv3)
{
	/*��ʼ��*/
	{
		if (!judge_input(argv1, argv2, argv3)) { puts("�����������������������ݸ�ʽ"); return; }
		filepath = argv1;
		if (!ini_socket(argv2, atoi(argv3))) { puts("socket��ʼ��ʧ�ܣ�"); return; }
		if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1) { puts("����ʧ�ܣ�����server��"); return; }
		if ((filename = zip(argv1)).empty()) { puts("ѹ��ʧ�ܣ�"); return; }
		if (!read_fileinfo(filepath + filename)) { puts("�ļ�·�������ڣ�"); return; }
	}
	
	/*��ʼ���ļ���Ϣ�������ļ��ĳ���,����,��ȡƫ������ȷ����*/
	{
		if(!send_filename(filename)){ puts("�����ļ���Ϣʧ�ܣ�"); return; }
		if (!send_total_len(total_len)) { puts("�������ļ�����ʧ�ܣ�"); return; }
		if ((offset = get_offset()) == -1) { puts("��ȡ�ļ�ƫ����ʧ�ܣ�"); return; }
		if (!recv_ack()) { puts("��ȡȷ����ʧ�ܣ�"); return; }
	}
	/*���������ļ�*/
	if (!send_whole_file()) { end_transmission(); return; }

	/*��������*/
	end_transmission();
}
//-----------�ļ�����------------
bool read_fileinfo(string filename)
{
	//������Ѿ���ѹ��֮����ļ�������,�ļ�·�������argv1

	//��Ҫ����total_len�ļ��ܳ�,��ʼ��fileptrָ��,֮��Ϊ�ļ���������fileptr���
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
	//����offset��fileptr�ж�ȡpck_len���ȵ��ļ���string�в�����
	//ע�⣺offset�ǡ��Ѿ�������ļ����ȡ�

	fsetpos(fileptr, (fpos_t*)&offset);

	char* buffer = new char[pck_len];
	fread(buffer, 1, pck_len, fileptr);

	return string(buffer);
}
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


//----------------zip----------------
std::string zip(char* argv1)
{
	string sub; return sub;
	//����ѹ������ļ�������,���ļ�·��,���仰˵ѹ�����ļ��ľ���·��Ϊargv1 + ����ֵ
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
	puts("��ʼ����");
	while (offset < total_len)
	{
		printf("%lld KB / %lld B , %.2lf \n",offset,total_len,(double)offset * 100 / total_len);
		string buf = get_slice(fileptr, offset, MAX_PACKET_LEN);
		offset += buf.size();
		if (!send_slice(buf)) { puts("�ļ�����ʧ��!"); return 0; }
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
bool send_slice(string pkt)
{
	return send(sock, pkt.c_str(), sizeof(pkt), 0) != -1;
}

void end_transmission()
{
	closesocket(sock);
	fclose(fileptr);
}


