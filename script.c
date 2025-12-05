#include <stdio.h> //snprintf, printf, fflush, fsnaf, getchar
#include <stdlib.h> // system, exit
#include <unistd.h> // access
#include <signal.h> //sigaction
#include <sys/stat.h> // mkdir
#include <time.h> //time

int count = 0;
char file1[256];
char file2[256];

void handler(int sig) {
    count++;
    if (count >= 3) {
        char dir[512];
        time_t t = time(NULL); // текущее календарное время
        struct tm tm = *localtime(&t); // возвращает указатель на struct tm
        snprintf(dir, sizeof(dir), "saves/saved_files_%04d%02d%02d_%02d%02d%02d",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                 tm.tm_hour, tm.tm_min, tm.tm_sec);

        mkdir("saves", 0755);
        mkdir(dir, 0755);

        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "cp \"%s\" \"%s\" \"%s\"", file1, file2, dir);
        system(cmd);

        printf("\nFiles saved to %s\n", dir);
        exit(0);
    }
}


int main() {
    struct sigaction sa; // Эта структура задаёт функцию вызывать при получении сигнала, сигналы блокировать внутри обработчика, как обрабатывать сигнал (флаги поведения)
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    printf("Enter first file:");
    scanf("%s", file1);
    printf("Enter second file:");
    scanf("%s", file2);

    if (access(file1, F_OK) != 0){ // F_OK - проверть путь и его доступность
        printf("File %s not found\n", file1);
        fflush(stdout);
        return 1;
    }

    if (access(file2, F_OK) != 0) {
        printf("File %s not found\n", file2);
        fflush(stdout);
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
