#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>


int main(int argc,char** argv)
{
	int result;
	int socketfd;
	int len;
	struct sockaddr_in address;
	fd_set read_fds,test_fds;
	int fd;
	int max_fds;
	char buffer[1024];
	
	while(1)
	{
	if(argc != 2)
		{
			printf("usage: ./client <ipaddress>\n"); 
			exit(1);
		}
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	
	address.sin_family = AF_INET;
	//strcpy(address.sun_path,"server_socket");
	//address.sin_addr.s_addr = inet_addr("192.168.4.36");
	address.sin_port = htons(10011);
	len=sizeof(address);
	
	if( inet_pton(AF_INET, argv[1], &address.sin_addr) == -1)////////////////////////////
	  {    
	  		printf("inet_pton error for %s\n",argv[1]);    
	  		exit(1);   
	  }
	
	result = connect(socketfd,(struct sockaddr*)&address,len);
	if(result == -1)
	{
		perror("connect");
		exit(EXIT_FAILURE);
		
	}
	FD_ZERO(&read_fds);
	FD_SET(0,&read_fds);
	FD_SET(socketfd,&read_fds);
	max_fds = socketfd +1;
	
	printf("Chat now!!\n");
	
	while(1)
	{
		test_fds = read_fds;
		result = select(max_fds,&test_fds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL);
		if(result<1)
		{
			perror("select");
			exit(EXIT_FAILURE);
		}
		if(FD_ISSET(0,&test_fds))
		{
			memset(buffer,'\0',sizeof(buffer));
			
			fgets(buffer,sizeof(buffer),stdin);
			if((strncmp("quit",buffer,4))==0)
				{
						printf("\n you are going to quit\n");
						break;
		}
		result = write(socketfd,buffer,sizeof(buffer));
		if(result == -1)
			{
				perror("write");
				exit(EXIT_FAILURE);
				}
		}
		if(FD_ISSET(socketfd,&test_fds))
			{
				memset(buffer,'\0',sizeof(buffer));
				result = read(socketfd,buffer,sizeof(buffer));
				if(result == -1)
					{
						perror("read");
						exit(EXIT_FAILURE);
					}
				else if(result == 0)
					{
						printf("the other side has terminal chat!\n");
						break;
					}
				else
					{
						printf("receive:%s",buffer);
					}
		}
  }
	close(socketfd);
	exit(EXIT_SUCCESS);
  }
}
