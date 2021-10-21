#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const int port = 5000;

int init_socket(int port) {

    // create
    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sfd == -1) {
        printf("Could not create socket\n");
        return -1;
    } 

    // bind
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if (bind(sfd, (sockaddr*) &server, sizeof(server)) < 0) {
        perror("bind");
        return -1;
    }

    // listen
    if (listen(sfd, 1) < 0) {
        perror("listen");
        return -1;
    }

    return sfd;
}

int main(int argc, char** argv) {
    int sfd = init_socket(port);

    // vars for the main loop below
    struct sockaddr_in client;
    socklen_t c = sizeof(struct sockaddr);
    
    bool is_done = false;
    char buf[BUFSIZ];
    int cfd;
    while(!is_done) {

        std::cout << "listening on port: " << std::to_string(port) << std::endl;

        // accept new connections
        cfd = accept(sfd, (sockaddr*) &client, &c);
        std::cout << "Connection accepted" << std::endl;
        
        // blank out the buffer
        memset(buf, '\0', BUFSIZ);

        // read from the connection until close
        while (recv(cfd, buf, BUFSIZ, 0) > 0) {

            // if exit command, shutdown the whole server
            std::string msg = buf;
            if (msg == "exit") {
                std::cout << "Exit message recieved. Shutting down." << std::endl;
                is_done = true;
                write(cfd, msg.c_str(), msg.size());

            // otherwise, echo back to the 
            } else {
                std::cout << "Read [" << msg << "] from socket" << std::endl;
                msg += " from the server";
                std::cout << "Writing [" << msg << "] to socket" << std::endl;
                write(cfd, msg.c_str(), msg.size());
            }

            // blank out the buffer
            memset(buf, '\0', BUFSIZ);
        }
        
        close(cfd);
    }
    close(sfd); 

    return 0;
}