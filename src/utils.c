#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include "../include/utils.h"

void clear_screen(void) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void print_header(const char* title) {
    printf("\n=== %s ===\n\n", title);
}

void print_menu(void) {
    printf("1. 캘린더 보기\n");
    printf("2. 이벤트 추가\n");
    printf("3. 이벤트 삭제\n");
    printf("4. 뽀모도로 시작\n");
    printf("5. 뽀모도로 설정\n");
    printf("6. 통계 보기\n");
    printf("0. 종료\n");
    printf("\n선택: ");
}

int get_user_input(void) {
    int input;
    scanf("%d", &input);
    while (getchar() != '\n'); // 입력 버퍼 비우기
    return input;
}

void format_time(time_t time, char* buffer, size_t buffer_size) {
    struct tm* timeinfo = localtime(&time);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M", timeinfo);
}

void create_data_directory(void) {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        mkdir("data", 0700);
    }
}

void log_message(const char* message) {
    FILE* log_file = fopen("data/app.log", "a");
    if (log_file) {
        time_t now = time(NULL);
        char time_str[100];
        format_time(now, time_str, sizeof(time_str));
        fprintf(log_file, "[%s] %s\n", time_str, message);
        fclose(log_file);
    }
}

void handle_error(const char* error_message) {
    fprintf(stderr, "오류: %s\n", error_message);
    log_message(error_message);
}

int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
} 