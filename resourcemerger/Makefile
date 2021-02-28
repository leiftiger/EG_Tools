
# Compiles the mod patcher component

SRC_DIR := .
OBJ_DIR := ./bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

OUT_LIB := ../x64/Release/lib_Resource_Merger.a

CC := g++
CCFLAGS := -O2 -std=c++17 -Wall
INCS :=  -I. -I../interpreter -I../resourcepacker

${OUT_LIB}: ${OBJ_FILES}
	ar rvs ${OUT_LIB} ${OBJ_FILES}

${OBJ_DIR}/IResourcePatch.o: ${SRC_DIR}/IResourcePatch.h ${SRC_DIR}/IResourcePatch.cpp
	mkdir -p ${OBJ_DIR}
	${CC} ${CCFLAGS} -c ${SRC_DIR}/IResourcePatch.cpp -o ${OBJ_DIR}/IResourcePatch.o ${INCS}

${OBJ_DIR}/IResourcePatcher.o: ${SRC_DIR}/IResourcePatcher.h ${SRC_DIR}/IResourcePatcher.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/IResourcePatcher.cpp -o ${OBJ_DIR}/IResourcePatcher.o ${INCS}

${OBJ_DIR}/ModPack.o: ${SRC_DIR}/ModPack.h ${SRC_DIR}/ModPack.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ModPack.cpp -o ${OBJ_DIR}/ModPack.o ${INCS}

${OBJ_DIR}/MutexCommunication.o: ${SRC_DIR}/MutexCommunication.h ${SRC_DIR}/MutexCommunication.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/MutexCommunication.cpp -o ${OBJ_DIR}/MutexCommunication.o ${INCS}

${OBJ_DIR}/PatchATN.o: ${SRC_DIR}/PatchATN.h ${SRC_DIR}/PatchATN.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/PatchATN.cpp -o ${OBJ_DIR}/PatchATN.o ${INCS}

${OBJ_DIR}/PatchCopy.o: ${SRC_DIR}/PatchCopy.h ${SRC_DIR}/PatchCopy.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/PatchCopy.cpp -o ${OBJ_DIR}/PatchCopy.o ${INCS}

${OBJ_DIR}/PatchCSV.o: ${SRC_DIR}/PatchCSV.h ${SRC_DIR}/PatchCSV.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/PatchCSV.cpp -o ${OBJ_DIR}/PatchCSV.o ${INCS}

${OBJ_DIR}/PatchDesc.o: ${SRC_DIR}/PatchDesc.h ${SRC_DIR}/PatchDesc.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/PatchDesc.cpp -o ${OBJ_DIR}/PatchDesc.o ${INCS}

${OBJ_DIR}/PatchGeneric.o: ${SRC_DIR}/PatchGeneric.h ${SRC_DIR}/PatchGeneric.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/PatchGeneric.cpp -o ${OBJ_DIR}/PatchGeneric.o ${INCS}

${OBJ_DIR}/PatchIni.o: ${SRC_DIR}/PatchIni.h ${SRC_DIR}/PatchIni.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/PatchIni.cpp -o ${OBJ_DIR}/PatchIni.o ${INCS}

${OBJ_DIR}/PatchPattern.o: ${SRC_DIR}/PatchPattern.h ${SRC_DIR}/PatchPattern.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/PatchPattern.cpp -o ${OBJ_DIR}/PatchPattern.o ${INCS}

${OBJ_DIR}/PatchStrings.o: ${SRC_DIR}/PatchStrings.h ${SRC_DIR}/PatchStrings.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/PatchStrings.cpp -o ${OBJ_DIR}/PatchStrings.o ${INCS}

${OBJ_DIR}/ResourceMerger.o: ${SRC_DIR}/ResourceMerger.h ${SRC_DIR}/ResourceMerger.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ResourceMerger.cpp -o ${OBJ_DIR}/ResourceMerger.o ${INCS}

${OBJ_DIR}/StringList.o: ${SRC_DIR}/StringList.h ${SRC_DIR}/StringList.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/StringList.cpp -o ${OBJ_DIR}/StringList.o ${INCS}

clean:
	rm -f ${OBJ_DIR}/*.o
	rm -f ${OUT_LIB}

