/* project: 02
 * author: Zain Qureshi
 * desc: YouTube likes server model for eventual consistency using UDP sockets to send and receive data.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>

//struct to save data from LikesServer (name and # of likes recieved)
typedef struct {
    char name[256];
    int likes;
} LikesServerData;

int main(){
    struct sockaddr_in servaddr = {0};

    //initialize UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1){
        perror("failed to create socket");
        exit(1);
    }

    //initialize address of host
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    //bind to socket
    int rc = bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr));
    if (rc == -1){
        perror("failed to bind");
        close(sockfd);
        exit(1);
    }

    //open PrimaryLikesLog to log likes data and total during runtime
    FILE *fp = fopen("/tmp/PrimaryLikesLog.log", "w");
    if (fp == NULL){
        perror("fopen");
        exit(1);
    }
    int total_likes = 0;

    //for formatting log with labels for each transaction
    int transactionNum = 0;
    char transactionLabel = 'a';

    //run indefinitely until terminated from terminal (ctrl+c)
    while(1){

        socklen_t addr_len = sizeof(servaddr);
        LikesServerData recData;

        //buffer to receive serialized obj from socket
        char recBuff[sizeof(LikesServerData)];
        
        int n = recvfrom(sockfd, &recBuff, sizeof(recBuff), 0, (struct sockaddr *)&servaddr, &addr_len);
        if (n < 0){
            perror("Error reading from socket");
            //log bad data
            fprintf(fp, "ERROR: bad data\n");
            //close socket and exit
            close(sockfd);
            exit(1);
        }
        //copy data to LikeServerData object from buffer
        memcpy(&recData, recBuff, sizeof(LikesServerData));
        //update total likes
        total_likes += recData.likes;

        //increment transaction #, this is just for the Log format using a-z as labels for each transaction received
        transactionNum++;

        //write to log with Client, Likes Received and current Total Likes
        fprintf(fp, "%c. Client: %s Likes: %d\n Total: %d\n", transactionLabel, recData.name, recData.likes, total_likes);

        //to loop alphabet
        if (transactionNum == 26){
            transactionNum = 0;
            transactionLabel = 'a';
        }else{
            //next letter
            transactionLabel++;
        }

        //buffer to send message back to LikesServer indicating successful receive
        char buffer[256];
        strcpy(buffer, "0");
        int send_result = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&servaddr, addr_len);
        if (send_result < 0){
            perror("send");
            exit(1);
        }

    }
    //close log and socket
    fclose(fp);
    close(sockfd);


}
