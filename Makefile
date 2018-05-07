# Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.

#CC = arm-linux-gnueabihf-gcc
#CXX = arm-linux-gnueabihf-g++

CC = gcc
CXX = g++

CFLAGS = -Wall -Wno-unused-variable -Wno-unused-but-set-parameter -Wno-unused-but-set-variable
CXXFLAGS = -Wall -Wno-unused-variable -Wno-unused-but-set-parameter -Wno-unused-but-set-variable -std=c++0x


TARGET = main

all: $(TARGET)

% : %.cpp 
	$(CXX) $(CXXFLAGS) $< -o $@  -lpthread  
	
% : %.c 
	$(CC) $(CFLAGS) $< -o $@ -lpthread  

clean:
	rm -f $(TARGET)

