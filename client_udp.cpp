#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(void)
{
    //SKOÐA BETUR
    struct sockaddr_in serv_addr;
    int sockfd;
    int bytes_sent, bytes_recv;
    char buffer[200];
    socklen_t slen;

    strcpy(buffer, "hello world!");

    // creating a socket using UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        printf("Error Creating Socket");
        exit(EXIT_FAILURE);
    }
    //Zero out socket address
    memset(&serv_addr, 0, sizeof(serv_addr));

    //Adress is ipv4
    serv_addr.sin_family = AF_INET;

    //convert a sting into correct value
    serv_addr.sin_addr.s_addr = inet_addr("skel.ru.is");

    //set port to xxxx
    serv_addr.sin_port = htons(80);


   /*if(sendto(sockfd, buffer, strlen(buffer), 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
   {
       printf("Error sending packet: %s\n", strerror(errno));
       exit(EXIT_FAILURE);
   }

    //Zero out the buffer
    memset(buffer, '\0', strlen(buffer));

   if(recvfrom(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &serv_addr, &slen) < 0)
   {
       printf("Error receving packet: %s\n", strerror(errno));
       exit(EXIT_FAILURE);
   }
   */

  bytes_sent = sendto(sockfd, buffer, strlen(buffer), 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if(bytes_sent < 0)
  {
      printf("Error sending packet: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
  }

  bytes_sent = (recvfrom(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &serv_addr, &slen));
  if(bytes_sent < 0)
  {
      printf("Error receving packet: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
  }

   printf("packet is %d bytes long\n",bytes_sent);
   buffer[bytes_sent] = '\0';
   printf("packet contains \"%s\"\n",buffer);

   close(sockfd);
   return 0;

}