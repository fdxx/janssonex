# 所有源文件和目标文件
BUILD_DIR := build
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

# 编译选项
CXX := clang
CXXFLAGS := -O3 -g3 -I. -DHAVE_CONFIG_H

# 默认目标
all: $(OBJS)

# 生成.o文件
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
# 创建目标文件夹
$(BUILD_DIR):
	mkdir -p $@
