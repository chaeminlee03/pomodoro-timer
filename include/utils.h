#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// 유틸리티 함수 선언
void clear_screen(void);
void print_header(const char* title);
void print_menu(void);
int get_user_input(void);
void format_time(time_t time, char* buffer, size_t buffer_size);
void create_data_directory(void);
void log_message(const char* message);
void handle_error(const char* error_message);
int kbhit(void);

#endif // UTILS_H 