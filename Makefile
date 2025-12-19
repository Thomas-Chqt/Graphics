# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2025/12/04 11:19:58
# ---------------------------------------------------

BUILD_DIR ?= build

NAME = scop
EXE  = ${BUILD_DIR}/examples/${NAME}/${NAME}

all: ${NAME}

${NAME}: ${EXE}
	ln -sf $< $@

${EXE}: config
	cmake --build ${BUILD_DIR} --target ${NAME} --parallel

config:
	@if ! [ -d  ${BUILD_DIR} ]; then       \
	    cmake -S . -B ${BUILD_DIR}         \
	        -DCMAKE_BUILD_TYPE=Release     \
	        -DCMAKE_C_COMPILER=${CC}       \
	        -DCMAKE_CXX_COMPILER=${CXX}    \
	        -DCMAKE_C_FLAGS=${CFLAGS}      \
	        -DCMAKE_CXX_FLAGS=${CXXFLAGS}  \
	        -DGFX_ENABLE_GLFW=ON           \
	        -DGFX_ENABLE_IMGUI=ON          \
	        -DGFX_BUILD_EXAMPLES=ON        \
	        -DGFX_EXAMPLES_TO_BUILD="scop" \
	        -DSCOP_MANDATORY=ON;           \
	fi

clean:
	@if [ -d  ${BUILD_DIR} ]; then                                   \
	    echo "cmake --build ${BUILD_DIR} --target clean --parallel"; \
	    cmake --build ${BUILD_DIR} --target clean --parallel;        \
	fi
	rm -f ${NAME}

fclean:
	rm -rf ${BUILD_DIR} ${NAME}

re: clean all
