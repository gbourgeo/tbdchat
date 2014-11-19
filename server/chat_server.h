#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

/* System Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>
#include <pthread.h>
#include "linked_list.h"

/* Preprocessor Macros */
// Misc constants
#define BACKLOG 2               // how many pending connections the queue will hold
#define BUFFERSIZE 128
#define DEFAULT_ROOM 1000
#define DEFAULT_ROOM_NAME "Lobby"
#define SERVER_NAME "SERVER"
#define USERS_FILE "Users.bin"
// Client options
#define INVALID -1
#define REGISTER 1
#define SETPASS 2
#define SETNAME 3
#define LOGIN 4
#define EXIT 5
#define INVITE 6
#define JOIN 7
#define GETUSERS 8
#define GETALLUSERS 9
#define GETUSER 10
#define LEAVE 11
#define GETMOTD 12
#define GETROOMS 13
// Server responses
#define LOGSUC 100
#define REGSUC 101
#define PASSSUC 102
#define NAMESUC 103
#define JOINSUC 104
#define MOTD 105
#define INVITESUC 106
#define SERV_ERR 107
// Defined color constants
#define NORMAL "\x1B[0m"
#define BLACK "\x1B[30;1m"
#define RED "\x1B[31;1m"
#define GREEN "\x1B[32;1m"
#define YELLOW "\x1B[33;1m"
#define BLUE "\x1B[34;1m"
#define MAGENTA "\x1B[35;1m"
#define CYAN "\x1B[36;1m"
#define WHITE "\x1B[37;1m"

/* Structures */
struct Packet {
   time_t timestamp;
   char buf[BUFFERSIZE];
   char username[64];
   char realname[64];
   int options;
};
typedef struct Packet packet;

struct chatSession {
   char aliases[2][32];
   int clients[2];
   int this_client;
   int running;
   //mutex
};
typedef struct chatSession session;

/* Function Prototypes */
// chat_server.c
int get_server_socket(char *hostname, char *port);
int start_server(int serv_socket, int backlog);
void debugPacket(packet *rx_pkt);
void sigintHandler(int sig_num);
int accept_client(int serv_sock);
// server_clients.c
void sanitizeBuffer(char *buf);
void *client_receive(void *ptr);
int register_user(packet *in_pkt, int fd);
int login(packet *pkt, int fd);
void exit_client(packet *pkt, int fd);
void send_message(packet *pkt, int clientfd);
void sendError(char *error, int clientfd);
void sendMOTD(int fd);
void get_active_users(int fd);
void get_room_users(packet *in_pkt, int fd);
void user_lookup(packet *in_pkt, int fd);
void get_room_list(int fd);
void set_pass(packet *pkt, int fd);
void set_name(packet *pkt, int fd);
void join(packet *pkt, int fd);
void invite(packet *in_pkt, int fd);
void leave(packet *pkt, int fd);

#endif