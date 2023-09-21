# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/16 11:20:21 by tchoquet          #+#    #+#              #
#    Updated: 2023/09/21 23:30:15 by tchoquet         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

LIB_TYPE	= dynamic# static | dynamic
TARGET_TYPE	= release# release | debug

EXPORT_INCLUDE_DIR	= ${MY_C_INCLUDE_PATH}
EXPORT_LIB_DIR		= ${MY_LIBRARY_PATH}

ROOT				= .
SRCS_DIR			= $(shell find ${ROOT}/sources -type d)
INCLUDES_DIR 		= $(shell find ${ROOT}/includes -type d)
BUILD_DIR			= ${ROOT}/.build

EXPORT_INCLUDE	= ${EXPORT_INCLUDE_DIR}/simpleWindow.h
NAME_BASE 		= ${EXPORT_LIB_DIR}/libsimpleWindow


    SRC = $(foreach dir, ${SRCS_DIR}, $(wildcard ${dir}/*.c))
    OBJ = $(foreach file, ${SRC:.c=.o}, ${BUILD_DIR}/$(notdir ${file}))
ifeq (${TARGET_TYPE}, debug)
    OBJ := ${OBJ:.o=_debug.o}
else ifneq (${TARGET_TYPE}, release)
    $(error Bad TARGET_TYPE)
endif


    CC			= cc
ifeq (${TARGET_TYPE}, release)
    CFLAGS		= -Wall -Wextra -Werror
    LDLIBS		= -l ft -l mlx -framework OpenGL -framework AppKit
else ifeq (${TARGET_TYPE}, debug)
    CFLAGS		= -g -D DEBUG
    LDLIBS		= -l ft_debug -l mlx -framework OpenGL -framework AppKit -l memory_leak_detector
else
    $(error Bad TARGET_TYPE)
endif
    CPPFLAGS	= $(foreach dir, ${INCLUDES_DIR}, -I${dir})
    LDFLAGS		=


ifeq (${TARGET_TYPE}, release)
    ifeq (${LIB_TYPE}, static)
        NAME = ${NAME_BASE}.a
    else ifeq (${LIB_TYPE}, dynamic) 
        NAME = ${NAME_BASE}.dylib
    else
        $(error Bad LIB_TYPE)
    endif
else ifeq (${TARGET_TYPE}, debug)
    ifeq (${LIB_TYPE}, static)
        NAME = ${NAME_BASE}_debug.a
    else ifeq (${LIB_TYPE}, dynamic) 
        NAME = ${NAME_BASE}_debug.dylib
    else
        $(error Bad LIB_TYPE)
    endif
else
    $(error Bad TARGET_TYPE)
endif


vpath %.c ${SRCS_DIR}
vpath %.h ${INCLUDES_DIR}

.PHONY: all clean fclean re debug redebug


all: ${NAME} ${EXPORT_INCLUDE}

${NAME}: ${OBJ} | ${EXPORT_LIB_DIR}
ifeq (${LIB_TYPE}, static)
	@ar rc $@ $^
else
	@${CC} ${LDFLAGS} -dynamiclib -o $@ $^ ${LDLIBS}
endif
	@echo "Library created at $@"

${BUILD_DIR}/%_debug.o ${BUILD_DIR}/%.o: %.c | ${BUILD_DIR}
	${CC} ${CFLAGS} ${CPPFLAGS} -o $@ -c $<

${EXPORT_INCLUDE_DIR}/%.h: %.h | ${EXPORT_INCLUDE_DIR}
	@cp $< $@
	@echo "Include file copied at $@"

clean:
ifeq (${TARGET_TYPE}, release)
	@rm -rf ${BUILD_DIR}
	@echo "Build folder deleted"
endif

fclean: clean
ifeq (${TARGET_TYPE}, debug)
	@rm -rf ${NAME}
	@echo "${NAME} deleted."
else ifeq (${TARGET_TYPE}, release)
	@rm -rf ${NAME}
	@echo "${NAME} deleted"
	@make TARGET_TYPE=debug fclean
	@rm -rf ${EXPORT_INCLUDE}
	@echo "${EXPORT_INCLUDE} deleted"
else
    $(error Bad TARGET_TYPE)
endif

re: fclean all

debug:
	@make TARGET_TYPE=debug all

redebug: fclean debug


#folders
${EXPORT_INCLUDE_DIR} ${EXPORT_LIB_DIR} ${BUILD_DIR}:
	@mkdir -p $@
	@echo "Folder $@ created"
