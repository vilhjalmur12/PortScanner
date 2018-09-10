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
#include <string>
#include<netinet/ip_icmp.h>   
#include<netinet/udp.h>   
#include<netinet/tcp.h>   
#include<netinet/ip.h>    
#include<sys/socket.h>
#include<arpa/inet.h>
#include<chrono>

// villi includes - END

/*
*   Ívar includes
*/


// Ívar includes - END

void triple(int *portno, const char *host_name);

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

      // setja þræði hér

      // Set sleep on
      //usleep(sleeptime);

    }

    return 0;
}

/*****************************
*   Villi workspace VVVVVV
******************************/

void *process_thread(void *host, void *portno, void *action) {
  const char *host_name = (const char *) host;
  int *port = (int *) portno;
  char *act = (char *) action;

  switch (*act) {
    case 't':
      triple(port, host_name);
      break;
    case 'u':
      // setja UDP hér
      break;
    case 's':
      // setja SYN hér
      break;
    default:
      error("cannot find action!");
  }

  return NULL;
}

void triple(int *portno, const char *host_name) {
  /*
    Initizialise variables
  */
  const char *host = (const char *) host_name;
  int sockfd;
  struct sockaddr_in serv_addr, socket_addr;           // Socket address structure
  struct hostent *server;

  char buffer[256];

  //loop_test_count = 0;
  sockfd = socket(AF_INET, SOCK_STREAM, 0); // Open Socket

  if (sockfd < 0)
    error("ERROR opening socket");

  server = gethostbyname(host);        // Get host from IP

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


  serv_addr.sin_port = htons(*portno);

  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) >= 0) {
      printf("Port %d IS open\n", *portno);
  }

  // Set sleep on
  //usleep(sleeptime);

  close(sockfd);

}





