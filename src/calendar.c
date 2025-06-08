#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/calendar.h"
#include "../include/utils.h"

#define MAX_EVENTS 1000
static Event events[MAX_EVENTS];
static int event_count = 0;

void init_calendar(void) {
    load_calendar_data();
}

int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int get_days_in_month(int year, int month) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year)) return 29;
    return days[month - 1];
}

void display_calendar(int year, int month) {
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = 1;
    mktime(&timeinfo);

    int first_day = timeinfo.tm_wday;
    int days_in_month = get_days_in_month(year, month);

    printf("\n     %d년 %d월\n", year, month);
    printf("일 월 화 수 목 금 토\n");

    // 첫 주의 시작 부분을 공백으로 채움
    for (int i = 0; i < first_day; i++) {
        printf("   ");
    }

    // 날짜 출력
    for (int day = 1; day <= days_in_month; day++) {
        printf("%2d ", day);
        if ((first_day + day) % 7 == 0) {
            printf("\n");
        }
    }
    printf("\n\n");

    // 해당 월의 이벤트 출력
    list_events(year, month);
}

void add_event(Event event) {
    if (event_count >= MAX_EVENTS) {
        handle_error("이벤트 최대 개수를 초과했습니다.");
        return;
    }

    events[event_count++] = event;
    save_calendar_data();
    log_message("새 이벤트가 추가되었습니다.");
}

void remove_event(time_t event_time) {
    for (int i = 0; i < event_count; i++) {
        if (events[i].start_time == event_time) {
            // 마지막 이벤트를 현재 위치로 이동
            if (i < event_count - 1) {
                events[i] = events[event_count - 1];
            }
            event_count--;
            save_calendar_data();
            log_message("이벤트가 삭제되었습니다.");
            return;
        }
    }
    handle_error("해당 시간의 이벤트를 찾을 수 없습니다.");
}

void list_events(int year, int month) {
    printf("이벤트 목록:\n");
    int found = 0;

    for (int i = 0; i < event_count; i++) {
        struct tm* event_time = localtime(&events[i].start_time);
        if (event_time->tm_year + 1900 == year && event_time->tm_mon + 1 == month) {
            char time_str[100];
            format_time(events[i].start_time, time_str, sizeof(time_str));
            printf("%s - %s\n", time_str, events[i].title);
            found = 1;
        }
    }

    if (!found) {
        printf("이번 달 예정된 이벤트가 없습니다.\n");
    }
}

void save_calendar_data(void) {
    FILE* file = fopen("data/calendar.dat", "wb");
    if (file) {
        fwrite(&event_count, sizeof(int), 1, file);
        fwrite(events, sizeof(Event), event_count, file);
        fclose(file);
    } else {
        handle_error("캘린더 데이터 저장 실패");
    }
}

void load_calendar_data(void) {
    FILE* file = fopen("data/calendar.dat", "rb");
    if (file) {
        fread(&event_count, sizeof(int), 1, file);
        fread(events, sizeof(Event), event_count, file);
        fclose(file);
    }
} 