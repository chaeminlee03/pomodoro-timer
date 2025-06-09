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

    current_session->end_time = time(NULL);
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