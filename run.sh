#!/bin/bash

# 필요한 디렉토리 생성
mkdir -p data

# 프로그램 컴파일
make

# 프로그램 실행
./bin/pomodoro_calendar

# 에러 처리
if [ $? -ne 0 ]; then
    echo "프로그램 실행 중 오류가 발생했습니다."
    exit 1
fi 