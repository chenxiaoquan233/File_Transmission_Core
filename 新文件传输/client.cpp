#include "client.h"
using namespace std;
void work(char* argv1, char* argv2, char* argv3)
{
	/*��ʼ��*/
	{
		if (!judge_input(argv1, argv2, argv3)) { puts("�����������������������ݸ�ʽ"); return; }
		if (!ini_socket(argv2, atoi(argv3))) { puts("socket��ʼ��ʧ�ܣ�"); return; }
		if (connect(sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1) { puts("����ʧ�ܣ�����server��"); return; }
		if ((filename = zip(argv1)).empty()) { puts("ѹ��ʧ�ܣ�"); return; }
		if (!read_fileinfo(filename)) { puts("�ļ�·�������ڣ�"); return; }
	}
	
	/*��ʼ���ļ���Ϣ�������ļ��ĳ���,����,��ȡƫ������ȷ����*/
	{
		if(!send_filename(filename)){ puts("�����ļ���Ϣʧ�ܣ�"); return; }
		if (!send_total_len(total_len)) { puts("�������ļ�����ʧ�ܣ�"); return; }
		if ((offset = get_offset()) == -1) { puts("��ȡ�ļ�ƫ����ʧ�ܣ�"); return; }
		if (!recv_ack()) { puts("��ȡȷ����ʧ�ܣ�"); return; }
	}
	/*���������ļ�*/
	send_whole_file();

	/*��������*/
	end_transmission();
}
//-----------�ļ�����------------
bool read_fileinfo(string filename)
{
	//������Ѿ���ѹ��֮����ļ�������,�ļ�·�������argv1

	//��Ҫ����total_len�ļ��ܳ�,��ʼ��fileptrָ��,֮��Ϊ�ļ���������fileptr���
}
string get_slice(FILE*fileptr,long long offset,int pck_len)
{
	//����offset��fileptr�ж�ȡpck_len���ȵ��ļ���string�в�����
	//ע�⣺offset�ǡ��Ѿ�������ļ����ȡ�
}
bool judge_input(char* argv1, char* argv2, char* argv3)
{
    //�ж�argv1-3�Ƿ�Ϸ�
}

//----------------zip----------------
std::string zip(char* argv1)
{
	//����ѹ������ļ�������,���ļ�·��,���仰˵ѹ�����ļ��ľ���·��Ϊargv1 + ����ֵ
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

