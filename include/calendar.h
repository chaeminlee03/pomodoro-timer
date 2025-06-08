#ifndef CALENDAR_H
#define CALENDAR_H

#include <time.h>

// 캘린더 이벤트 구조체
typedef struct {
    char title[100];
    time_t start_time;
    time_t end_time;
    char description[500];
} Event;

// 캘린더 함수 선언
void init_calendar(void);
void display_calendar(int year, int month);
void add_event(Event event);
void remove_event(time_t event_time);
void list_events(int year, int month);
void save_calendar_data(void);
void load_calendar_data(void);

#endif // CALENDAR_H 