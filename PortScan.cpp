#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <map>
#include <unistd.h>

/*
*   Villi Includes
*/


// villi includes - END

/*
*   Ívar includes
*/


// Ívar includes - END


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    /*
      Initizialise variables
    */
    int sockfd, portno, n;
    struct sockaddr_in serv_addr, socket_addr;           // Socket address structure
    struct hostent *server;
    int begin_port = 1025;
    int end_port = 60000;
    unsigned int sleeptime = 500000;

    // bool hash map for which ports are finished
    std::map<int, bool> port_map;
    int ports_counted = end_port - begin_port;
    //typedef std::map<int, bool>::iterator map_iter;

    if (argc > 2) {
      begin_port = atoi(argv[2]);
      end_port = atoi(argv[3]);
    }

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = end_port;

    while ((end_port - begin_port) > (port_map.size())) {

      if(port_map[portno] == true) {
        portno = begin_port + (rand() % static_cast<int>(end_port - begin_port + 1));
        continue;
      }
      //loop_test_count = 0;
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


      serv_addr.sin_port = htons(portno);

      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) >= 0) {
          printf("Port %d IS open\n", portno);
      }

      port_map[portno] = true;
      ports_counted--;
      portno = begin_port + (rand() % static_cast<int>(end_port - begin_port + 1));

      // Set sleep on
      //usleep(sleeptime);

      close(sockfd);
    }

    return 0;
}





/*****************************
*   Villi workspace VVVVVV
******************************/







/*****************************
 *  ÍVAR workspace  VVVVV
 **************************** /



