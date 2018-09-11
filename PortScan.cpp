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
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include <pthread.h>

// villi includes - END

/*
*   Ívar includes
*/
#include <netinet/ip_icmp.h>   //Provides declarations for icmp header
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <arpa/inet.h>
// Ívar includes - END

void sendSYN(int portno);
void triple(int *portno, const char *host_name);
//void *process_thread(void *host, void *portno, void *action);
void udp(struct hostent *server, int *portNumber);
void process(const char *host_name, int port, char act);
void sendSYN(struct hostent *hostentry, int portno);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

struct arg_struct {
  int portno;
  std::string host;
  char action;
};

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr,"usage: %s hostname port-low port-high protocol(u = UDP, t = triple handshake, s = SYN)\n", argv[0]);
        exit(0);
    }

    /*
      Initizialise variables
    */

    int sockfd, portno, n;
    struct sockaddr_in serv_addr, socket_addr;           // Socket address structure
    struct hostent *server;
    int begin_port = 1025;
    int end_port = 60000;
    unsigned int sleeptime = 500000;
    int port_count;

    // bool hash map for which ports are finished
    std::map<int, bool> port_map;
    int ports_counted = end_port - begin_port;
    //typedef std::map<int, bool>::iterator map_iter;

    if (argc > 2) {
      begin_port = atoi(argv[2]);
      end_port = atoi(argv[3]);
    }

    port_count = end_port - begin_port;

    //Init threads
    pthread_t thread_arr[port_count];

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = end_port;
    std::string hostn = argv[1];
    char *action = argv[4];
    

    while ((end_port - begin_port) > (port_map.size())) {

      if(port_map[portno] == true) {
        portno = begin_port + (rand() % static_cast<int>(end_port - begin_port + 1));
        continue;
      }

      process(argv[1], portno, *argv[4]);

      /* þræðir eru í bullinu
      struct arg_struct *args = new struct arg_struct();
      args->portno = portno;
      args->action = *action;
      args->host = hostn;

      pthread_create(&thread_arr[portno], NULL, process_thread ,(void *)&args);

      */

      port_map[portno] = true;
      ports_counted--;
      portno = begin_port + (rand() % static_cast<int>(end_port - begin_port + 1));

    }

  /*
    for (int i = 0; i < port_count; i++) {
      pthread_join(thread_arr[i], NULL);
    }
  */
    return 0;
}

/*****************************
*   Villi workspace VVVVVV
******************************/

void process(const char *host_name, int port, char act) {

    struct hostent *serv;
    if((serv = gethostbyname(host_name)) == NULL) {
        error("Error with source address");
    }

  switch (act) {
    case 't':
        triple(&port, host_name);
        break;
    case 'u':
        udp(serv ,&port);
      break;
    case 's':
      sendSYN(serv, port);
      break;
    default:
      error("cannot find action!");
  }
}
/*
void *process_thread(void *arguments) {

  struct arg_struct *args = arguments;
  const char *host_name = (const char *) args->host;
  int *port = (int *) args->port;
  char *act = (char *) args->action;

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
*/
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


/*****************************
 *  ÍVAR workspace  VVVVV
 **************************** */

void udp(struct hostent *server, int *portNumber) {
    //variables
    int sockfd = 0, sockfd2 = 0, bytes_sent = 0, bytes_recv = 0;
    struct sockaddr_in their_addr;
    //struct hostent *he;
    struct sockaddr saddr;
    //unsigned char *buffer[200];
    unsigned char *buffer = (unsigned char *)malloc(65536); //TEST HARD COPY
    // ATH
    struct icmphdr *recv = (struct icmphdr*) (buffer + sizeof(struct iphdr));
    socklen_t addr_len;

    //get host info
    /*if((he = gethostbyname(server) == NULL) {
        herror("gethostbyname");
        exit(1);
    }*/
    // creating a socket using UDP
    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        printf("in socket 1 error begin");
        exit(1);
    }
    //added extra socket like on piazza and RAW to recv ICMP error
    if((sockfd2 = socket(AF_INET, SOCK_RAW|SOCK_NONBLOCK, IPPROTO_ICMP)) < 0) {
        perror("socket2");
        printf("in socket 2 error begin");
        exit(1);
    }

    //portnumber
    int portnum = *portNumber;
    // host byte order
    their_addr.sin_family = AF_INET;
    // short, network byte order
    their_addr.sin_port = htons(portnum);
    their_addr.sin_addr = *((struct in_addr *)server->h_addr);
    // zero the rest of the struct
    bzero(&(their_addr.sin_zero), 8);

    if((bytes_sent = sendto(sockfd, "\n", 1, 0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) < 0) {
        perror("sendto");
        close(sockfd);
        exit(1);
    }

    printf("sent %d bytes to %s\n", bytes_sent, inet_ntoa(their_addr.sin_addr));

    for(int i = 0; i <= 5; i++) {
        //here is sockfd2 used
        bytes_recv = recvfrom(sockfd2, buffer, 65536, 0,(struct sockaddr *)&their_addr, &addr_len);
        if(bytes_recv < 0) {
            printf("Port %d is open\n", portnum);
            printf("Recvfrom error, Failed to get the packets\n");
            continue;
        }
        else {
            //ATH
            printf("Msg received:\ttype %d\tcode %d\n", recv->type, recv->code);
            printf("Port %d is closed\n", portnum);
            break;
        }
    }
    close(sockfd);
    close(sockfd2);
}

