

detected_OS = $(shell uname -s)

program_NAME := main 
program_C_SRCS := $(wildcard *.c) ./sharedMemory/sharedMemory.c
program_CXX_SRCS := $(wildcard *.cpp) ./rplidar/rplidarPi.cpp 
program_C_OBJS := ${program_C_SRCS:.c=.o} 
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
program_INCLUDE_DIRS := ./rplidar ./sharedMemory ../transceiver
program_LIBRARY_DIRS :=
program_LIBRARIES := $(wildcard *.a)

CC=gcc
CXX=g++
CXXFLAGS += -std=c++11 -objdump -objcopy -Wall -pthread -funsigned-char -fPIC
CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),$(library))

.PHONY: all memcheck clean distclean

all: clean $(program_NAME)

$(program_NAME): $(program_OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(program_OBJS)$(LDFLAGS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)
	
distclean: clean 
