#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>


int main(int argc, char* argv[]){

    openlog(NULL, 0, LOG_USER);

    if(argc != 3){
        syslog(LOG_ERR,"Invalid amount of arguments, should be 2, writefile and writestr, got %d args.",(argc-1));
        printf("Inavlid amount of arguments, should be 2, writefile and writestr, got %d args.\n",(argc-1));
    } 

    char* filename = argv[1];
    char* writestr = argv[2];

    syslog(LOG_DEBUG,"Writing %s to %s",writestr, filename);

    FILE *file = fopen(filename,"wb");

    if(file == NULL){
        syslog(LOG_ERR, "Failed opening file: %s, value of errno was: %d\n",filename,errno);
        perror("perror returned");
        syslog(LOG_ERR, "Error opening file is: %s",strerror(errno));
        printf("Failed opening or creating file: %s, errno was %d:%s",filename,errno,strerror(errno));
        fclose(file);
        return 1;
    }

    int ret;
    ret = fwrite(writestr, sizeof(char), sizeof(char)*strlen(writestr),file);
    
    if (ret != sizeof(char)*strlen(writestr)){
        syslog(LOG_ERR, "Error encounterd while writing data. fwrite returned: %d",ret);
        syslog(LOG_ERR, "The errno was: %d : %s\n", errno, strerror(errno));
        fclose(file);
        return 1;
    }

    ret = fwrite("\n", sizeof(char), sizeof(char),file);
    
    if (ret != sizeof(char)*strlen(writestr)){
        syslog(LOG_ERR, "Error encounterd while writing data. fwrite returned: %d",ret);
        syslog(LOG_ERR, "The errno was: %d : %s\n", errno, strerror(errno));
        fclose(file);
        return 1;
    }

    ret = fclose(file);

    if(ret != 0){
        syslog(LOG_ERR, "Error closing file, errno was: %d:%s", errno,strerror(errno));
        return 1;
    }

    syslog(LOG_DEBUG, "succsefully wrote %s to %s", writestr,filename);


}
