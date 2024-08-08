# 注意命名处后面不能有空格
.PHONY 		: clean
CC 			:= gcc
INCLUDE_DIR := -I include
CFLAGS   	:= -c
LDFLAGS     := -lpthread

OBJ_DIR 	:= ./obj
BIN_DIR 	:= ./bin
MAIN_DIR	:= ./main

SRC      	= $(shell find ./driver -name "*.c")
# notdir,去除路径名
OBJ     	= $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRC)))

# 当源文件与.o文件不在一个文件夹下，需要配置.c文件
SOURSE_DIR 	= $(dir $(SRC))
vpath %.c $(SOURSE_DIR)

TARGET1 		:= $(BIN_DIR)/server
TARGET2 		:= $(BIN_DIR)/client-sub
TARGET3 		:= $(BIN_DIR)/client-pub
	
OBJ1			:= $(OBJ) $(MAIN_DIR)/server.o
OBJ2 			:= $(OBJ) $(MAIN_DIR)/client-sub.o
OBJ3 			:= $(OBJ) $(MAIN_DIR)/client-pub.o

# 先创建bin目录，再编译，需要依赖多个条件
all : $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1) : $(OBJ1)
	$(CC) $(OBJ1) -o $(TARGET1) $(LDFLAGS)

$(TARGET2) : $(OBJ2)
	$(CC) $(OBJ2) -o $(TARGET2) $(LDFLAGS)

$(TARGET3) : $(OBJ3)
	$(CC) $(OBJ3) -o $(TARGET3) $(LDFLAGS)

$(OBJ_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_DIR) 

%.o : %.c
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDE_DIR)

clean:
	rm -rf  ./bin/*  ./obj/*.o ./main/*.o

# 因usr/bin 在PATH中靠后，如有同名的文件在其他目录，可执行文件可能无法执行
install:
	sudo cp $(BIN_DIR)/$(TARGET)  /usr/bin 

uninstall:
	sudo rm -rf /usr/bin/$(TARGET)