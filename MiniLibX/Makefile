# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/21 14:47:43 by tchoquet          #+#    #+#              #
#    Updated: 2023/10/05 20:12:48 by tchoquet         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libmlx.a

SRC	= mlx_shaders.c				\
	  mlx_new_window.m			\
	  mlx_init_loop.m			\
	  mlx_new_image.m			\
	  mlx_xpm.c					\
	  mlx_int_str_to_wordtab.c	\
	  mlx_png.c					\
	  mlx_mouse.m
		
OBJ1	= ${SRC:.c=.o}
OBJ		= ${OBJ1:.m=.o}

CFLAGS	= -O2
CFLAGS	+= -DSTRINGPUTX11 # add to match string put with X11 in size and position
CFLAGS	+= -w # add hide warnings

vpath %.h ${INCLUDES_DIR}

all: ${NAME}

${NAME}: ${OBJ}
	@ar rc $@ $^
	@ranlib $@
	@echo "Library created at $@"

clean:
	@rm -rf ${OBJ} *~
	@echo "Object files deleted (MiniLibX)"

fclean: clean
	@rm -rf ${NAME}
	@echo "${NAME} deleted."

re: clean all
