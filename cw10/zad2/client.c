#include "common.h"

int socket_fd;
char name[MAX_ARRAY];
char *ip;
int port;
char *path;
struct sockaddr_in msg_addr;
Msg msg;

void __del__();
void howToUse();
void sigintHandler(){ exit(0);}
void __init__(int argc, char *argv[]);

enum type{LAN,UNIX};
int connection;

void registerOnServer(){
    
    strcpy(msg.name,name);
    msg.type = REGISTER;
    if(connection==LAN){
        if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto1 %s\n",strerror(errno));    
        if(recvfrom(socket_fd,&msg,sizeof(Msg),0 ,0,0) !=sizeof(Msg)) WRITE("recvform\n");  
    }else{
        write(socket_fd,&msg,sizeof(Msg));
        read(socket_fd,&msg,sizeof(Msg));
    }
   
   

    switch(msg.type){
        case(KILL_CLIENT):
            
            //if(shutdown(socket_fd,SHUT_RDWR)) printf("Atexit failed to shutdown socket_fd\n");
            // close(socket_fd);
            exit(0);
            break;
        case(SUCCESS):
            atexit(__del__);
            break;
    }
}


int main(int argc, char *argv[]){
    
    __init__(argc, argv);
    registerOnServer();
    
    WRITE("I registered\n");
    while(1){
        
        Msg msg;
        if(connection == LAN){
            if(recvfrom(socket_fd,&msg,sizeof(Msg),0 ,0,0) !=sizeof(Msg)) WRITE("recvform1\n");  
        }else{
            read(socket_fd,&msg,sizeof(Msg));
        }
        
        
       // read(socket_fd,&msg,sizeof(Msg));
        if(msg.type!=PING) WRITE("Msg received\n");
        if(msg.type==PING &&verbose) WRITE("Ping\n");
       
        switch(msg.type){
            case MUL:
                
                msg.type = RESULT;
                msg.result = msg.arg1 * msg.arg2;
                
                if(connection == LAN){
                    if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");  
                }else{
                    write(socket_fd,&msg,sizeof(Msg));
                }
                  

                break;
            case ADD:
                
                msg.type = RESULT;
                msg.result = msg.arg1 + msg.arg2;

                if(connection == LAN){
                    if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");  
                }else{
                    write(socket_fd,&msg,sizeof(Msg));
                }
                break;

            case DIV:
                
                msg.type = RESULT;
                msg.result = msg.arg1 / msg.arg2;
                  

                if(connection == LAN){
                    if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");  
                }else{
                    write(socket_fd,&msg,sizeof(Msg));
                }
                break;

            case SUB:
                         
                msg.type = RESULT;
                msg.result = msg.arg1 - msg.arg2;
                if(connection == LAN){
                    if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");    
                }else{
                    write(socket_fd,&msg,sizeof(Msg));
                }
                

                break;

            case KILL_CLIENT:
                exit(0);
                break;

            case PING:
                strcpy(msg.name,name);
                msg.type = PONG;
                 if(connection == LAN){
                    if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");     
                }else{
                    write(socket_fd,&msg,sizeof(Msg));
                }
                

                break;
            default:
              
                exit(0);
        }
    }

}



void __del__(){
    Msg msg;
    strcpy(msg.name,name);
    msg.type = UNREGISTER;
       

    if(connection == LAN){
        if(sendto(socket_fd,&msg,sizeof(Msg),0 ,(struct sockaddr*)&msg_addr,(socklen_t) sizeof(struct sockaddr))!=sizeof(Msg)) WRITE("sendto\n");  
    }else{
        write(socket_fd,&msg,sizeof(Msg));
    }
    
    // if(shutdown(socket_fd,SHUT_RDWR)) printf("Atexit failed to shutdown socket_fd\n");
    // close(socket_fd);
}



void howToUse(){
    printf("./client <name> <lan> <ipv4> <port>  or ./client <name> <unix> <path> \n");
    printf("./client 1 lan 94.254.145.105 9992\n");
    printf("./client 1 unix ./myfile\n");
    exit(0);
}
void __init__(int argc, char *argv[]){
    signal(SIGINT,sigintHandler);
    
    if(strcmp(argv[2],"lan")==0 && argc == 5){
        strcpy(name,argv[1]);
        connection = LAN;
       
        
        

        uint32_t ip = inet_addr(argv[3]); // this code I get when I type "what is my ip?" in internet
        if(ip == -1) FAILURE_EXIT("Failed to convert ip to byte order network address: %s\n",strerror(errno));
       
        msg_addr.sin_family = AF_INET;    
        msg_addr.sin_addr.s_addr =INADDR_ANY;
        int port = atoi(argv[4]);
        if (port < 1024 || port> 65535)  FAILURE_EXIT("Incorrect number of port\n");
        msg_addr.sin_port = htons(atoi(argv[4]));

        socket_fd = socket(AF_INET, SOCK_DGRAM,0);
        if(socket_fd == -1) FAILURE_EXIT("Failed to create client socket\n");

        if(connect(socket_fd, (const struct sockaddr*) &msg_addr, sizeof(struct sockaddr)) == -1) FAILURE_EXIT("Failed to assign server_addr to a web_fd: %s\n",strerror(errno));
        
    }else if(strcmp(argv[2],"unix")==0 && argc == 4){
        strcpy(name,argv[1]);
        path = argv[3];
        connection = UNIX;
        struct sockaddr_un local_address;
        local_address.sun_family = AF_UNIX;
        strcpy(local_address.sun_path,path);

        socket_fd = socket(AF_UNIX, SOCK_DGRAM,0);
        if(socket_fd == -1) FAILURE_EXIT("Failed to create client socket\n");

        int res = connect(socket_fd,(const struct sockaddr*) &local_address, sizeof(local_address));
        if(res == -1) FAILURE_EXIT("Failed in connecting to server_socket unix: %s\n",strerror(errno));
    

        
    }else{
        howToUse();
    }
    


    
}