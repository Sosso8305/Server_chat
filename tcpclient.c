/**
	Simple TCP client to fetch a web page
*/

#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr

int main(int argc , char *argv[])
{
	int socket_desc;
	struct sockaddr_in server;
	char *message , server_reply[512];
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8000 );

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected\n");
    int n;
	while(1){
        //Receive a reply from the server
        if( (n=recv(socket_desc, server_reply , 511 , 0)) == -1)
        {
            perror("rcv");
            break;
        }
        if(n==0){
            puts("client disconnected");
            break;
        }
        server_reply[n]='\0';
        puts("Reply received\n");
        puts(server_reply);
        
        
    }
	return 0;
}