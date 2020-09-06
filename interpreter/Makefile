
# Compiles the ATN interpreter component

SRC_DIR := ./ATN
OBJ_DIR := ./bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

OUT_LIB := ../x64/Release/lib_ATN_Interpreter.a

CC := g++
CCFLAGS := -O2 -std=c++17 -Wall


all: ${SRC_FILES} utils.cpp
	mkdir -p bin
	${CC} ${CCFLAGS} -c ATN/*.cpp utils.cpp -I. -I./ATN
	mv *.o ${OBJ_DIR}/
	ar rvs ${OUT_LIB} ${OBJ_FILES} ${OBJ_DIR}/utils.o

clean:
	rm -f ${OBJ_DIR}/*.o
	rm -f ${OUT_LIB}