unsigned short csum(unsigned short *buf,int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--){sum += *buf++;}
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short) (~sum);
}

void sendSYN(struct hostent *hostentry, int portno) {

    int destination_port = htons(portno);
    int source_port = htons(random());
    char buffer[65536], rbuffer[65536];
    char string[65536];
    char *host_source, *host_dest;
    int dlength = (sizeof(struct tcphdr) + sizeof(struct iphdr));

    struct iphdr *iphdr, *riphdr;
    struct tcphdr *tcphdr, *rtcphdr;

    struct sockaddr_in sock_addr, dest_addr;
    struct sockaddr from;
    int ethlen;
    socklen_t fromlen;

    host_source = "localhost";
    host_dest = "portno";

    printf("host_dest: %c", &host_dest);
    // get host destination
    if((hostentry = gethostbyname(host_source)) == NULL) {
        error("source address error");
    }

    bzero(&sock_addr, sizeof(struct sockaddr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr = *((struct in_addr *)hostentry->h_addr);

    

    bzero(&dest_addr, sizeof(struct sockaddr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr = *((struct in_addr *)hostentry->h_addr);

    dest_addr.sin_port = destination_port;

    
    //ethlen = sizeof(ethhdr);
    setvbuf(stdout, NULL, _IONBF, 0);
    fflush(stdout);

    int s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(s < 0) { error("Error: error opening socket");
    }

    int rs;
    if((rs = socket(AF_INET, SOCK_PACKET, htons(0x0800))) < 0) {
        error("rs socket input");
    }

    int on = 1;
    
    if(setsockopt(s, IPPROTO_IP, IP_HDRINCL,(char *)&on, sizeof(on)) < 0) {
            error("set sock option error");
    }
    

    bzero(buffer, 65536);

    // TCP header
    tcphdr = (struct tcphdr *) (buffer + sizeof(struct iphdr));
    tcphdr->source = htons(source_port);
    tcphdr->dest = destination_port;
    tcphdr->window = htons(65535);
    tcphdr->seq = random();
    // Flags
    tcphdr->syn = 1;
    tcphdr->ack = 0;
    tcphdr->rst = 0;
    tcphdr->psh = 0;
    tcphdr->urg = 0;
    tcphdr->fin = 0;
    tcphdr->doff = sizeof(struct tcphdr) / 4;

    bzero(buffer, sizeof(struct iphdr));
    iphdr = (struct iphdr *) buffer;
    iphdr->ihl = 5;
    iphdr->version = 4;
    iphdr->tot_len = htons(dlength);
    iphdr->id = htons(random());
    iphdr->ttl = IPDEFTTL;
    iphdr->protocol = IPPROTO_TCP;
    iphdr->daddr = dest_addr.sin_addr.s_addr;
    iphdr->saddr = sock_addr.sin_addr.s_addr;

    if(sendto(s, buffer, dlength, 0x0, (struct sockaddr *) &dest_addr, 
            sizeof(struct sockaddr)) != dlength) {
        error("socket input");
    }

    // Recieve part

    if((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        error("recieve socket error");
    }

    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char *)&on, sizeof(on)) < 0) {
        error("sock option error");
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto lap = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = lap - start;
    printf("clock begin %d", elapsed.count());
    bool found = false;

    while(elapsed.count() < 10 && !found) {
        
        if(recvfrom(rs, rbuffer, 65536, 0x0, 
            (struct sockaddr *) &from, &fromlen) > 0) {
                riphdr = (struct iphdr *)(rbuffer + ethlen);

                rtcphdr = (struct tcphdr *)(rbuffer + ethlen + sizeof(struct iphdr));

                printf("Port from return: %d", rtcphdr->dest);

                found = true;
            }

        lap = std::chrono::high_resolution_clock::now();
        elapsed = lap - start;
    }
   
   close(rs);
   close(s);
}


