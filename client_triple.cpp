#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <map>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr, socket_addr;           // Socket address structure
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);     // Read Port No from command line

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Open Socket


    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);        // Get host from IP

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET; // This is always set to AF_INET

    // Host address is stored in network byte order
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    // bool hash map for which ports are finished
    std::map<int, bool> port_map;
    int ports_counted;
    typedef std::map<int, bool>::iterator map_iter;
    int begin_port = 2500;
    int end_port = 2600;
    portno = begin_port;

    for (int iter = begin_port; iter <= end_port; iter++) {
      port_map[iter] = false;
    }

    while (ports_counted <= 100) {
      serv_addr.sin_port = htons(portno);

      if(port_map[portno]) {
        continue;
      }

      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
          printf("Port %d NOT open\n", portno);
      }
      else {
          printf("Port %d IS open\n", portno);
      }

      port_map[portno] = true;
      ports_counted++;
      portno++;
    }

    /*

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");



    // Read and write to socket
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));

    if (n < 0)
         error("ERROR writing to socket");


    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    */

    close(sockfd);
    return 0;
}
