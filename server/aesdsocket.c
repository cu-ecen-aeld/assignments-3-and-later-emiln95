#include <asm-generic/socket.h>
#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <stdbool.h>


#define LIST_PORT "9000"
#define BACKLOG 5
#define BUFFERSIZE 256

int setup_socket(int *sockfd);
int listen_forConnects(int* sockfd, const char* filename);
int return_data(const int socketfd, const char * filename, char* buffer, const int bufLen);
static void term_handler(int signal_number);

bool sigint_caught = false;
bool success = true;

int main(int argc, char *argv[])
{
    openlog(NULL, 0,LOG_USER); 
    syslog(LOG_INFO, "aesdsocket has started.");


    int sockfd = -2, rc;
    
    char* filename="/var/tmp/aesdsocketdata";

    struct sigaction termAction;
    memset(&termAction,0,sizeof(struct sigaction));
    termAction.sa_handler=term_handler;
    rc = sigaction(SIGTERM, &termAction, NULL);
    if( rc != 0){
        syslog(LOG_ERR, "Error setting up SIGTERM handler errno was: %d : %s", errno, strerror(errno));
        return -1;
    }

    rc = sigaction(SIGINT, &termAction, NULL);
    if( rc != 0){
        syslog(LOG_ERR, "Error setting up SIGINT handler errno was: %d : %s", errno, strerror(errno));
        return -1;
    }



    rc = setup_socket(&sockfd);
    if(rc == -1){
        syslog(LOG_ERR, "Error in setup socket. Freeing and closing.");
        if(sockfd != -2){close(sockfd);}
        exit(-1);
    }
    
    if(argc == 2 && strcmp("-d", argv[1])==0){
        rc = fork();
        if(rc == -1){
            syslog(LOG_ERR, "Error encounterd while forking. Errno was: %d : %s, exiting.", errno, strerror(errno));
            success = false;
        }else if(rc == 0){
            syslog(LOG_INFO, "Daemon started.");
        }else{
            syslog(LOG_INFO, "Daemon PID is: %d, parent exiting.", rc);
            close(sockfd);
            exit(0);
        }
    }

    while (!sigint_caught && success){
        rc = listen_forConnects(&sockfd,filename);
        if(rc == -1){
            syslog(LOG_ERR,"Error in listen for connects, freeing and closing");
            close(sockfd);
            exit(-1);
        }
    }

    if(sigint_caught){
        syslog(LOG_INFO, "Caught signal, exiting");
    }

    rc = close(sockfd);
    if(rc != 0){
        syslog(LOG_WARNING, "An error was encounterd while closing the socket. Errno was %d : %s", errno, strerror(errno));
        success = false;
    }

    if (access(filename, F_OK) == 0){
        rc = remove(filename);
        if(rc != 0){
            syslog(LOG_WARNING, "An error was encounterd while deleting file: %s. Errno was %d : %s", filename, errno, strerror(errno));
            success = false;
        }
    }

    if(success){
        syslog(LOG_INFO, "program completed sucessfully, exiting");
        return EXIT_SUCCESS;
    }else{
        syslog(LOG_INFO, "program completed with issues see loggs, exiting");
        perror("Something whent wrong while closing program");
        return -1;
    }

}

int setup_socket(int *sockfd){
    
    struct addrinfo hints;
    struct addrinfo *res = 0;
    int rc;

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    rc = getaddrinfo(NULL, LIST_PORT, &hints, &res);
    if(rc != 0){
        perror("issues in getaddrinfo\n");
        syslog(LOG_ERR, "Error encounterd in getaddrinfo, errno: %d , error: %s", errno, strerror(errno));
        freeaddrinfo(res);
        return -1;
    }

    *sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(*sockfd == -1){
        syslog(LOG_ERR, "Error opening socket, errno: %d, error: %s", errno, strerror(errno));
        perror("issue getting socket");
        freeaddrinfo(res);
        return -1;
    }

    rc = setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    if (rc != 0 ){
        syslog(LOG_ERR, "Error setting socket options, errno: %d, error: %s", errno, strerror(errno));
        perror("issue setting socket options");
        freeaddrinfo(res);
        return -1;
    }

    rc = bind(*sockfd, res->ai_addr, res->ai_addrlen);
    if (rc != 0){
        syslog(LOG_ERR, "Error binding socket, errno: %d, error: %s", errno, strerror(errno));
        perror("issue binding socket");
        freeaddrinfo(res);
        return -1;
    }


    rc = listen(*sockfd, BACKLOG);
    if (rc != 0){
        syslog(LOG_ERR, "Error starting listen, errno: %d, error: %s", errno, strerror(errno));
        perror("issue listening with socket");
        freeaddrinfo(res);
        return -1;
    }
    struct sockaddr_in *addr;
    addr = (struct sockaddr_in *) res->ai_addr;
    syslog(LOG_INFO, "aesdsocket is listening in interface %s and port %s",inet_ntoa((struct in_addr) addr->sin_addr), LIST_PORT);
    freeaddrinfo(res);

    return 0; 
}

