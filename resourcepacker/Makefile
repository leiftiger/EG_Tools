
# Compiles the ERB resource packer component

SRC_DIR := .
OBJ_DIR := ./bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

OUT_LIB := ../x64/Release/lib_Resource_Packer.a

CC := g++
CCFLAGS := -O2 -std=c++17 -Wall


all: ${SRC_FILES}
	mkdir -p bin
	${CC} ${CCFLAGS} -c *.cpp -I.
	mv *.o ${OBJ_DIR}/
	ar rvs ${OUT_LIB} ${OBJ_FILES}

clean:
	rm -f ${OBJ_DIR}/*.o
	rm -f ${OUT_LIB}

