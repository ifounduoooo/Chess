/*****************************************************************
                          Header file
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include "server_opengl.h" 

/*****************************************************************
                 SERVER initialization parameters
******************************************************************/
#define PORT 4444
#define MAX_CLIENTS 2

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
typedef struct {
    int socket;
    char username[10];
} client_info;

typedef struct{
    game_data game;
    char player1[10];
    char player2[10];
} room_info;

client_info clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

/************************************************************************
                  Function prototype declaration
*************************************************************************/
void initialize_server(int* server_fd, struct sockaddr_in* address);
void accept_client_connections(int server_fd);
void* handle_client(void* arg);
void broadcast_Chessdata(int sender_socket, game_data *game);
void send_message(int sock,void *data,int type,size_t siz);

void send_message(int sock,void *data,int type,size_t size){
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

/*****************************************************************
void broadcast_Chessdata(int sender_socket, int **grid, int gridSize)

FUNCTION: Broadcast the chessboard data to all clients
******************************************************************/
void broadcast_Chessdata(int sender_socket, game_data *game) {
    int grid_temp[game->size][game->size];
    printf("%s,%d,grid size:%d\n",__func__,__LINE__,game->size);
    // 逐个复制元素
    for (int i = 0; i < game->size; i++) {
        for (int j = 0; j < game->size; j++) {
            grid_temp[i][j] = game->grid[i][j];
            printf("%d ",grid_temp[i][j]);
        }
        printf("\n");
    }
    
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
            send_message(clients[i].socket,grid_temp,GRID_TYPE,sizeof(grid_temp));
            //send(clients[i].socket, grid_temp, sizeof(grid_temp), 0);
        }
    pthread_mutex_unlock(&client_mutex);
}

/*****************************************************************
void initialize_server(int* server_fd, struct sockaddr_in* address)

FUNCTION: 
******************************************************************/
void initialize_server(int* server_fd, struct sockaddr_in* address) {
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
    
    bind(*server_fd, (struct sockaddr *)address, sizeof(*address));
    listen(*server_fd, MAX_CLIENTS);
    printf("Listening on port %d\n", PORT);
}

/*****************************************************************
void* handle_client(void* arg)

FUNCTION: 
******************************************************************/
void* handle_client(void* arg) {
    int num = *(int*)arg;
    int socket = clients[num].socket;
    free(arg); // 释放malloc分配的内存
    game_data game;
    while (1) {
        MessageHeader header;
        int bytes_received = recv(socket, &header, sizeof(header), 0);
        if (bytes_received <= 0) break; // 客户端断开连接或出现错误
        
        // 根据消息类型和长度处理消息
        switch (header.type)
        {
        case USERNAME_TYPE:
            recv(socket, clients[num].username,header.length, 0); // 假设第一条消息是用户名
            printf("New connection form: %s\n", clients[num].username);
            break;

        case COORDINATE_TYPE:
        {
            int xy[2];
            recv(socket, xy,header.length, 0); // 假设这是坐标数据
            printf("Received from %d X:%d, Y:%d\n", socket, xy[0], xy[1]);
            if(game.size != 0){
                score(xy[0], xy[1], &game);
                broadcast_Chessdata(socket, &game);
                break;
            }
            printf("size set to 0!\n");
            break;
        }

        case SIZE_TYPE:
        {
            recv(socket, &game.size,header.length, 0);
            printf("Received from %d gridsize:%dx%d\n", socket, game.size, game.size);
            init_board(&game);
            break;
        }

        default:
            printf("recv invalid data!\n");
            break;
        }
    }
    printf("Client:%s disconnected\n", clients[num].username);
    free_board(&game);
    close(socket);
    pthread_mutex_lock(&client_mutex);
    client_count--; 
    pthread_mutex_unlock(&client_mutex);
    return NULL;
}

/*****************************************************************
void accept_client_connections(int server_fd) 

FUNCTION: 
******************************************************************/
void accept_client_connections(int server_fd) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    while (1) {
        pthread_mutex_lock(&client_mutex);
        if (client_count >= MAX_CLIENTS) {
            pthread_mutex_unlock(&client_mutex);
            sleep(1); // 短暂的延迟以避免CPU过度使用
            continue;
        }
        pthread_mutex_unlock(&client_mutex);
        
        int new_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_sock < 0) {
            perror("accept");
            continue;
        }
        
        int* new_client_index = malloc(sizeof(int));
        *new_client_index = client_count;
        
        pthread_mutex_lock(&client_mutex);
        clients[client_count++].socket = new_sock;
        pthread_mutex_unlock(&client_mutex);
        
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void*)new_client_index);
    }
}

int main(int argc, char** argv) {
    int server_fd;
    struct sockaddr_in address;
    
    initialize_server(&server_fd, &address);
    accept_client_connections(server_fd);
    
    return 0;
}
