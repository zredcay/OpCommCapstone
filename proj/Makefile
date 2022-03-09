

detected_OS = $(shell uname -s)

program_NAME := main
program_C_SRCS := $(wildcard *.c)
program_CXX_SRCS := $(wildcard *.cpp)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
program_INCLUDE_DIRS :=
program_LIBRARY_DIRS :=
program_LIBRARIES := $(wildcard *.a)

CXX=g++
CXXFLAGS += -std=c++11 -Wall -pthread -funsigned-char -fPIC -std=gnu99
CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),$(library))

.PHONY: all memcheck clean distclean

all: clean $(program_NAME)

$(program_NAME): $(program_OBJS) Flags.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(program_OBJS) $(LDFLAGS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)
	
distclean: clean clean_submit
