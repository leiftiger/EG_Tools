
# Compiles the EG resource parser component

SRC_DIR := .
OBJ_DIR := ./bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

OUT_LIB := ../x64/Release/lib_Resource_Parser.a

CC := g++
CCFLAGS := -O2 -std=c++17 -Wall
INCS := -I. -I../resourcepacker

${OUT_LIB}: ${OBJ_FILES}
	ar rvs ${OUT_LIB} ${OBJ_FILES}

${OBJ_DIR}/RP_Animation.o: ${SRC_DIR}/RP_Animation.h ${SRC_DIR}/RP_Animation.cpp
	mkdir -p ${OBJ_DIR}
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_Animation.cpp -o ${OBJ_DIR}/RP_Animation.o ${INCS}

${OBJ_DIR}/RP_Audio.o: ${SRC_DIR}/RP_Audio.h ${SRC_DIR}/RP_Audio.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_Audio.cpp -o ${OBJ_DIR}/RP_Audio.o ${INCS}

${OBJ_DIR}/RP_BaseResource.o: ${SRC_DIR}/RP_BaseResource.h ${SRC_DIR}/RP_BaseResource.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_BaseResource.cpp -o ${OBJ_DIR}/RP_BaseResource.o ${INCS}

${OBJ_DIR}/RP_EntityDesc.o: ${SRC_DIR}/RP_EntityDesc.h ${SRC_DIR}/RP_EntityDesc.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_EntityDesc.cpp -o ${OBJ_DIR}/RP_EntityDesc.o ${INCS}

${OBJ_DIR}/RP_FileLoader.o: ${SRC_DIR}/RP_FileLoader.h ${SRC_DIR}/RP_FileLoader.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_FileLoader.cpp -o ${OBJ_DIR}/RP_FileLoader.o ${INCS}

${OBJ_DIR}/RP_GUI.o: ${SRC_DIR}/RP_GUI.h ${SRC_DIR}/RP_GUI.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_GUI.cpp -o ${OBJ_DIR}/RP_GUI.o ${INCS}

${OBJ_DIR}/RP_IResourceParser.o: ${SRC_DIR}/RP_IResourceParser.h ${SRC_DIR}/RP_IResourceParser.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_IResourceParser.cpp -o ${OBJ_DIR}/RP_IResourceParser.o ${INCS}

${OBJ_DIR}/RP_Objectives.o: ${SRC_DIR}/RP_Objectives.h ${SRC_DIR}/RP_Objectives.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_Objectives.cpp -o ${OBJ_DIR}/RP_Objectives.o ${INCS}

${OBJ_DIR}/RP_PackLoader.o: ${SRC_DIR}/RP_PackLoader.h ${SRC_DIR}/RP_PackLoader.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_PackLoader.cpp -o ${OBJ_DIR}/RP_PackLoader.o ${INCS}

${OBJ_DIR}/RP_Strings.o: ${SRC_DIR}/RP_Strings.h ${SRC_DIR}/RP_Strings.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_Strings.cpp -o ${OBJ_DIR}/RP_Strings.o ${INCS}

${OBJ_DIR}/RP_utils.o: ${SRC_DIR}/RP_utils.h ${SRC_DIR}/RP_utils.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_utils.cpp -o ${OBJ_DIR}/RP_utils.o ${INCS}

${OBJ_DIR}/RP_Video.o: ${SRC_DIR}/RP_Video.h ${SRC_DIR}/RP_Video.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/RP_Video.cpp -o ${OBJ_DIR}/RP_Video.o ${INCS}

clean:
	rm -f ${OBJ_DIR}/*.o
	rm -f ${OUT_LIB}

