#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER "127.0.0.1"
#define PORT 8888
#define BUFSIZE  512
#define NAMESIZE 10

void isEnd(char *msg, int * END){
    if(strcmp(msg,"exit") == 0){
       // printf("[%d] End connection \n",getpid());
        *END = 1;
    }
    

}

void stop(char* msg,int FD){
	perror(msg);
    close(FD);
	exit(EXIT_FAILURE);
	
}

void ChangeName(int socket,char * name){

    char Rep[3];
    printf("Name (max %d lettre):",NAMESIZE-3);
    fflush(stdout);
    scanf("%[^\n]",name);
    fgetc( stdin );
    send(socket,name,NAMESIZE,0);

    int n = recv(socket,Rep,2,0);
    Rep[n]='\0';

    printf("%s\n",Rep);

    if(strcmp(Rep,"NO")){
        ChangeName(socket,name);
    }
    else{
        strcat(name,": ");
    }
}


int main(int argc, char const *argv[])
{
    char buff[BUFSIZE];
    char name[NAMESIZE];
    char msg[BUFSIZE - NAMESIZE];
    int END = 0;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) stop("socket",sockfd);
    struct sockaddr_in serv_addr;
    int len=sizeof(serv_addr);
    int pid;

    bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
    inet_aton(SERVER,&serv_addr.sin_addr);
    serv_addr.sin_port = htons(PORT);
    
   
    if (connect(sockfd,(const struct sockaddr *)&serv_addr,(socklen_t )len) < 0) stop("Connect",sockfd);

    ChangeName(sockfd,name);

    printf("Ecrivez vos messages \n");
    if ((pid = fork()) < 0) stop("fork petit",sockfd);

    while(END == 0){
        
        if (pid !=0){
            //pere
            bzero(&buff,BUFSIZE);
            fflush(stdout);
            scanf("%[^\n]",msg);
            strcat(buff,name);
            strcat(buff,msg);
            send(sockfd,buff,BUFSIZE,0);
            fgetc( stdin );
        }
        else{
            //fils
            int n =recv(sockfd,buff,BUFSIZE,0);
            if (n == -1)  stop("recv",sockfd);
            if (n == 0) break;
            buff[n]='\0';
            printf("\n%s\n",buff);
        }

        isEnd(buff,&END);

    }

    close(sockfd);

    if (pid==0) printf("End connection \n");

    return 0;
}
