# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/16 11:20:21 by tchoquet          #+#    #+#              #
#    Updated: 2023/09/19 19:33:13 by tchoquet         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

LIB_TYPE	= dynamic# static | dynamic
TARGET_TYPE	= release# release | debug
USED_LIB	= MiniLibX# MiniLibX | SDL2

ROOT			= .
SRCS_DIR		= ${ROOT}/sources
INCLUDES_DIR 	= ${ROOT}/includes
BUILD_DIR		= ${ROOT}/.build

EXPORT_INCLUDE_DIR	= ${MY_C_INCLUDE_PATH}
EXPORT_LIB_DIR		= ${MY_LIBRARY_PATH}

SRC		= ${wildcard ${SRCS_DIR}/*.c}

ifeq (${TARGET_TYPE}, release)
    OBJ	= ${patsubst ${SRCS_DIR}/%, ${BUILD_DIR}/%, ${SRC:.c=.o}}
else ifeq (${TARGET_TYPE}, debug)
    OBJ	= ${patsubst ${SRCS_DIR}/%, ${BUILD_DIR}/%, ${SRC:.c=_debug.o}}
else
    $(error Bad TARGET_TYPE)
endif

CC			= cc

ifeq (${TARGET_TYPE}, release)
    CFLAGS	= -Wall -Wextra -Werror
else ifeq (${TARGET_TYPE}, debug)
    CFLAGS	= -g -D MEMCHECK -D DEBUG
else
    $(error Bad TARGET_TYPE)
endif

CPPFLAGS	= -I${INCLUDES_DIR}
LDFLAGS		=
LDLIBS		=

ifeq (${TARGET_TYPE}, release)
    NAME_STATIC		= ${EXPORT_LIB_DIR}/libsimpleWindow.a
    NAME_DYNAMIC	= ${EXPORT_LIB_DIR}/libsimpleWindow.dylib
else ifeq (${TARGET_TYPE}, debug)
    NAME_STATIC		= ${EXPORT_LIB_DIR}/libsimpleWindow_debug.a
    NAME_DYNAMIC	= ${EXPORT_LIB_DIR}/libsimpleWindow_debug.dylib
else
    $(error Bad TARGET_TYPE)
endif


EXPORT_INCLUDE	= ${EXPORT_INCLUDE_DIR}/simpleWindow.h

ifeq (${USED_LIB}, MiniLibX)
CFLAGS += -D USING_MINILIBX
else ifeq (${USED_LIB}, SDL2)
CFLAGS += -D USING_SDL2
else
$(error Bad USED_LIB)
endif


.PHONY: all clean fclean re


ifeq (${LIB_TYPE}, static)
all: ${NAME_STATIC} ${EXPORT_INCLUDE}
else ifeq (${LIB_TYPE}, dynamic)
all: ${NAME_DYNAMIC} ${EXPORT_INCLUDE}
else
$(error Bad LIB_TYPE)
endif


${NAME_STATIC}: ${OBJ} | ${EXPORT_LIB_DIR}
	@ar rc $@ $^
	@echo "Static library created at $@"

${NAME_DYNAMIC}: ${OBJ} | ${EXPORT_LIB_DIR}
	gcc -dynamiclib -o $@ $^
	@echo "dynamic library created at $@"

${EXPORT_INCLUDE_DIR}/%.h: ${INCLUDES_DIR}/%.h | ${EXPORT_INCLUDE_DIR}
	@cp $< $@
	@echo "Include file copied at $@"

${BUILD_DIR}/%_debug.o ${BUILD_DIR}/%.o: ${SRCS_DIR}/%.c | ${BUILD_DIR}
	${CC} ${CFLAGS} ${CPPFLAGS} -o $@ -c $<

clean:
	@rm -rf ${OBJ}
	@echo "Object files deleted."

fclean: clean
ifeq (${LIB_TYPE}, static)
	@rm -rf ${NAME_STATIC}
	@echo "${NAME_STATIC} deleted."
else ifeq (${LIB_TYPE}, dynamic)
	@rm -rf ${NAME_DYNAMIC}
	@echo "${NAME_DYNAMIC} deleted."
else
$(error Bad LIB_TYPE)
endif
	@rm -rf ${EXPORT_INCLUDE}
	@echo "${EXPORT_INCLUDE} deleted."

re: fclean all


#folders
${EXPORT_INCLUDE_DIR} ${EXPORT_LIB_DIR} ${BUILD_DIR}:
	@mkdir -p $@
	@echo "Folder $@ created."