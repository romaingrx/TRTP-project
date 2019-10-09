#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr

int main(int argc, char* argv){
  int socket_desc, c, new_socket;
  struct sockaddr_in server, client;

  //Create socket
  socket_desc = socket(AF_INET6 , SOCK_DGRAM , 0);
  if (socket_desc == -1)
  {
  	printf("Cant create cocket");
  }

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("::1");
  server.sin_port = htons(80);

  //Bind
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) ==  0)
  {
  	printf("Bind fail\n");
  }

  listen(socket_desc , 3);

//NOW ACCEPTING
  c = sizeof(struct sockaddr_in);
	new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (new_socket==0)
	{
		printf("accept fail\n");
	}

	printf("Connected\n");


}
