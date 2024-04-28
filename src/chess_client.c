/*****************************************************************
                          Header file
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "client_opengl.h"

/*****************************************************************
                 CLIENT initialization parameters
******************************************************************/
#define PORT 4444
int sock = 0;

/*****************************************************************
                         Protocol parameter
******************************************************************/
#define USERNAME_TYPE 1
#define USERNAMELIST_TYPE 2
#define COORDINATE_TYPE 3
#define SIZE_TYPE 4
#define GRID_TYPE 5

typedef struct {
    int type; // 消息类型
    int length; // 消息体长度，单位为字节
} MessageHeader;

/*****************************************************************
                           Global variable
******************************************************************/
extern int size;
extern int **grid;

/************************************************************************
                  Function prototype declaration
*************************************************************************/
void* receive_msg(void* sock) {
    int server_sock = *(int*)sock;

    while(1) {
        MessageHeader header;
        int valread = recv(server_sock,&header,sizeof(header), 0);
        if(valread > 0) {   
            printf("Message from server\n");
        } else {
            printf("Connection closed by server.\n");
            break;
        }
        switch (header.type)
        {
        case USERNAMELIST_TYPE:
        {   
            printf("recv usernamelist\n");
            char **userlist = malloc(header.length);
            recv(server_sock,userlist,header.length,0);
            break;
        }
        case GRID_TYPE:
        {   
            // 接收数据到临时缓冲区
            int *tempBuffer = malloc(header.length);
            if (tempBuffer == NULL) {
                // 错误处理: 内存分配失败
                break;
            }
            ssize_t bytesReceived = recv(server_sock, tempBuffer, header.length, 0);
            if (bytesReceived <= 0) {
                // 错误处理: 接收失败或连接关闭
                free(tempBuffer);
                break;
            }
            // 将数据从临时缓冲区复制到grid中
            // 这里假设棋盘数据是int类型的，每个int表示一个格子的状态
            for (int i = 0; i < size; i++) {
                memcpy(grid[i], &tempBuffer[i * size], size * sizeof(int));
            }
            free(tempBuffer);
            break;
        }
        case SIZE_TYPE:
        {
            printf("recv size\n");
            recv(server_sock,&size,header.length,0);
            break;
        }
        default:
            printf("recv invalid data!\n");
            break;
        }
    }
    return NULL;
}

void send_message(void *data,int type,size_t size){
    MessageHeader header;
    header.type = type;
    header.length = size;

    int total_length = sizeof(header) + size;
    char *buffer = malloc(total_length);

    memcpy(buffer,&header,sizeof(header));
    memcpy(buffer + sizeof(header),data,size);

    send(sock,buffer,total_length,0);
    free(buffer);
}

void mySendDataCallback(void *data,int type, size_t size) {
    send_message(data,type,size);
}

int main(int argc, char** argv) {
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "10.1.79.16", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0) {
        printf("Connected to the server\n");

        pthread_t thread_id;
        if(pthread_create(&thread_id, NULL, receive_msg, (void*)&sock) != 0) {
            printf("Failed to create thread for receiving messages.\n");
        }
    } else {
        printf("\nConnection Failed \n");
        return -1;
    }

    char username[10] = {0};
    do {
        printf("Please enter your name: ");
        scanf("%9s", username);
    } while(username[0] == '\0');
    send_message(username,USERNAME_TYPE,sizeof(char)*10);

    register_send_data_callback(mySendDataCallback);
    printf("Please enter grid size: ");
    scanf("%d",&size);
    send_message(&size,SIZE_TYPE,sizeof(size));
    if(size != 0){
        init_game(argc, argv); 
    }
    close(sock);

    return 0;
}
