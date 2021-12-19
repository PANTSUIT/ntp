


TARGET_NAMES = main domain

# PROJECT
PROJECT_ROOT_PATH = ${PWD}

# SUB_DIRS = ../n_lib/cJSON ../n_lib/x_string ../n_lib/x_log ../n_lib/x_protocol
# VPATH += ${SUB_DIRS} \

C_SOURCES = ./ntp.c

C_OBJECTS = $(patsubst %.c, %.o, ${C_SOURCES})

# compiler

# CC = arm-linux-gnueabihf-gcc
# CPP = arm-linux-gnueabihf-g++
# AR = arm-linux-gnueabihf-ar

CC = gcc

INCLUDE_PATHS += 

C_LIBS += -L. \
					
COM_FLAGS = -Wall -g -O2 -fPIC

C_FLAGS = ${COM_FLAGS} ${INCLUDE_PATHS} ${C_LIBS} -std=gnu11

CPP_FLAGS = ${COM_FLAGS} -std=gnu++11

.PHONY: all before_build after_build app clean ${TARGET_NAMES}

all: before_build app after_build 

before_build: 
	@echo " **************** ${@} ****************"
	echo ${RM}

after_build: 
	@echo " **************** ${@} ****************"

app: ${TARGET_NAMES}

${TARGET_NAMES}: %: %.o ${C_OBJECTS}
	@echo " **************** ${@} ****************"
	${CC} -o $@ $^ ${C_FLAGS}

%.o: %.c
	${CC} -c -o $@ $^ ${C_FLAGS}

clean:
	${RM} ${TARGET_NAMES}
	${RM} *.o
	${RM} ${C_OBJECTS}

