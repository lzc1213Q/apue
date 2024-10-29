#include <stdio.h>SOCK_STREAM
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

char buffer[1024];
void *pthread_function(void *arg);
pthread_mutex_t work_mutex;

void *pthread_function(void *arg)
{
	int fd = (int) arg;//函数形参，即连接成功后的套接字赋给fd.
	int result;
	fd_set read_fds;//文件描述符集合，用于select函数。
	int max_fds;//文件描述符集和的最大数
	
	printf("%d id has connected!!\n",fd);
	while(1)
	{
		FD_ZERO(&read_fds);//清除文件描述符集中的read_fds中的所有位（即把所有位都设置为0）
		FD_SET(0,&read_fds);//将标准输入放入文件描述符集合，用于键盘输入。
		FD_SET(fd,&read_fds);//将连接后的客户文件描述符放入监听的文件描述符集合，这个用于向客户端读取数据
		max_fds = fd + 1;
		
		pthread_mutex_lock(&work_mutex);
		printf("%d has get the lock\n",fd);
		result = select(max_fds, &read_fds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL);//开始监听那些文件描述符处于可读状态
		
		if(result < 1)
		{
			printf("select");

			
		}
		if(FD_ISSET(0,&read_fds))
		{  //如果标准输入处于可读取状态，说明键盘有输入，将输入的数据存放在buffer中，
			//然后向客户端写回，如果输入quit将退出一个聊天线程。
			memset(buffer,'\0',sizeof(buffer));//清空数组
			fgets(buffer,sizeof(buffer),stdin);
			if((strncmp("quit",buffer,4))==0)
			{
				printf("You have terminaled the chat\n");
				pthread_mutex_unlock(&work_mutex);
				break;
			}
			else
			{
				result = write(fd,buffer,sizeof(buffer));
				if(result == -1)
				{
					perror("write");
					exit(EXIT_FAILURE);
				}
			}
		}
	if(FD_ISSET(fd,&read_fds))//如果客户套接字符可读，那么读取存放在buffer中
		//然后显示出来，如果对方中断聊天，那么result==0
	{
		memset(buffer,'\0',sizeof(buffer));
		result = read(fd,buffer,sizeof(buffer));
		if(result == -1)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		else if(result == 0)
		{
			printf("the other side has terminal the chat \n");
			pthread_mutex_unlock(&work_mutex);
			break;
		}
		else
		{
			printf("receive message:%s",buffer);
		}
	}
	pthread_mutex_unlock(&work_mutex);
	sleep(1);//如果没有这一行，当前线程会一直占据buffer，让当前线程暂停一秒可以实现1对N的功能
	}
	close(fd);
	pthread_exit(NULL);
}


int main(int argc,char **argv)
{
	int result;
	struct sockaddr_in server_address,client_address;
	int client_len;
	int server_socketfd,client_socketfd;
	pthread_t a_thread;
	
  //初始化互斥锁
  result=pthread_mutex_init(&work_mutex,NULL);
  if(result != 0)
  {
  	perror("pthread_mutex_init");
  	exit(EXIT_FAILURE);
  }
  server_socketfd = socket(AF_INET,SOCK_STREAM,0);
  
  server_address.sin_family = AF_INET;
  //strcpy(server_address.sun_path,"server_socket");
  server_address.sin_addr.s_addr=htonl(INADDR_ANY);
  server_address.sin_port = htons(10011);
  //unlink(server_socket);//把以前存在当前目录下的套接字删除。
  
  result = bind(server_socketfd,(struct sockaddr*)&server_address,sizeof(server_address));//这里应该有个bind绑定鉴定函数。绑定不成功该怎么办！！
  
  if(result != 0)
  {
  	perror("bind");
  	exit(EXIT_FAILURE);
  }
   result = listen(server_socketfd,5);
   if(result != 0)
   {
   		perror("listen");
   		exit(EXIT_FAILURE);
   }
	 client_len = sizeof(client_address);
	 while(1)
	 {  //调用accept函数来接受客户端的连接，这时就可以和客户端通信了。
	 		client_socketfd = accept(server_socketfd,(struct sockaddr*)&client_address,&client_len);
	 		result = pthread_create(&a_thread,NULL,pthread_function,(void *)client_socketfd);//成功接收一个请求后，就会创建一个线程，然后主线程又进入accept函数，如果此时没有新的连接请求，进入阻塞状态
	 		if(result !=0 )
	 		{
	 			perror("pthread_create");
	 			exit(EXIT_FAILURE);
	 		}
	 }
}




