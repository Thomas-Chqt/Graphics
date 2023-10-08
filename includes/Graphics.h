/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Graphics.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 14:32:15 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 10:43:39 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GRAPHICS_H
# define GRAPHICS_H

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
# define TRANSP	0xFF000000

# ifndef UINT32_TYPE
#  define UINT32_TYPE

typedef unsigned int		t_uint32;

# endif // UINT32_TYPE

typedef struct s_vect2di
{
	int	x;
	int	y;

}	t_vi2d;

typedef struct s_vect2df
{
	float	x;
	float	y;

}	t_vf2d;

typedef struct s_context	t_ctx;

int		create_window(char *title, t_vi2d size);
void	start_main_loop(void (*func)(void *), void *data);
void	delete_window(int return_code);

int		add_event(int key, int trig, void (*func)(void *), void *data);
int		poll_key(int *key);

t_vi2d	mouse_pos(void);
void	set_mouse_pos(t_vi2d pos);

#endif // GRAPHICS_H