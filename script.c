#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <time.h>

int count = 0;
char file1[256];
char file2[256];

void handler(int sig) {
    count++;
    if (count >= 3) {
        char dir[512];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        snprintf(dir, sizeof(dir), "saves/saved_files_%04d%02d%02d_%02d%02d%02d",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                 tm.tm_hour, tm.tm_min, tm.tm_sec);

        // Создаём папку saves, если не существует
        mkdir("saves", 0755);
        mkdir(dir, 0755);

        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "cp \"%s\" \"%s\" \"%s\"", file1, file2, dir);
        system(cmd);

        printf("\nFiles saved to %s\n", dir);
        exit(0);
    }
}

int file_exists(const char *filename) {
    struct stat st;
    return (stat(filename, &st) == 0);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    printf("Enter first file:");
    scanf("%s", file1);
    printf("Enter second file:");
    scanf("%s", file2);

    if (access(file1, F_OK) != 0){
        printf("File %s not found\n", file1);
        fflush(stdout);
    }

    if (access(file2, F_OK) != 0) {
        printf("File %s not found\n", file2);
        fflush(stdout);
    }


    if (!file_exists(file1) && !file_exists(file2)) {
        printf("Both files not found\n");
        return 1;
    } else if (!file_exists(file1)) {
        printf("First file not found\n");
        return 1;
    } else if (!file_exists(file2)) {
        printf("Second file not found\n");
        return 1;
    }

    char command[1024];
    snprintf(command, sizeof(command), "cmp \"%s\" \"%s\"", file1, file2);
    FILE* fp = popen(command, "r");
    if (!fp) {
        perror("popen failed");
        return 1;
    }
    int result = pclose(fp);
    if (result) {
        printf("Files are different.\n");
    }
    else {
        printf("Files are the same.\n");
    }

    snprintf(command, sizeof(command), "du \"%s\"", file1);
    FILE* fp2 = popen(command, "r");
    if (!fp2) {
        perror("popen failed");
        return 1;
    }
    long size;
    fscanf(fp2, "%ld", &size);
    printf("%s: %ld\n", file1, size);
    pclose(fp2);

    snprintf(command, sizeof(command), "du \"%s\"", file2);
    FILE* fp3 = popen(command, "r");
    if (!fp3) {
        perror("popen failed");
        return 1;
    }
    long size2;
    fscanf(fp3, "%ld", &size2);
    printf("%s: %ld\n", file2, size2);
    pclose(fp3);

    printf("Press Ctrl+C 3 times to save, or press Enter to exit...\n");

    getchar();
    getchar();

    return 0;
}
