
# 静态库名称和目标文件列表
TARGET := libnetsender.a

INC_DIR := -Iinclude
CFLAGS := -g

SRC := $(wildcard src/*.cpp)
OBJS := $(subst .cpp,.o, $(SRC))
D_FILE := $(subst .cpp,.d, $(SRC))

SRC_C := $(wildcard *.c)
OBJS_C := $(subst .c,.o, $(SRC_C))
D_FILE_C := $(subst .c,.d, $(SRC_C))

$(TARGET) : $(OBJS) $(OBJS_C)
#	$(CXX) -o $(TARGET) $(LD_LIBS) $(LOCAL_LDLIBS) $(LIB_EX_DIR) $^
#	$(CXX) -o $(TARGET) $^ `pkg-config opencv4 --cflags --libs`
#	$(CXX) -o $(TARGET) $^ $(LD_LIBS)
	ar rcs $@ $(OBJS)

-include *.d
%.o : %.cpp
	$(CXX)  $(CFLAGS) -MMD -c -o $@ $(INC_DIR) $<

%.o : %.c
	$(CC) $(CFLAGS) -MMD -c -o $@ $(INC_DIR) $<

clean:
	rm $(TARGET) $(D_FILE) $(D_FILE_C) $(OBJS) $(OBJS_C)
