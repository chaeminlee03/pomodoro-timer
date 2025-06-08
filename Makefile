CC = gcc
CFLAGS = -I./include -Wall -Wextra
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# 소스 파일 목록
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/pomodoro_calendar

# 기본 타겟
all: directories $(TARGET)

# 디렉토리 생성
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# 오브젝트 파일 컴파일
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 실행 파일 링크
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# 실행 파일 권한 설정
	chmod +x $(TARGET)

# 정리
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean directories 