int listen_forConnects(int* sockfd, const char* filename){

    socklen_t addr_size;
    int new_fd = -1, rc = -1;
    struct sockaddr connector_address;

    char buffer[BUFFERSIZE] = {0};
    int filrewriteSize = BUFFERSIZE;

    addr_size = sizeof(connector_address);

    new_fd = accept(*sockfd, &connector_address, &addr_size);
    if(new_fd == -1 && sigint_caught == false){
        syslog(LOG_ERR, "Error accepting socket, errno: %d, error: %s", errno, strerror(errno));
        perror("issue accepting socket");
        return -1;
    }else if(new_fd == -1 && sigint_caught == true){
        syslog(LOG_INFO, "accept was interupted by SIGINT or SIGTERM. Shuting down.");
        return 0;
    }
    
    struct sockaddr_in *addr;
    addr = (struct sockaddr_in*) &connector_address;
    syslog(LOG_INFO, "Accepted connection from %s",inet_ntoa((struct in_addr) addr->sin_addr));
   
    FILE *file = fopen(filename,"ab");

    if(file == NULL){
        syslog(LOG_ERR, "Failed opening file: %s, value of errno was: %d\n",filename,errno);
        perror("perror returned");
        syslog(LOG_ERR, "Error opening file is: %s",strerror(errno));
        printf("Failed opening or creating file: %s, errno was %d:%s",filename,errno,strerror(errno));
        close(new_fd);
        fclose(file);
        return -1;
    }


    while (filrewriteSize == BUFFERSIZE && buffer[BUFFERSIZE - 1] != '\n'){
        rc = recv(new_fd,buffer,BUFFERSIZE, 0);
        if(rc == -1){
            syslog(LOG_ERR, "Error recieving on socket, errno: %d, error: %s", errno, strerror(errno));
            perror("issue recieving socket");
            close(new_fd);
            fclose(file);
            return -1;
        }else{
            filrewriteSize = rc;
        }

        rc = fwrite(buffer, sizeof(char), sizeof(char)*filrewriteSize,file);
        if (rc != sizeof(char)*filrewriteSize){
            syslog(LOG_ERR, "Error encounterd while writing data to file: %s. fwrite returned: %d",filename,rc);
            syslog(LOG_ERR, "The errno was: %d : %s\n", errno, strerror(errno));
            close(new_fd);
            fclose(file);
            return -1;
        }
    }
    
    fclose(file);

    rc = return_data(new_fd, filename, buffer, BUFFERSIZE);
    if(rc == -1){
        syslog(LOG_ERR,"Error encounterd while returning data.");
        close(new_fd);
        return -1;
    }

    close(new_fd);
    syslog(LOG_INFO, "Closed connection from %s",inet_ntoa((struct in_addr) addr->sin_addr));
    return 0;
}

int return_data(const int socketfd, const char * filename, char* buffer, const int bufLen){

    int rc=0, readNum=bufLen;

    syslog(LOG_INFO, "Returning Data");

    FILE *file = fopen(filename,"r");

    if(file == NULL){
        syslog(LOG_ERR, "Failed opening file: %s, value of errno was: %d\n",filename,errno);
        perror("perror returned");
        syslog(LOG_ERR, "Error opening file is: %s",strerror(errno));
        printf("Failed opening or creating file: %s, errno was %d:%s",filename,errno,strerror(errno));
        fclose(file);
        return -1;
    }

    int itter = 0;
    while(readNum == bufLen){
        
        readNum = fread(buffer, sizeof(char), sizeof(char)*bufLen, file);
        if(readNum==0){
            syslog(LOG_ERR,"Failed when reading file: %s, on itteration: %d, errno was: %d : %s",filename,itter,errno, strerror(errno));
            fclose(file);
            return -1;
        }
        
        rc = send(socketfd, buffer, sizeof(char)*readNum, 0);
        if(rc == -1){
            syslog(LOG_ERR, "Error sending bytes, errno was: %d : %s", errno, strerror(errno));
            fclose(file);
            return -1;
        }
        itter++;
    }

    fclose(file);   

    return 0;
}


static void term_handler(int signal_number){
    if (signal_number == SIGINT || signal_number == SIGTERM){
        sigint_caught = true;
    }
}
