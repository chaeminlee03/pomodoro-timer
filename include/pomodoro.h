#ifndef POMODORO_H
#define POMODORO_H

#include <time.h>

// 뽀모도로 세션 구조체
typedef struct {
    time_t start_time;
    time_t end_time;
    int duration;  // 분 단위
    char task_name[100];
    int completed;
} PomodoroSession;

// 뽀모도로 설정 구조체
typedef struct {
    int work_duration;    // 기본 25분
    int break_duration;   // 기본 5분
    int long_break_duration;  // 기본 15분
    int sessions_before_long_break;  // 기본 4회
} PomodoroSettings;

// 뽀모도로 함수 선언
void init_pomodoro(void);
void start_pomodoro_session(const char* task_name);
void pause_pomodoro_session(void);
void resume_pomodoro_session(void);
void end_pomodoro_session(void);
void display_current_session(void);
void save_pomodoro_data(void);
void load_pomodoro_data(void);
void update_pomodoro_settings(PomodoroSettings new_settings);

// 세션 접근 함수
int get_session_count(void);
PomodoroSession get_session(int index);

#endif // POMODORO_H 