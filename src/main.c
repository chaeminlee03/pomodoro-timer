#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "../include/calendar.h"
#include "../include/pomodoro.h"
#include "../include/utils.h"
#define _XOPEN_SOURCE 700
void handle_calendar_view(void) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    int year = timeinfo->tm_year + 1900;
    int month = timeinfo->tm_mon + 1;

    printf("연도 입력 (현재: %d): ", year);
    int input_year;
    if (scanf("%d", &input_year) == 1) {
        year = input_year;
    }
    while (getchar() != '\n');

    printf("월 입력 (현재: %d): ", month);
    int input_month;
    if (scanf("%d", &input_month) == 1 && input_month >= 1 && input_month <= 12) {
        month = input_month;
    }
    while (getchar() != '\n');

    display_calendar(year, month);
}

void handle_add_event(void) {
    Event event;
    printf("이벤트 제목: ");
    fgets(event.title, sizeof(event.title), stdin);
    event.title[strcspn(event.title, "\n")] = 0;

    printf("시작 날짜 (YYYY-MM-DD HH:MM): ");
    char date_str[100];
    fgets(date_str, sizeof(date_str), stdin);
    date_str[strcspn(date_str, "\n")] = 0;

    struct tm timeinfo = {0};
    strptime(date_str, "%Y-%m-%d %H:%M", &timeinfo);
    event.start_time = mktime(&timeinfo);

    printf("종료 날짜 (YYYY-MM-DD HH:MM): ");
    fgets(date_str, sizeof(date_str), stdin);
    date_str[strcspn(date_str, "\n")] = 0;

    strptime(date_str, "%Y-%m-%d %H:%M", &timeinfo);
    event.end_time = mktime(&timeinfo);

    printf("설명: ");
    fgets(event.description, sizeof(event.description), stdin);
    event.description[strcspn(event.description, "\n")] = 0;

    add_event(event);
}

void handle_remove_event(void) {
    printf("삭제할 이벤트의 시작 시간 (YYYY-MM-DD HH:MM): ");
    char date_str[100];
    fgets(date_str, sizeof(date_str), stdin);
    date_str[strcspn(date_str, "\n")] = 0;

    struct tm timeinfo = {0};
    strptime(date_str, "%Y-%m-%d %H:%M", &timeinfo);
    time_t event_time = mktime(&timeinfo);

    remove_event(event_time);
}

void handle_pomodoro_start(void) {
    char task_name[100];
    printf("작업 이름: ");
    fgets(task_name, sizeof(task_name), stdin);
    task_name[strcspn(task_name, "\n")] = 0;

    start_pomodoro_session(task_name);

    // 타이머 카운트다운
    printf("\n뽀모도로 타이머 시작!\n");
    int minutes = 0;
    int total = get_session_count();
    PomodoroSession session = get_session(total - 1);
    int duration = session.duration;
    int is_paused = 0;
    time_t start_time = time(NULL);
    time_t pause_start = 0;
    int paused_minutes = 0;
    int remaining_seconds = 0;  // 1분 내 남은 초

    for (minutes = duration; minutes > 0; minutes--) {
        remaining_seconds = 60;  // 매 분마다 60초로 초기화
        
        while (remaining_seconds > 0) {
            if (kbhit()) {  // 키 입력 체크
                char key = getchar();
                if (key == 'p' || key == 'P') {
                    if (!is_paused) {
                        is_paused = 1;
                        pause_start = time(NULL);
                        char time_str[100];
                        time_t current_time = time(NULL);
                        struct tm* timeinfo = localtime(&current_time);
                        strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
                        printf("\n[%s] 타이머가 일시정지되었습니다. (재개: p, 종료: q)\n", time_str);
                        pause_pomodoro_session();
                        
                        // 일시정지 상태에서 대기
                        while (is_paused) {
                            if (kbhit()) {
                                key = getchar();
                                if (key == 'p' || key == 'P') {
                                    is_paused = 0;
                                    time_t pause_duration = time(NULL) - pause_start;
                                    paused_minutes += pause_duration / 60;
                                    current_time = time(NULL);
                                    timeinfo = localtime(&current_time);
                                    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
                                    printf("\n[%s] 타이머가 재개되었습니다.\n", time_str);
                                    resume_pomodoro_session();
                                    break;
                                } else if (key == 'q' || key == 'Q') {
                                    printf("\n[%s] 타이머가 종료되었습니다.\n", time_str);
                                    end_pomodoro_session();
                                    return;
                                }
                            }
                            usleep(100000);  // 0.1초 대기
                        }
                    }
                }
            }
            
            if (!is_paused) {
                sleep(1);
                remaining_seconds--;
                
                // 매 분마다 시간 표시
                if (remaining_seconds == 0) {
                    char time_str[100];
                    time_t current_time = time(NULL);
                    struct tm* timeinfo = localtime(&current_time);
                    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
                    display_current_session();
                    fflush(stdout);
                }
            } else {
                usleep(100000);  // 0.1초 대기
            }
        }
    }

    char time_str[100];
    time_t end_time = time(NULL);
    struct tm* timeinfo = localtime(&end_time);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
    printf("[%s] 남은 시간: 0분\n", time_str);
    end_pomodoro_session();
    printf("[%s] 뽀모도로 세션이 종료되었습니다!\n", time_str);
    
    // 세션 요약 출력
    time_t total_time = end_time - start_time;
    printf("\n=== 세션 요약 ===\n");
    printf("시작 시간: %s\n", time_str);
    printf("총 작업 시간: %ld분\n", total_time / 60);
    printf("일시정지 시간: %d분\n", paused_minutes);
}

void handle_pomodoro_settings(void) {
    PomodoroSettings new_settings;
    printf("작업 시간 (분, 기본값: 25): ");
    scanf("%d", &new_settings.work_duration);
    while (getchar() != '\n');

    printf("휴식 시간 (분, 기본값: 5): ");
    scanf("%d", &new_settings.break_duration);
    while (getchar() != '\n');

    printf("긴 휴식 시간 (분, 기본값: 15): ");
    scanf("%d", &new_settings.long_break_duration);
    while (getchar() != '\n');

    printf("긴 휴식 전 세션 수 (기본값: 4): ");
    scanf("%d", &new_settings.sessions_before_long_break);
    while (getchar() != '\n');

    update_pomodoro_settings(new_settings);
}

void handle_statistics(void) {
    display_statistics();
}

int main(void) {
    create_data_directory();
    init_calendar();
    init_pomodoro();

    while (1) {
        clear_screen();
        print_header("뽀모도로 캘린더");
        print_menu();

        int choice = get_user_input();

        switch (choice) {
            case 0:
                printf("프로그램을 종료합니다.\n");
                return 0;
            case 1:
                handle_calendar_view();
                break;
            case 2:
                handle_add_event();
                break;
            case 3:
                handle_remove_event();
                break;
            case 4:
                handle_pomodoro_start();
                break;
            case 5:
                handle_pomodoro_settings();
                break;
            case 6:
                handle_statistics();
                break;
            default:
                printf("잘못된 선택입니다.\n");
        }

        printf("\n계속하려면 Enter를 누르세요...");
        while (getchar() != '\n');
    }

    return 0;
} 
