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

void ProcessPacket(unsigned char* , int);
void print_ip_header(unsigned char* , int);
void PrintData (unsigned char* , int);

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
    socklen_t addr_len;

    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10;

    //args checker
    /*if(argc < 3)
    {
        fprintf(stderr, "usage: talker hostname message\n");
        exit(1);
    }
    */
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
    //made recv non-blocking
    //setsockopt(sockfd2, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
    // host byte order
    their_addr.sin_family = AF_INET;
    // short, network byte order
    their_addr.sin_port = htons(portnum);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    // zero the rest of the struct
    bzero(&(their_addr.sin_zero), 8);

    if((bytes_sent = sendto(sockfd, argv[2], strlen(argv[2]), 0,(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) < 0) {
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
            int saddr_size = sizeof(saddr);
            //printf("recv Open Test\n");
            printf("Port %d is open\n", portnum);
            printf("Recvfrom error, Failed to get the packets\n");
            //perror("recvfrom");
            //close(sockfd2);
            //exit(1);
            //printf("Port %d is open\n", portnum);
            //return 1;
            continue;
        }
        else {
            //DO SOMETHING MAGIC
            ProcessPacket(buffer , bytes_recv); // HARD COPY TEST
            printf("Port %d is closed\n", portnum);
            /* else {
                printf("recv Closed Test\n");
                printf("Port %d is closed\n", portnum);
                perror("recvfrom");
                printf("got packet from %s\n", inet_ntoa(their_addr.sin_addr));
                printf("packet is %d bytes long\n", bytes_recv);
                buffer[bytes_recv] = '\0';
                printf("packet contains \"%s\"\n", buffer);		
                close(sockfd2);
                exit(1);
            } */
            break;
        }

    }

    close(sockfd);
    close(sockfd2);

    return 0;
}

void ProcessPacket(unsigned char* Buffer, int Size) { //HARD COPY FALL ATH
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)Buffer;
    iphdrlen = iph->ihl*4;
     
    struct udphdr *udph = (struct udphdr*)(Buffer + iphdrlen);
     
    printf("***********************UDP Packet*************************\n");
     
    print_ip_header(Buffer,Size);           
     
    printf("\nUDP Header\n");
    printf("   |-Source Port      : %d\n" , ntohs(udph->source));
    printf("   |-Destination Port : %d\n" , ntohs(udph->dest));
    printf("   |-UDP Length       : %d\n" , ntohs(udph->len));
    printf("   |-UDP Checksum     : %d\n" , ntohs(udph->check));
     
    printf("\n");
    printf("IP Header\n");
    PrintData(Buffer , iphdrlen);
         
    printf("UDP Header\n");
    PrintData(Buffer+iphdrlen , sizeof udph);
         
    printf("Data Payload\n");  
    PrintData(Buffer + iphdrlen + sizeof udph ,( Size - sizeof udph - iph->ihl * 4 ));
     
    printf("\n###########################################################\n");
}

void PrintData (unsigned char* data , int Size)// HARD COPY FALL TAKA UT
{
     
    for(int i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)   //if one line of hex printing is complete...
        {
            printf("         ");
            for(int j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    printf("%c",(unsigned char)data[j]); //if its a number or alphabet
                 
                else printf("."); //otherwise print a dot
            }
            printf("\n");
        } 
         
        if(i%16==0) printf("   ");
            printf(" %02X",(unsigned int)data[i]);
                 
        if( i==Size-1)  //print the last spaces
        {
            for(int j=0;j<15-i%16;j++) printf("   "); //extra spaces
             
            printf("         ");
             
            for(int j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) printf("%c",(unsigned char)data[j]);
                else printf(".");
            }
            printf("\n");
        }
    }
}

void print_ip_header(unsigned char* Buffer, int Size) //HARD COPY FALl
{
    unsigned short iphdrlen;
         
    struct iphdr *iph = (struct iphdr *)Buffer;
    iphdrlen =iph->ihl*4;
     
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;
     
    printf("\n");
    printf("IP Header\n");
    printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
    printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
    printf("   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
    printf("   |-Identification    : %d\n",ntohs(iph->id));
    //fprintf(logfile,"   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
    //fprintf(logfile,"   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
    //fprintf(logfile,"   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
    printf("   |-TTL      : %d\n",(unsigned int)iph->ttl);
    printf("   |-Protocol : %d\n",(unsigned int)iph->protocol);
    printf("   |-Checksum : %d\n",ntohs(iph->check));
    printf("   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
    printf("   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}