
# 静态库名称和目标文件列表
TARGET := libnetsender.a
DYNANIC_LIB := libnetsender.so

CXX := $(CROSS_COMPILE)g++

INC_DIR := -Iinclude
CFLAGS := -g -fPIC

SRC := $(wildcard src/*.cpp)
OBJS := $(subst .cpp,.o, $(SRC))
D_FILE := $(subst .cpp,.d, $(SRC))

SRC_C := $(wildcard *.c)
OBJS_C := $(subst .c,.o, $(SRC_C))
D_FILE_C := $(subst .c,.d, $(SRC_C))

LD_LIBS := -pthread

all : $(TARGET) $(DYNANIC_LIB)

$(TARGET) : $(OBJS) $(OBJS_C)
#	$(CXX) -o $(TARGET) $(LD_LIBS) $(LOCAL_LDLIBS) $(LIB_EX_DIR) $^
#	$(CXX) -o $(TARGET) $^ `pkg-config opencv4 --cflags --libs`
#	$(CXX) -o $(TARGET) $^ $(LD_LIBS)
	ar rcs $@ $(OBJS) $(OBJS_C)

$(DYNANIC_LIB) : $(OBJS) $(OBJS_C)
	$(CXX) -shared -fPIC -o $@ $^ $(LD_LIBS)

-include *.d
%.o : %.cpp
	$(CXX)  $(CFLAGS) -MMD -c -o $@ $(INC_DIR) $<

%.o : %.c
	$(CC) $(CFLAGS) -MMD -c -o $@ $(INC_DIR) $<

clean:
	rm $(TARGET) $(D_FILE) $(D_FILE_C) $(OBJS) $(OBJS_C)
