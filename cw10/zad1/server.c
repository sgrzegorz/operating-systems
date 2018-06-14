#include "common.h"

#define MAX_EVENTS 10
#define MAX_CLIENTS 4096

#define WRITE_MSG(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}

typedef struct{
    int is_active;
    int fd;
}Client;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

Client client[MAX_CLIENTS];
int nclients=0;

//file descriptors
int server_socket, client_socket, epoll;

pthread_t threads[3];

void __del__();
void __init__(int argc, char*argv[]);
void registerClient();
void receiveMessage(int fd);
void *handleTerminal(void * arg);


void signalHandler(){
    exit(0);
}



///etc/init.d/networking restart

void removeClient(){

}



int main(int argc, char*argv[]){
    __init__(argc,argv);
    pthread_create(&threads[0],NULL,handleTerminal,NULL);

    printf("Server starts loop.\n");

    while(1){
        // struct epoll_event event;
        struct epoll_event event;
        int nfd = epoll_wait(epoll,&event,1,-1);

        

        if(event.data.fd == server_socket){
            registerClient();
        }else{
            WRITE_MSG("me:\n")
            receiveMessage(event.data.fd);
        }
       
    }
    close(server_socket);

}


void receiveMessage(int fd){
    Msg msg;
    read(fd,&msg,sizeof(Msg));
    printf("%i %i\n",msg.arg1,msg.arg2);
}


void registerClient(){
    int new_client = accept(server_socket,NULL,NULL);
    WRITE_MSG("New client: %i registered\n",new_client);

    pthread_mutex_lock(&mutex);
    int client_registered_successfully = 0;
    for(int i=0;i<MAX_CLIENTS;i++){
        if(client[i].is_active = 0){
            client[i].fd = new_client;
            client[i].is_active=1;
            nclients++;
            WRITE_MSG("Client registered successfully\n");
            client_registered_successfully=1;
            break;
        }   
    }
    if(!client_registered_successfully) WRITE_MSG("Client wasn't registered\n");
    pthread_mutex_unlock(&mutex);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = new_client;
    if(epoll_ctl(epoll,EPOLL_CTL_ADD,new_client,&event)== -1) FAILURE_EXIT("Failed to register client on epoll: %s\n",strerror(errno));

}


void *handleTerminal(void * arg){
    while(1){
        
        Msg msg;
        char type;
       
        scanf(" %c %i %i",&type,&msg.arg1,&msg.arg2);
       
        switch(type){
            case '+':
                msg.type = ADD;
                break;
            case '-':
                msg.type = SUB;
                break;
            case '/':
                msg.type = DIV;
                break;
            case '*':
                msg.type = MUL;
                break;
            default:
                WRITE_MSG("unknown command\n");     
        }
        
        pthread_mutex_lock(&mutex);

        if(nclients == 0){
            WRITE_MSG("No clients registered...\n"); 
            pthread_mutex_unlock(&mutex);
            continue;
        } 

        
        int who = rand()%nclients;
        int k=0;
        for(int i=0;i<MAX_CLIENTS;i++){
            if(client[i].is_active){
                if(k == who){
                    write(client[i].fd,&msg,sizeof(Msg));
                    break;
                }
                k++;
            }
        } 
        pthread_mutex_unlock(&mutex);
    }
}

void __init__(int argc, char*argv[]){
    signal(SIGINT,signalHandler);
    atexit(__del__);
    srand(time(NULL));
    
    server_socket = socket(AF_INET, SOCK_STREAM,0);
    if(server_socket == -1)  FAILURE_EXIT("Failed to create communication endpoint\n");

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(9991);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    int tr =1;
   // if (setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int)) == -1) FAILURE_EXIT("Dekete address alreadin use\n");
       
    if(bind(server_socket,(const struct sockaddr*) &address,sizeof(address)) == -1) FAILURE_EXIT("Failed to assign address to a socket: %s\n",strerror(errno));

    if(listen(server_socket,100) == -1) FAILURE_EXIT("Failed to mark server_socket as a passive socket\n");

    epoll = epoll_create1(0);
    if(epoll == -1) FAILURE_EXIT("Failed to create new epoll instance: %s\n",strerror(errno));

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_socket;
    if(epoll_ctl(epoll,EPOLL_CTL_ADD,server_socket,&event)== -1) FAILURE_EXIT("Failed to register server_socket file descriptor on epoll instance:   %s\n",strerror(errno));

}


void __del__(){
    if(shutdown(server_socket,SHUT_RDWR)) printf("Atexit failed to shutdown server_socket: %s\n",strerror(errno));
    close(server_socket);
    for(int i=0;i<MAX_CLIENTS;i++){
        if(client[i].is_active) close(client[i].fd);
    }
}