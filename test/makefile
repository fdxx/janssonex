BUILD_DIR := build
CXX := clang++
CXXFLAGS := -O3 -g3 --std=c++17 -I../jansson
PROJECT_NAME := test

all: jansson.o
	mkdir -p $(BUILD_DIR) 
	$(CXX) $(CXXFLAGS) -c $(PROJECT_NAME).cpp -o $(BUILD_DIR)/$(PROJECT_NAME).o
	$(CXX) $(BUILD_DIR)/*.o ../jansson/build/*.o -o $(PROJECT_NAME)
	rm -rf $(BUILD_DIR) ../jansson/build 

jansson.o:
	cd ../jansson && make all
