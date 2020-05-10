#pragma once
#include "stander.h"
using namespace std;
sockaddr_in serv_addr;
sockaddr_in client_serv_addr;
#ifdef _WIN32
SOCKET sock,client_sock;
#endif
#ifdef __linux__
int sock,client_sock;
#endif

//文件的总长度
long long int total_len = -1;
//已传长度(从log中读取)
long long int offset = -1;
std::string filename;

void work(char* argv1, char* argv2, char* argv3);

void recv_whole_file();
bool read_folder_info(char* argv1);
bool ini_socket(char* ipaddr, int port);
bool listen_and_accept();
string recv_filename();
long long recv_total_len();
bool create_new_file(string filename, long long filelen);
bool check_local_file(string filename, string logname, long long filelen);
bool write_file(string filename, long long offset, string data);
long long read_logfile(string filename);
bool write_logfile(string filename, long long offset);
bool unzip();
bool delete_logfile(string filename);
void end_transmission();
bool send_offset();