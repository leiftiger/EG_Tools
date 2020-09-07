
# Compiles the ATN interpreter component

SRC_DIR := ./ATN
OBJ_DIR := ./bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

OUT_LIB := ../x64/Release/lib_ATN_Interpreter.a

CC := g++
CCFLAGS := -O2 -std=c++17 -Wall
INCS := -I. -I./ATN

${OUT_LIB}: ${OBJ_FILES} ${OBJ_DIR}/utils.o
	ar rvs ${OUT_LIB} ${OBJ_FILES} ${OBJ_DIR}/utils.o

${OBJ_DIR}/ATN_DeltaPatching.o: ${SRC_DIR}/ATN_DeltaPatching.h ${SRC_DIR}/ATN_DeltaPatching.cpp
	mkdir -p ${OBJ_DIR}
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_DeltaPatching.cpp -o ${OBJ_DIR}/ATN_DeltaPatching.o ${INCS}

${OBJ_DIR}/ATN_Effect.o: ${SRC_DIR}/ATN_Effect.h ${SRC_DIR}/ATN_Effect.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Effect.cpp -o ${OBJ_DIR}/ATN_Effect.o ${INCS}

${OBJ_DIR}/ATN_Entry.o: ${SRC_DIR}/ATN_Entry.h ${SRC_DIR}/ATN_Entry.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Entry.cpp -o ${OBJ_DIR}/ATN_Entry.o ${INCS}

${OBJ_DIR}/ATN_Exception.o: ${SRC_DIR}/ATN_Exception.h ${SRC_DIR}/ATN_Exception.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Exception.cpp -o ${OBJ_DIR}/ATN_Exception.o ${INCS}

${OBJ_DIR}/ATN_Manager.o: ${SRC_DIR}/ATN_Manager.h ${SRC_DIR}/ATN_Manager.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Manager.cpp -o ${OBJ_DIR}/ATN_Manager.o ${INCS}

${OBJ_DIR}/ATN_Network.o: ${SRC_DIR}/ATN_Network.h ${SRC_DIR}/ATN_Network.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Network.cpp -o ${OBJ_DIR}/ATN_Network.o ${INCS}

${OBJ_DIR}/ATN_Object.o: ${SRC_DIR}/ATN_Object.h ${SRC_DIR}/ATN_Object.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Object.cpp -o ${OBJ_DIR}/ATN_Object.o ${INCS}

${OBJ_DIR}/ATN_Percept.o: ${SRC_DIR}/ATN_Percept.h ${SRC_DIR}/ATN_Percept.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Percept.cpp -o ${OBJ_DIR}/ATN_Percept.o ${INCS}

${OBJ_DIR}/ATN_Property.o: ${SRC_DIR}/ATN_Property.h ${SRC_DIR}/ATN_Property.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Property.cpp -o ${OBJ_DIR}/ATN_Property.o ${INCS}

${OBJ_DIR}/ATN_Resources.o: ${SRC_DIR}/ATN_Resources.h ${SRC_DIR}/ATN_Resources.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Resources.cpp -o ${OBJ_DIR}/ATN_Resources.o ${INCS}

${OBJ_DIR}/ATN_State.o: ${SRC_DIR}/ATN_State.h ${SRC_DIR}/ATN_State.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_State.cpp -o ${OBJ_DIR}/ATN_State.o ${INCS}

${OBJ_DIR}/ATN_Thread.o: ${SRC_DIR}/ATN_Thread.h ${SRC_DIR}/ATN_Thread.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Thread.cpp -o ${OBJ_DIR}/ATN_Thread.o ${INCS}

${OBJ_DIR}/ATN_Transition.o: ${SRC_DIR}/ATN_Transition.h ${SRC_DIR}/ATN_Transition.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/ATN_Transition.cpp -o ${OBJ_DIR}/ATN_Transition.o ${INCS}

${OBJ_DIR}/IATN_Data.o: ${SRC_DIR}/IATN_Data.h ${SRC_DIR}/IATN_Data.cpp
	${CC} ${CCFLAGS} -c ${SRC_DIR}/IATN_Data.cpp -o ${OBJ_DIR}/IATN_Data.o ${INCS}

${OBJ_DIR}/utils.o: utils.h utils.cpp
	${CC} ${CCFLAGS} -c utils.cpp -o ${OBJ_DIR}/utils.o ${INCS}

clean:
	rm -f ${OBJ_DIR}/*.o
	rm -f ${OUT_LIB}

