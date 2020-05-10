#pragma once
#include "stander.h"
using namespace std;
//�ļ����ܳ���
long long int total_len = -1;
long long int offset = -1;
std::string filename;
sockaddr_in serv_addr;
#ifdef _WIN32
SOCKET sock;
#endif
#ifdef __linux__
int sock;
#endif



std::string get_next_slice();


bool send_filename(string filename);
bool send_total_len(long long total_len);
long long get_offset();
void send_slice(string pkt);

void work(char* argv1, char* argv2,char* argv3);
bool judge_input(char* argv1, char* argv2, char* argv3);
bool ini_socket(char* ipaddr, int port);
bool read_fileinfo(char* argv1);
string zip();
void send_whole_file();
void end_transmission();
bool recv_ack();