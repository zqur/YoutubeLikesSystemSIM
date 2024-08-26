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

void ParentProcess();
void LikesServer(int likeServerNum);

//struct to hold LikeServer name and # of likes while running
typedef struct {
    char name[256];
    int likes;
} LikesServerData;

int main() {
    ParentProcess();
    return 0;
}

void ParentProcess(){
    // Create a log file.
    FILE *fp = fopen("/tmp/ParentProcessStatus.log", "w");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    //code for timestamp used to write to file
    char *timestamp() {
        time_t rawtime;
        struct

            tm *timeinfo;

        char buffer[80];
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        //format str from time
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
        return strdup(buffer);
    }

    //array to hold child pids
    pid_t child_pids[10];

    //fork 10 LikesServers
    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // This is the child process.
            // Write to log
            fprintf(fp, "%s: Started LikesServer%d\n", timestamp(), i);
            LikesServer(i);
	        exit(0);
        } else if (pid > 0) {
            // This is the parent process.
            // record pid, sleep 1 second between creating child processes
            child_pids[i] = pid;
            sleep(1);
        } else {
            // Fork failed.
            perror("fork");
            exit(1);
        }

    }

    //wait for child process' to finish and write to log
    for (int i = 0; i < 10; i++){
        int status;
        waitpid(child_pids[i], &status, 0);
        //child exits normally
        if (WIFEXITED(status)){
            fprintf(fp, "%s: Ended LikesServer%d ExitCode:%d\n", timestamp(), i, 0);

        }
    }
    // Close the log file.
    fclose(fp);

    // Exit.
    exit(0);
}

void LikesServer(int likeServerNum) {
    //srand for generating random nums (likes)
    srand((unsigned int)time(NULL));

    //save start time of child process, timeout is 5 mins (300s)
    time_t start_time = time(NULL);
    time_t timeout = 300;
    time_t elapsed_time = 0;

    //elapsed time checked on iteration, exit when >= 5 mins (300s)
    while(elapsed_time < timeout){

        //generate random num of likes 1-42 and a random interval for sleep()
    	int likes = rand() % 43;
    	int interval = rand() % 5 + 1;

        //to save path+name of LikesServer for log file
    	char currServPath[256];
        //to save return message from PrimaryLikesServer
    	char server_message[256];

        //format str with the Name of current like server (the #)
    	snprintf(currServPath, sizeof(currServPath), "/tmp/LikeServer%d.log", likeServerNum);

    	//clean buffer for server msg each iteration
    	memset(server_message, '\0', sizeof(server_message));

        //struct for socket
    	struct sockaddr_in servaddr = {0};

        //create UDP socket
    	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    	if (sockfd == -1) {
       	    perror("Failed to create socket");
            exit(EXIT_FAILURE);
        }

        //initialize address of host
    	servaddr.sin_family = AF_INET;
    	servaddr.sin_port = htons(12345);
    	servaddr.sin_addr.s_addr = INADDR_ANY;

    	LikesServerData data;
        //save current LikeServer Name
    	char serverName[256];
    	snprintf(serverName, sizeof(serverName), "LikesServer%d", likeServerNum);

        //copy name and # likes to struct obj
        strcpy(data.name, serverName);
    	data.likes = likes;

        //char buff to serialize struct obj to send over socket
    	char sendBuff[sizeof(LikesServerData)];
    	memcpy(sendBuff, &data, sizeof(LikesServerData));

        //send to PrimaryLikesServer
    	int send_data = sendto(sockfd, sendBuff, sizeof(sendBuff), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    	if (send_data < 0) {
            perror("send");
            exit(1);
    	}

    	socklen_t addr_len = sizeof(servaddr);
        //recieve message from PrimaryLikesServer
    	if (recvfrom(sockfd, server_message, sizeof(server_message), 0, (struct sockaddr *)&servaddr, &addr_len) < 0) {
            perror("Error receiving server msg.");
            exit(1);
    	}
    	FILE *fp = fopen(currServPath, "w");
    	if (fp == NULL) {
            perror("fopen");
            exit(1);
    	}
        //write message to log of LikeServer#
    	fprintf(fp, "LikeServer%d %d %s\n", likeServerNum, likes, server_message);
    	fclose(fp);

        //close socket, reset running Likes count, update elapsed time
    	close(sockfd);
    	likes = 0;
    	elapsed_time = time(NULL) - start_time;
        //sleep for random interval between 1-5s
    	sleep(interval);
    }
    //exit when 5 minutes has elapsed
    exit(0);
}
