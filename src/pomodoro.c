#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../include/pomodoro.h"
#include "../include/utils.h"

#define MAX_SESSIONS 1000
static PomodoroSession sessions[MAX_SESSIONS];
static int session_count = 0;
static PomodoroSettings settings = {
    .work_duration = 25,
    .break_duration = 5,
    .long_break_duration = 15,
    .sessions_before_long_break = 4
};

static PomodoroSession* current_session = NULL;
static int is_paused = 0;
static time_t pause_start_time = 0;

void init_pomodoro(void) {
    load_pomodoro_data();
}

void start_pomodoro_session(const char* task_name) {
    if (current_session != NULL) {
        handle_error("이미 진행 중인 세션이 있습니다.");
        return;
    }

    if (session_count >= MAX_SESSIONS) {
        handle_error("세션 최대 개수를 초과했습니다.");
        return;
    }

    current_session = &sessions[session_count++];
    current_session->start_time = time(NULL);
    current_session->duration = settings.work_duration;
    strncpy(current_session->task_name, task_name, sizeof(current_session->task_name) - 1);
    current_session->completed = 0;
    is_paused = 0;

    log_message("새로운 뽀모도로 세션이 시작되었습니다.");
    display_current_session();  // 시작하자마자 현재 상태를 표시
}

void pause_pomodoro_session(void) {
    if (current_session == NULL || is_paused) {
        handle_error("일시정지할 수 있는 세션이 없습니다.");
        return;
    }

    is_paused = 1;
    pause_start_time = time(NULL);
    log_message("뽀모도로 세션이 일시정지되었습니다.");
}

void resume_pomodoro_session(void) {
    if (current_session == NULL || !is_paused) {
        handle_error("재개할 수 있는 일시정지된 세션이 없습니다.");
        return;
    }

    time_t pause_duration = time(NULL) - pause_start_time;
    current_session->start_time += pause_duration;
    is_paused = 0;
    log_message("뽀모도로 세션이 재개되었습니다.");
}

void end_pomodoro_session(void) {
    if (current_session == NULL) {
        handle_error("종료할 수 있는 세션이 없습니다.");
        return;
    }

    time_t now = time(NULL);
    time_t total_pause_time = 0;
    
    // 일시정지 시간 계산
    if (is_paused) {
        total_pause_time = now - pause_start_time;
        is_paused = 0;
    }
    
    current_session->end_time = now;
    current_session->start_time += total_pause_time;  // 일시정지 시간만큼 시작 시간을 조정
    current_session->completed = 1;
    save_pomodoro_data();
    log_message("뽀모도로 세션이 종료되었습니다.");
    current_session = NULL;
}

void display_current_session(void) {
    if (current_session == NULL) {
        printf("현재 진행 중인 세션이 없습니다.\n");
        return;
    }

    time_t now = time(NULL);
    time_t elapsed = now - current_session->start_time;
    int remaining_minutes = current_session->duration - (elapsed / 60);
    int remaining_seconds = 60 - (elapsed % 60);
    
    // 60초가 되면 분을 증가시키고 초를 0으로 리셋
    if (remaining_seconds == 60) {
        remaining_seconds = 0;
    }

    printf("\n현재 작업: %s\n", current_session->task_name);
    printf("남은 시간: %d분 %d초\n", remaining_minutes, remaining_seconds);
    printf("상태: %s\n", is_paused ? "일시정지" : "진행 중");
}

void save_pomodoro_data(void) {
    FILE* file = fopen("data/pomodoro.dat", "wb");
    if (file) {
        fwrite(&settings, sizeof(PomodoroSettings), 1, file);
        fwrite(&session_count, sizeof(int), 1, file);
        fwrite(sessions, sizeof(PomodoroSession), session_count, file);
        fclose(file);
    } else {
        handle_error("뽀모도로 데이터 저장 실패");
    }
}

void load_pomodoro_data(void) {
    FILE* file = fopen("data/pomodoro.dat", "rb");
    if (file) {
        fread(&settings, sizeof(PomodoroSettings), 1, file);
        fread(&session_count, sizeof(int), 1, file);
        fread(sessions, sizeof(PomodoroSession), session_count, file);
        fclose(file);
    }
}

void update_pomodoro_settings(PomodoroSettings new_settings) {
    settings = new_settings;
    save_pomodoro_data();
    log_message("뽀모도로 설정이 업데이트되었습니다.");
}

int get_session_count(void) {
    return session_count;
}

PomodoroSession get_session(int index) {
    if (index >= 0 && index < session_count) {
        return sessions[index];
    }
    PomodoroSession empty = {0};
    return empty;
}

void display_statistics(void) {
    int total_work_time = 0;
    int total_break_time = 0;
    int completed_sessions = 0;
    int actual_work_time = 0;
    int actual_break_time = 0;

    printf("\n=== 뽀모도로 통계 ===\n");
    printf("총 세션 수: %d\n", session_count);
    
    if (session_count > 0) {
        printf("\n=== 세션 상세 내역 ===\n");
        for (int i = 0; i < session_count; i++) {
            printf("\n세션 %d:\n", i + 1);
            printf("  작업명: %s\n", sessions[i].task_name);
            printf("  작업 시간: %d분\n", sessions[i].duration);
            printf("  완료 여부: %s\n", sessions[i].completed ? "완료" : "미완료");
            
            if (sessions[i].completed) {
                char start_time_str[20];
                char end_time_str[20];
                strftime(start_time_str, sizeof(start_time_str), "%Y-%m-%d %H:%M", localtime(&sessions[i].start_time));
                strftime(end_time_str, sizeof(end_time_str), "%Y-%m-%d %H:%M", localtime(&sessions[i].end_time));
                printf("  시작 시간: %s\n", start_time_str);
                printf("  종료 시간: %s\n", end_time_str);
                
                // 실제 작업 시간 계산 (분 단위)
                int actual_minutes = (sessions[i].end_time - sessions[i].start_time) / 60;
                printf("  실제 작업 시간: %d분\n", actual_minutes);
                actual_work_time += actual_minutes;
                
                completed_sessions++;
                total_work_time += sessions[i].duration;
                
                // 휴식 시간 계산 (일반 휴식 + 긴 휴식)
                int break_time = 0;
                if (completed_sessions % settings.sessions_before_long_break == 0) {
                    break_time = settings.long_break_duration;
                } else {
                    break_time = settings.break_duration;
                }
                total_break_time += break_time;
                actual_break_time += break_time;
            }
        }
    }

    printf("\n=== 전체 통계 ===\n");
    printf("완료된 세션 수: %d\n", completed_sessions);
    printf("계획된 작업 시간: %d분\n", total_work_time);
    printf("계획된 휴식 시간: %d분\n", total_break_time);
    printf("계획된 총 시간: %d분\n", total_work_time + total_break_time);
    printf("\n실제 통계:\n");
    printf("실제 작업 시간: %d분\n", actual_work_time);
    printf("실제 휴식 시간: %d분\n", actual_break_time);
    printf("실제 총 시간: %d분\n", actual_work_time + actual_break_time);
    printf("\n계속하려면 Enter를 누르세요...\n");
    getchar();
} 