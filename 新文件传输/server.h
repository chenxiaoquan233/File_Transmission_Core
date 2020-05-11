#pragma once
#include "stander.h"
using namespace std;
sockaddr_in serv_addr;
sockaddr_in client_serv_addr;
FILE* fileptr=NULL,*logptr=NULL;
#ifdef _WIN32
SOCKET sock,client_sock;
#endif
#ifdef __linux__
int sock,client_sock;
#endif

//�ļ����ܳ���
long long int total_len = -1;
//�Ѵ�����(��log�ж�ȡ)
long long int offset = -1;
string filepath;
string filename;
string logname;

void work(char* argv1, char* argv2, char* argv3);

bool judge_input(char* argv1, char* argv2, char* argv3);
bool recv_whole_file();
bool read_folder_info(char* argv1);
bool ini_socket(char* ipaddr, int port);
bool listen_and_accept();
string recv_filename();
string recv_slice();
long long recv_total_len();
bool create_new_file(string filename, long long filelen);
bool check_local_file(string filename, long long filelen);
bool write_file(FILE* fileptr, long long offset, string data);
long long read_logfile(string filename);
bool write_logfile(FILE* logptr, long long offset);
bool unzip(string filename);
bool delete_logfile(string logpath);
void end_transmission();
bool send_offset(long long offs);
bool send_ack();