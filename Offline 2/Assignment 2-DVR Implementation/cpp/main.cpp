#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;


int main(int argc, char *argv[]){

	int bytes_received;

	int sockfd;
	
	char buffer[1024];
	

	if(argc != 3){
		printf("%s <ip address>\n", argv[1]);
		exit(1);
	}
	
	
//	printf("Router's IP address: %s",IPAddress);
	
    int bind_flag;
    struct sockaddr_in client_address;

    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(4747);
    inet_pton(AF_INET, argv[1], &client_address.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));

	if(!bind_flag) cout<<"Connection successful!!"<<endl;
	else cout<<"Connection failed!!!"<<endl;

    cout<<"--------------------------------------"<<endl;
    struct sockaddr_in router_address;
    socklen_t addrlen;

	while(true)
	{
		char buffer[1024];
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &router_address, &addrlen);
		if(bytes_received!=-1)
		{
			cout << "Hello from : \n" ;
			string recv(buffer);
			cout << recv;
		}
	}

	return 0;

}
