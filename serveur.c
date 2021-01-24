/**
	Handle multiple socket connections with select and fd_set on Linux
*/
 
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
 
#define TRUE   1
#define FALSE  0
#define PORT 8888
#define NAMESIZE 10
#define DEBUG 1

void SendPrivateMe(int socket,char * name,char * msg,char ** listName,int * Csocket,int maxClient, int nb_sender){
    int check=0;
    char * Fail ="Your friend don't exist or it's you";
    //char * Success = "MSG send";


    for(int i = 0; i<maxClient; i++){
        if(!strcmp(listName[i],name) && i!=nb_sender){
            char buff[1025];
            bzero(buff,1025);
            strcat(buff,"(private)");
            strcat(buff,listName[nb_sender]);
            strcat(buff,": ");
            strcat(buff,msg);
            send(Csocket[i],buff,strlen(buff),0);
            check=1;
            //send(socket,Success,strlen(Success),0);  // pas besoin de s'envoyer une confirmation pour un msg pour les fichier suremment 
            break;
        } 
    }

    if(!check){
        send(socket,Fail,strlen(Fail),0);
    }
}

void ChangeName(int socket,char * name, char ** listName, int maxClient,int numero_tab){
    char * Success = "Change name is a success      ";
    char * Fail = "name already used, check /list";
    int check = 0;

    for (int i = 0;i<maxClient; i++){
        if(!strcmp(listName[i],name)){
            send(socket,Fail,strlen(Fail),0);
            check=1;
            break;
        }
    }

    if(!check){
        strcpy(listName[numero_tab],name);
        send(socket,Success,strlen(Success),0);
    }

}

// @old_methode
// void Name(int socket,char * name, char ** listName, int maxClient,int numero_tab){
//     char * NoValid = "NO";
//     char * Valid  = "OK";
//     int check = 0;
//     bzero(name,NAMESIZE);
//     recv(socket,name,NAMESIZE,0);
//     printf("name users: %s\n",name);

//     for (int i = 0;i<maxClient; i++){
//         if(!strcmp(listName[i],name)){
//             send(socket,NoValid,strlen(NoValid),0);
//             check =1;
//             break;
//         }
//     }

//     if (check){
//         Name(socket,name,listName,maxClient,numero_tab);
//     }
//     else{
//         send(socket,Valid,strlen(Valid),0);
//         strcpy(listName[numero_tab],name);
//    }

// }


int main(int argc , char *argv[])
{
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, i , valread , sd;
	int max_sd;
    struct sockaddr_in address;
    char name[NAMESIZE+3];
    char msg[1025];
    int number_curr_users = 0;
     
    char buffer[1025];  //data buffer of 1K
    char * TableName[max_clients];

    //init tableName
    for (int i = 0; i<max_clients; i++){
        TableName[i] = malloc(sizeof(char)*NAMESIZE);
    }
     
    //set of socket descriptors
    fd_set readfds;
     
    //a message
    char *message = "Chat Server v2.0 \r\n";
 
    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) 
    {
        client_socket[i] = 0;
    }
     
    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
 
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
     
    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	printf("Listener on port %d \n", PORT);
	
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
     
    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
    
	while(TRUE) 
    {
        //clear the socket set
        FD_ZERO(&readfds);
 
        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
		
        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++) 
        {
            //socket descriptor
			sd = client_socket[i];
            
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
            
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
				max_sd = sd;
        }
 
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
   
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error\n");
        }
         
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) 
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }




            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
       
           
             
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) 
            {
                //if position is empty
				if( client_socket[i] == 0 )
                {

                    //Name(new_socket,name,TableName,max_clients,i);
                    
                    client_socket[i] = new_socket;
                    number_curr_users++;
                    printf("Adding to list of sockets as %d\n" , i);
					break;
                }
            }

             //send new connection greeting message
            if( send(new_socket,message, strlen(message), 0) != strlen(message) ) 
            {
                perror("send");
            }
             
            puts("Welcome message sent successfully");
        }
         
        //else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
             
            if (FD_ISSET( sd , &readfds)) 
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , msg, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                     
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                    bzero(TableName[i],NAMESIZE);
                    number_curr_users--;
                }
                 
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    msg[valread] = '\0';
                    char slash ='/';
                    char * delim=" ";
                    char msg2[1025] ;
                    strcpy(msg2,msg);
                    char * ptr=strtok(msg2,delim);

                    if(DEBUG) printf("recievd n°%i: %s\n",i,msg);
                    

                    if(!strcmp(msg,"/list")){
                        puts("User's list: ");
                        bzero(buffer,1025);
                        for (i= 0; i < number_curr_users; i++){
                            if(DEBUG) printf("%s\n",TableName[i]);
                            strcat(buffer,TableName[i]);
                            strcat(buffer,"\n");
                        }
                        send(sd,buffer,1024,0);
                    }

                    else if(!strcmp(ptr,"/nick")){    
                        ptr = strtok(NULL,delim);
                        if(!ptr) {
                            char * cmd = "command: /nick <name>";
                            send(sd,cmd,strlen(cmd),0);
                        }
                        else ChangeName(sd,ptr,TableName,number_curr_users,i);
                    }


                    else if(!strcmp(ptr,"/me")){
                        char MP_name[NAMESIZE];
                        char MP_msg[1025];
                        int flag =0;

                        bzero(MP_msg,1025);

                        ptr = strtok(NULL,delim);
                        if(!ptr) flag=1;
                        else strcpy(MP_name,ptr);

                        ptr = strtok(NULL,delim);
                        if(!ptr) flag=1;
                        else {
                            while(ptr != NULL){
                                strcat(MP_msg,ptr);
                                strcat(MP_msg,delim);
                                ptr = strtok(NULL,delim);
                            }
                            
                        }

                        if (DEBUG) printf("private msg: %s\n",MP_msg);

                        if(flag) {
                            char * cmd = "command: /me <name> <msg>";
                            send(sd,cmd,strlen(cmd),0);
                        }
                        else SendPrivateMe(sd,MP_name,MP_msg,TableName,client_socket,number_curr_users,i);
                    }

                    else if(!strcmp(msg,"@all")){
                        char * beep ="Beep";

                        int sdd;
                        for (i= 0; i < number_curr_users; i++){
                            sdd = client_socket [i];
                            if (sdd != sd && sdd != 0){
                                send(sdd,beep,strlen(beep),0);
                            }
                        }

                    }

                    else if (slash == msg[0]){
                        char * err = "Command don't exist";
                        send(sd,err,strlen(err),0);
                    }
                     
                    else {
                        
                        
                        bzero(name,NAMESIZE);
                        bzero(buffer,1025);
                        strcpy(name,TableName[i]);
                        strcat(name,": ");
                        strcat(buffer,name);
                        strcat(buffer,msg);

                        int sdd;
                        for (i= 0; i < max_clients; i++){
                            sdd = client_socket [i];
                            if (sdd != sd && sdd != 0){
                                send(sdd,buffer,strlen(buffer),0);
                            }
                        }
                    }
                }
            }
        }
    }
     
    return 0;
}
