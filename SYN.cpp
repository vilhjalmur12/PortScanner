#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include<netinet/ip_icmp.h>   //Provides declarations for icmp header
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<chrono>

#include <iostream>
using namespace std;

#include <map>
#include <unistd.h>

void sendSYN();
void debugFunc() {
    printf("************ DEBUG ************\n");
}


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
    int begin_port = 1025;
    int end_port = 60000;
    unsigned int sleeptime = 500000;
    int portno;

    // Test variables


    // bool hash map for which ports are finished
    std::map<int, bool> port_map;
    int ports_counted = end_port - begin_port;
    //typedef std::map<int, bool>::iterator map_iter;

    /*
    if (argc > 2) {
      begin_port = atoi(argv[2]);
      end_port = atoi(argv[3]);
    }

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
     */

    //portno = end_port;

    sendSYN();

    return 0;
}

unsigned short csum(unsigned short *buf,int nwords)
{
    //this function returns the checksum of a buffer
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--){sum += *buf++;}
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short) (~sum);
}

void sendSYN() {
    /*
    int raw_sock, data_size;
    int saddr_size;
    struct sockaddr saddr;
    struct in_addr in;
    unsigned int addr_len;

    unsigned char *buffer = (unsigned char *)malloc(65536);

    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(raw_sock < 0)
    {
        printf("Socket Error\n");
        exit(-1);
    }

    addr_len = sizeof saddr;
    data_size = recvfrom(raw_sock, buffer, 65536, 0, &saddr, &addr_len);

    debugFunc();

    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *) buffer;
    iphdrlen = iph->ihl*4;
    struct tcphdr *tcph = (struct tcphdr*) (buffer + iphdrlen);

    printf("Source Port: %u\n", tcph->source);
     */

    int destination_port = htons(31313);
    int source_port = htons(random());
    char buffer[65536], rbuffer[65536];
    char string[65536];
    char *host_source, *host_dest;
    int dlength = (sizeof(struct tcphdr) + sizeof(struct iphdr));
    struct hostent *hostentry;

    struct iphdr *iphdr, *riphdr;
    struct tcphdr *tcphdr, *rtcphdr;

    struct sockaddr_in sock_addr, dest_addr;
    struct sockaddr from;
    int ethlen;
    socklen_t fromlen;

    host_source = "10.0.2.15";
    host_dest = "130.208.243.61";

    printf("host_dest: %c", &host_source);
    // get host source
    if((hostentry = gethostbyname(host_source)) == NULL) {
        error("Error with source address");
    }



    bzero(&sock_addr, sizeof(struct sockaddr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr = *((struct in_addr *)hostentry->h_addr);

    cout << "host address: " << &sock_addr.sin_addr.s_addr << "\n";

    printf("host_dest: %c", &host_dest);
    // get host destination
    if((hostentry = gethostbyname(host_dest)) == NULL) {
        error("Error with destination address");
    }

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


void sendSYN2() {


    // init variables
    struct sockaddr_in sock_in;
    unsigned char packetBuf[4096];

    // Createing raw socket
    //int raw_fd = socket(AF_INET, SOCK_RAW, 6);
    int raw_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW); // Open Socket

    if (raw_fd < 0)
    {
        printf("ERROR creating raw socket\n");
        exit(1);
    }else{
        printf("Raw Socket Created!		:-D\n");
    }

    //specify port to bind to
    bzero((char *)& sock_in,sizeof(sock_in));
    sock_in.sin_port = htons(2400);

    /*  Binding procedure - reyna að keyra án þess.
     *

    //bind socket
    int err = bind(raw_fd,(struct sockaddr &)sock_in,sizeof(&sock_in));
    if (err < 0)
    {
        printf("ERROR binding socket.\n");
        exit(1);
    }else{
        printf("Bound socket!			:-D\n");
    }

    //inform os of recieving raw ip packet
    {
        int tmp = 1;
        setsockopt(raw_fd, 0, IP_HDRINCL,&tmp,sizeof(tmp));
    }

    */

    //re-use socket structure
    //Details about where this custom packet is going:
    bzero((char *)& sock_in, sizeof(sock_in));
    sock_in.sin_family = AF_INET;
    sock_in.sin_port = htons(2400);	//port to send packet to
    sock_in.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP to send packet to

    // buffer size
    unsigned short buffer_size = sizeof(struct ip) + sizeof(struct tcphdr);

    struct ip *IPheader = (struct ip *) packetBuf;
    struct tcphdr *TCPheader = (struct tcphdr *) (packetBuf + sizeof (struct ip));

    //Fill out IP Header information:
    IPheader->ip_hl = 5;
    IPheader->ip_v = 4;		    //IPv4
    IPheader->ip_tos = 0;		//type of service
    IPheader->ip_len = htons(buffer_size);	//length
    IPheader->ip_id = htonl(54321);
    IPheader->ip_off = 0;
    IPheader->ip_ttl = 255;	//max routers to pass through
    IPheader->ip_p = 6;		//tcp
    IPheader->ip_sum = 0;	//Set to 0 before calulating later
    IPheader->ip_src.s_addr = inet_addr("123.4.5.6");	//source IP address
    IPheader->ip_dst.s_addr = inet_addr("127.0.0.1");	//destination IP address

    //Fill out TCP Header information:
    TCPheader->th_sport = htons(55000);	//source port
    TCPheader->th_dport = htons(2400);			//destination port
    TCPheader->th_seq = random();
    TCPheader->th_ack = 0;	//Only 0 on initial SYN
    TCPheader->th_off = 0;
    TCPheader->th_flags = TH_SYN;	//SYN flag set
    TCPheader->th_win = htonl(65535);	//used for segmentation
    TCPheader->th_sum = 0;				//Kernel fill this out
    TCPheader->th_urp = 0;


    //Now fill out the checksum for the IPheader
    //IPheader->ip_sum = csum((unsigned short *) packetBuf, IPheader->ip_len >> 1);
    //printf("IP Checksum: ");


    // Createing raw socket
    int sendRaw = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    if (sendRaw < 0)
    {
        printf("ERROR creating raw socket for sending.\n");
        exit(1);
    }else{
        printf("Raw socket created for sending!	:-D\n");
    }
    int sendErr = sendto(sendRaw,packetBuf,
                         sizeof(packetBuf),0,(struct sockaddr *)&sock_in,sizeof(sock_in));



    if (sendErr < sizeof(packetBuf))
    {
        printf("%d out of %d were sent.\n", sendErr, sizeof(packetBuf));
        exit(1);
    }else{
        printf("< %d Sent message!!\n", sendErr);
    }

    printf("Sleep for 2 sec\n");
    sleep(1);
    printf(".\n");
    sleep(1);
    printf(".\n");
    char recvPacket[4096] = "";
    int newData = recv(raw_fd,recvPacket,sizeof(recvPacket),0);


    struct tcphdr *tcph=(struct tcphdr*)(recvPacket + sizeof (struct ip));
    printf("Checking tcphdr: %u\n", tcph->th_flags);
    //printf("Checking tcphdr: %u\n", tcph->);


    if (newData <=0)
    {
        printf("%d returned by recv!\n", newData);
        exit(1);
    }else{
        printf("<%d> RECIEVE SUCCESSFULL!!\n", newData);
    }


}

void sendSYN3(struct sockaddr_in serv_addr, struct sockaddr_in socket_addr ,int portnr) {

    // Initialize header variables
    int SYN = 1;
    int ACK = 0;
    int RST = 0;
    int URG = 0;
    int FIN = 0;
    int PSH = 0;
    int dest_port = htons(portnr);
    int source_port = htons(rand());

    int                 status, i, pid, s;
    int buff_long = 65536;

    char                buffer[buff_long], rbuffer[buff_long];
    char                string[buff_long];
    struct iphdr        *iphdr, *riphdr;
    struct tcphdr       tcphdr, *rtcphdr;
    struct sockaddr     from;
    int                 fromlen, ethlen;

    struct pseudohdr {
        struct in_addr saddr;
        struct in_addr daddr;
        unsigned char zero;
        unsigned char protocol;
        unsigned short length;
    } *pseudoheader;

    //ethlen = sizeof(struct ethhdr);
    int on = 1;
    source_port = htons(random());
    dest_port = htons(dest_port);

    setvbuf(stdout, NULL, _IONBF, 0);
    fflush(stdout);

    if((pid=fork()) == -1)
        error("fork");

    if(setsockopt(s, IPPROTO_IP, IP_HDRINCL,(char *)&on,
                  sizeof(on)) < 0)
        error("setsockopt");

    bzero(buffer, buff_long);

    /*

    // TCP header
    tcphdr =                       (struct tcphdr *)(buffer+
                                                     sizeof(struct iphdr));
    tcphdr->source =               htons(source_port);		// puerto origen
    tcphdr->dest =                 destination_port;			// puerto destino
    tcphdr->window =               htons(65535);			// ventana
    tcphdr->seq =                  random();			// numero de secuencia aleatorio
    tcphdr->syn =                  syn;				// flag SYN
    tcphdr->ack =                  ack;				// flag ACK
    tcphdr->rst =                  rst;				// flag RST
    tcphdr->psh =                  psh;				// flag PSH
    tcphdr->urg =                  urg;				// flag URG
    tcphdr->fin =                  fin;				// flag FIN
    tcphdr->doff =                 sizeof(struct tcphdr) / 4;

*/
     
}
