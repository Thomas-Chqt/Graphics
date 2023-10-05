/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simpleWindow.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 14:32:15 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/05 18:54:05 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLEWINDOW_H
# define SIMPLEWINDOW_H

# define L_CLICK		1
# define R_CLICK		2
# define M_CLICK		3

# define W_DOWN			4
# define W_UP			5

# define R_KEY			15

# define ONE_KEY		18
# define TWO_KEY		19
# define THREE_KEY		20
# define FOUR_KEY		21
# define FIVE_KEY		23

# define PLUS_KEY		24
# define NINUS_KEY		27

# define ENTER_KEY		36
# define SPACE_KEY		49
# define ESC_KEY		53

# define LEFT_KEY		123
# define RIGHT_KEY		124
# define UP_KEY			126
# define DOWN_KEY		125

# define ON_KEYDOWN		0b00000001
# define ON_KEYUP		0b00000010
# define ON_MOUSEMOVE	0b00010000
# define ON_EXPOSE		0b00100000
# define ON_DESTROY		0b01000000

# define RED	0x00FF0000
# define GREEN	0x0000FF00
# define BLUE	0x000000FF
# define BLACK	0x00000000
# define WHITE	0x00FFFFFF
# define NONE	0xFF000000

# ifndef UINT32_TYPE
#  define UINT32_TYPE

typedef unsigned int		t_uint32;

# endif // UINT32_TYPE

typedef struct s_width_height
{
	int	w;
	int	h;

}	t_wh;

typedef struct s_function_one_arg
{
	void	(*ptr)(void*);
	void	*data;

}	t_func;

typedef struct s_position
{
	int	x;
	int	y;

}	t_pos;

typedef struct s_window		t_win;
typedef struct s_context	t_ctx;		

t_win	*new_window(char *title, t_wh size);
void	set_destructor(t_win *window, t_func func);
void	set_backcolor(t_win *window, t_uint32 color);
void	delete_window(t_win *window);

int		add_event(t_win *window, int key, int trig, t_func func);
int		poll_key(t_win *window, int *key);

void	start_main_loop(t_func func);

t_pos	get_mouse_pos(t_win *window);
void	set_mouse_pos(t_win *window, t_pos pos);

t_ctx	*new_context(t_wh size);
void	fill_ctx(t_ctx *ctx, t_uint32 color);
void	put_ctx_to_win(t_win *window, t_ctx *ctx, t_pos pos);
void	delete_ctx(t_ctx *context);

void	put_pixel(t_ctx *ctx, t_pos pos, t_uint32 color);

#endif // SIMPLEWINDOW_H