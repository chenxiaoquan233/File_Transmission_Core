#include "server.h"
using namespace std;
void work(char* argv1, char* argv2, char* argv3)
{

	/*��ʼ��*/
	{
		if (!judge_input(argv1, argv2, argv3)) { puts("�����������������������ݸ�ʽ"); return; }
		if (!read_folder_info(argv1)) { puts("�ļ���·�������ڣ�"); return; }
		if (!ini_socket(argv2, atoi(argv3))) { puts("socket��ʼ��ʧ�ܣ�"); return; }
		if (!listen_and_accept()) { puts("����client������ʧ�ܣ�"); return; }
	}

	/*����ʱ��*/
	clock_t start_t, end_t;
	start_t = clock();

	/*��ȡ�ļ����ƣ�ѹ���󣩣������Ѿ���ȡ���ֽ���(offset,��log�ж�ȡ),����ļ��ܳ���,(�����)����һ����С==�ܳ��ȵĻ����ļ�*/
	{
		if ((filename = recv_filename()).empty()) { puts("��ȡ�ļ���ʧ�ܣ�"); return; }
		if ((total_len = recv_total_len()) == -1) { puts("����ļ��ܳ�ʧ�ܣ�"); return; }
		if ((offset = read_logfile())== -1) { puts("��ȡlog�ļ�ƫ����ʧ��"); return; }
		if (!(send_offset())) { puts("�����ļ�ƫ����ʧ�ܣ�"); return; }
		
	}
	/*��ȡ�����ļ�,ÿ�ӵ�һƬ��ͨ��ƫ������ָ��д��(���ǻ����ļ���),ͬʱ���±���log*/
	recv_whole_file();

	if (!unzip()) { puts("��ѹʧ�ܣ�"); return; }
	
	/*��������*/
	end_transmission();


	end_t = clock();
	printf("����ʱ: %f\n", ((double)(end_t - start_t) / CLOCKS_PER_SEC));
}
//-----------�ļ�����------------
bool judge_input(char* argv1, char* argv2, char* argv3)
{
	//�ж�argv1-3�Ƿ�Ϸ�
}
bool read_folder_info(char* argv1)
{
	//�ж�argv1�ǲ��Ǹ��ļ���
}
bool check_local_file(string filename,string logname, long long filelen)
{
	//���������ף���鱾���Ƿ��и��ļ�,�����,��������ĺ���дһ��filelen��С�Ļ����ļ�
}
bool create_new_file(string filename, long long filelen)
{

}
bool write_file(string filename,long long offset,string data)
{
	//���ļ��и���ƫ����д��data�ļ�������д�룩
}
long long read_logfile(string filename)
{
	//����������:��ȡ/д��log�ļ�,����offset��Ϣ
}
bool write_logfile(string filename,long long offset)
{

}
bool delete_logfile(string filename)
{
	//ɾ��log�ļ�
}

//----------------zip----------------
bool unzip(string filename)
{
	//��ѹ��Ȼ���ԭ����ѹ���ļ�����
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