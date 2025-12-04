# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2025/12/04 11:19:58
# ---------------------------------------------------


BUILD_DIR ?= build

CC       ?= gcc-14
CXX      ?= g++-14
CFLAGS   ?=
CXXFLAGS ?=

NAME = scop
EXE  = ${BUILD_DIR}/examples/${NAME}/${NAME}

all: ${NAME}

${NAME}: ${EXE}
	cp $< $@

${EXE}: config-mandatory
	cmake --build ${BUILD_DIR} --target ${NAME} --parallel

re: config-mandatory
	cmake --build ${BUILD_DIR} --target ${NAME} --parallel --clean-first

bonus: config-bonus
	cmake --build ${BUILD_DIR} --parallel
	cp $< $@

re-bonus: config-bonus
	cmake --build ${BUILD_DIR} --parallel --clean-first
	cp $< $@

clean:
	@if [ -d  ${BUILD_DIR} ]; then\
		cmake --build ${BUILD_DIR} --target clean --parallel
	fi
	rm -f ${NAME}

fclean:
	rm -rf ${BUILD_DIR} ${NAME}

config-mandatory:
	cmake -S . -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} -DCMAKE_C_FLAGS=${CFLAGS} -DCMAKE_CXX_FLAGS=${CXXFLAGS} -DGFX_ENABLE_GLFW=ON -DGFX_ENABLE_IMGUI=ON -DGFX_BUILD_EXAMPLES=ON -DSCOP_MANDATORY=ON

config-bonus:
	cmake -S . -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} -DCMAKE_C_FLAGS=${CFLAGS} -DCMAKE_CXX_FLAGS=${CXXFLAGS} -DGFX_ENABLE_GLFW=ON -DGFX_ENABLE_IMGUI=ON -DGFX_BUILD_EXAMPLES=ON -DSCOP_MANDATORY=OFF

