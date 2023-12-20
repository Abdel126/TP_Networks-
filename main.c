#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


//#define PORT "69"  // TFTP port number
#define BUFSIZE 1024 // Size of the buffer
#define PORT "1069" // Port open on the server
#define MAXDATA 512 // Maximum size of a packet



int main(int argc, char *argv[]) {
    int so;
    struct addrinfo hints;
    struct addrinfo *server_info;
    struct addrinfo *current;
    socklen_t slen;
    int sockfd;
    int message_length;
    char *buf;
    struct sockaddr_in addr;
    int block_size_value = 512;
    char *block_size;
    char *mode = "octet";

    // Retrieve command-line arguments
    char *server = argv[2];  // server IP address
    char *file = argv[3];    // file name

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    // Get server information
    if ((so = getaddrinfo(server, PORT, &hints, &server_info)) != 0)
    //0 (indicating success or no error) or a different value indicating a specific error if the address resolution fails."
    {
        perror("Client: 'Getaddrinfo' Error\n");
        exit(EXIT_FAILURE);
    }



    // Create socket and connect to the server
    for (current = server_info; current != NULL; current = current->ai_next)
    {
        if ((sockfd = socket(current->ai_family, current->ai_socktype, current->ai_protocol)) == -1) {
            perror("Client: 'socket' Error\n");
            continue;
        }
        break;
    }

    // TFTP-related section
    if (strcmp(argv[1], "gettftp") == 0) {
        // Constructing a Read Request (RRQ) and sending it to the server
        char *RRQ;
        int len = strlen(file) + strlen(mode) + 4;

        if (argc == 6) {
            if (strcmp(argv[4], "-blocksize") == 0) {
                len = strlen(file) + strlen(mode) + 4 + 2 + strlen("blksize") + strlen(block_size);
            }
        }

        RRQ = malloc(len);
        RRQ[0] = 0;
        RRQ[1] = 1;  // Opcode
        strcpy(RRQ + 2, file);
        RRQ[2 + strlen(file)] = 0;
        strcpy(RRQ + 3 + strlen(file), mode);
        RRQ[3 + strlen(file) + strlen(mode)] = 0;

        if (argc == 6) {
            if (strcmp(argv[4], "-blocksize") == 0) {
                strcpy(RRQ + 4 + strlen(file) + strlen(mode), "blksize");
                RRQ[4 + strlen(file) + strlen(mode) + strlen("blksize")] = 0;
                strcpy(RRQ + 5 + strlen(file) + strlen(mode) + strlen("blksize"), block_size);
                RRQ[5 + strlen(file) + strlen(mode) + strlen("blksize") + strlen(block_size)] = 0;
            }
        }

        // Sending the RRQ to the server
        if ((message_length = sendto(sockfd, RRQ, len, 0, (struct sockaddr *)current->ai_addr, current->ai_addrlen)) == -1) {
            perror("Client: 'sendto RRQ' Error\n");
            exit(EXIT_FAILURE);
        }

        printf("CLIENT: RRQ of %d bytes sent to the server\n", message_length);

        // Receiving a file
        int data_received;
        FILE *downloaded_file = fopen(file, "wb");  // Create a file

        do {
            buf = malloc(block_size_value + 4);

            if ((message_length = recvfrom(sockfd, buf, block_size_value + 4, 0, (struct sockaddr *)&addr, &slen)) == -1) {
                perror("Client: 'recvfrom' Error\n");
                exit(EXIT_FAILURE);
            }

            buf[message_length] = '\0';

            // Write the downloaded file
            if (*(buf + 1) != 6) {
                printf("CLIENT: Received packet #%d of %d bytes\n", *(buf + 3), message_length - 4);
                data_received = message_length - 4;
                fwrite(buf + 4, sizeof(char), data_received, downloaded_file);
            }

            // Acknowledgment
            u_int16_t *ACK;
            len = 4;
            ACK = malloc(len);
            *ACK = htons((u_int16_t)4);            // Opcode
            *(ACK + 1) = htons((u_int16_t)*(buf + 3));  // Block

            // Sending acknowledgment
            if (*(buf + 1) == 6) {
                *(ACK + 1) = htons((u_int16_t)0);  // Block

                if ((sendto(sockfd, ACK, len, 0, (struct sockaddr *)&addr, slen)) == -1) {
                    perror("Client: 'sendto ACK 0' Error\n");
                    exit(EXIT_FAILURE);
                }

                printf("CLIENT: Acknowledgment for packet #0\n");
                data_received = block_size_value;
            } else {
                if ((sendto(sockfd, ACK, len, 0, (struct sockaddr *)&addr, slen)) == -1) {
                    perror("Client: 'sendto ACK' Error\n");
                    exit(EXIT_FAILURE);
                }

                printf("CLIENT: Acknowledgment for packet #%d\n", *(buf + 3));
            }
        } while ((data_received == block_size_value));

        fclose(downloaded_file);
        printf("CLIENT: Transfer completed\n");
    }

    freeaddrinfo(server_info);
    close(sockfd);

    return 0;
}












/*


#define MAX_SIZE 512


int main(int argc,char *argv[])
{
    char* hostname=argv[1];
    char* filename=argv[2];
    //Q2
    struct addrinfo host, *res;
    memset(&host,0,sizeof host);

    int status = getaddrinfo(hostname,"1069",&host,&res);
    host.ai_family=AF_UNSPEC; //Allow IPv4 ou IPv6
    host.ai_socktype=SOCK_DGRAM;
    host.ai_flags=AI_PASSIVE;
    host.ai_protocol=0;
    host.ai_canonname=NULL;
    host.ai_addr=NULL;
    host.ai_next=NULL;
    if(status != 0)
    {
        fprintf(stderr,"getadrrinfo:  %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }else
    {
        fprintf(stderr,"getaddrinfo: ok\n");
    }
    if(res==NULL)
    {
        fprintf(stderr, "Connection can't be established\n");
        exit(EXIT_FAILURE);
    }else
    {
        fprintf(stderr, "connection established: ok\n");
    }
    int sock=socket(res->ai_family,res-> ai_socktype,res->ai_protocol);
    if(sock==-1)
    {
        fprintf(stderr, "Creation Error\n");
        exit(EXIT_FAILURE);
    }
    //Q4)
    char RRQ[MAX_SIZE];
    RRQ[0]=0;
    RRQ[1]=1;

    strcpy(&RRQ[2],filename);
    RRQ[2+strlen(filename)]=0;
    RRQ[8+strlen(filename)]=0;
    int length_RRQ=9+strlen(filename);
    if(!sendto(sock,RRQ,length_RRQ,0,res->ai_addr, res->ai_addrlen))
    {
        fprintf(stderr,"ERROR\n");
    }
}
*/