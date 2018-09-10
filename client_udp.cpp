#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/ip_icmp.h>   //Provides declarations for icmp header
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header

/* 
Open port sends no replay, closed port will result in ICMP
*/
struct sockaddr_in source,dest;

int main(int argc, char* argv[]) {
    //variables
    int sockfd = 0, sockfd2 = 0, portnum = 0, bytes_sent = 0, bytes_recv = 0;
    struct sockaddr_in their_addr;
    struct hostent *he;
    struct sockaddr saddr;
    //unsigned char *buffer[200];
    unsigned char *buffer = (unsigned char *)malloc(65536); //TEST HARD COPY
    // ATH
    struct icmphdr *rec = (struct icmphdr*) (buffer + sizeof(struct iphdr));
    socklen_t addr_len;

    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10;
    
    

    //args checker
    if(argc < 1)
    {
        fprintf(stderr, "usage: hostname\n");
        exit(1);
    }
    
    //get host info
    if((he = gethostbyname(argv[1])) == NULL) {
        herror("gethostbyname");
        exit(1);
    }
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
    portnum = 31313;
    // host byte order
    their_addr.sin_family = AF_INET;
    // short, network byte order
    their_addr.sin_port = htons(portnum);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    // zero the rest of the struct
    bzero(&(their_addr.sin_zero), 8);

    if((bytes_sent = sendto(sockfd, "\n", 1, 0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) < 0) {
        perror("sendto");
        close(sockfd);
        exit(1);
    }

    printf("Print after sendto()\n");
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
            printf("Received:\ttype %d\tcode %d\n", rec->type, rec->code);
            printf("Port %d is closed\n", portnum);
            break;
        }

    }

    close(sockfd);
    close(sockfd2);

    return 0;
}