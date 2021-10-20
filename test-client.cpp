#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h> // socket
#include <arpa/inet.h>	// inet_addr
#include <unistd.h>

int init_socket(const int port, const char* path) {
    int sfd;
	struct sockaddr_in server;
	
	// create socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		std::cerr << "ERROR: Create socket failed." << std::endl;
		return sfd;
	}
	
	server.sin_addr.s_addr = inet_addr(path);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	// connect to remote server
	if (connect(sfd, (struct sockaddr*) &server, sizeof(server)) < 0) {
		std::cerr << "ERROR: Connect failed." << std::endl;
        close(sfd);
		return -1;
	}

    return sfd;
}

int main(int argc , char *argv[]) {

    int port;
	if (argc != 2) {
		std::cout << "usage: ./test-client [port]" << std::endl;
		return 0;
	} else {
		port = atoi(argv[1]);
	}

	const char path[] = "127.0.0.1";
    int sfd = init_socket(port, path);
    
    std::string msg = "Hello from the client";

    char server_msg[BUFSIZ];

    for (int i = 0; i < 3 ; i++){
        memset(server_msg, '\0', BUFSIZ);
        if (i == 0) {
            msg = "Hello from the client";
        }
        else if (i == 1) {
            msg = "My name is Robert from the client";
        } else {
            msg = "exit";
        }

        std::cout << "Sending [" << msg << "] to the server" << std::endl;
        int r = send(sfd, msg.c_str(), msg.size(), 0);
        if (r <= 0) {
            std::cerr << "Error: send" << std::endl;
            close(sfd);
            return 1;
        }
        
        int s = recv(sfd, server_msg, BUFSIZ, 0); 
        if (s <= 0) {
            std::cerr << "Error: recv" << std::endl;
            close(sfd);
            return 1;
        }

        std::cout << "Recieved [" << server_msg << "] from the server" << std::endl;
    }

    close(sfd);

    return 0;
}