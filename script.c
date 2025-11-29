#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>

int count = 0;
char file1[512], file2[512];

void handler(int signum){
    count++;
}

void handler_logic(){
    if(strlen(file1) > 0 && strlen(file2) > 0){
        char answer;
        printf("Save files? (y/n):");
        scanf("%c", &answer);
        if (answer == 'y'){
            char dir[256];
            snprintf(dir, sizeof(dir), "saves/saved_files_%lld", time(NULL));
            mkdir(dir);

            char command[1024];
            snprintf(command, sizeof(command), "cp \"%s\" \"%s\"", file1, dir);
            system(command);
            snprintf(command, sizeof(command), "cp \"%s\" \"%s\"", file2, dir);
            system(command);

            printf("Files saved.\n");
        }
    }
}

long countBlocks(const char *file, int block_size) {
    struct stat st;
    if (stat(file, &st) != 0) return 0;
    long size = st.st_size;
    return (size + block_size - 1) / block_size;
}

int main(){
    signal(SIGINT, handler);
    printf("Enter first file:");
    fflush(stdout);
    scanf("%s", file1);
    printf("Enter second file:");
    fflush(stdout);
    scanf("%s", file2);

    if (access(file1, F_OK) != 0){
        printf("File %s not found\n", file1);
        fflush(stdout);
    }


    if (access(file2, F_OK) != 0) {
        printf("File %s not found\n", file2);
        fflush(stdout);
    }

    char command[1024];
    snprintf(command, sizeof(command), "fc /c \"%s\" \"%s\"", file1, file2);
    FILE* fp = popen(command, "r");
    if(!fp){
        perror("popen failed.");
        return 1;
    }
    int result = pclose(fp);
    if(result) {
        printf("Files are different.\n");
        fflush(stdout);
    }
    else {
        printf("Files are identical.\n");
        fflush(stdout);
    }

    printf("Place taken:\n");
    printf("%s %ld blocks\n", file1, countBlocks(file1, 512));
    printf("%s %ld blocks\n", file2, countBlocks(file2, 512));

    printf("Press Ctrl+C 3 times to save, press anything else to exit...\n");
    fflush(stdout);

    while (1){
        if(count >= 3){
            handler_logic();
            break;
        }

    }
}