
# Compiles the ERB resource packer component

SRC_DIR := .
OBJ_DIR := ./bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

OUT_LIB := ../x64/Release/lib_Resource_Packer.a

CC := g++
CCFLAGS := -O2 -std=c++17 -Wall
INCS := -I.

${OUT_LIB}: ${OBJ_FILES}
	ar rvs ${OUT_LIB} ${OBJ_FILES}

${OBJ_DIR}/ResourcePack.o: ${SRC_DIR}/ResourcePack.h ${SRC_DIR}/ResourcePack.cpp
	mkdir -p ${OBJ_DIR}
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ResourcePack.cpp -o ${OBJ_DIR}/ResourcePack.o ${INCS}

${OBJ_DIR}/ResourcePacks.o: ${SRC_DIR}/ResourcePacks.h ${SRC_DIR}/ResourcePacks.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ResourcePacks.cpp -o ${OBJ_DIR}/ResourcePacks.o ${INCS}

clean:
	rm -f ${OBJ_DIR}/*.o
	rm -f ${OUT_LIB}

