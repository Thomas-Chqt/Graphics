/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Graphics_internal.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 15:18:33 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 19:07:26 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLEWINDOW_INTERNAL_H
# define SIMPLEWINDOW_INTERNAL_H

# include <stdlib.h>
# include <math.h>

# include <libft.h>
# include <mlx.h>

# include "Graphics.h"

# ifdef DEBUG
#  include <memory_leak_detector.h>
# endif // DEBUG

typedef struct s_simple_window_global
{
	void	*mlx_ptr;
	t_list	*win_lst;

}	t_swglob;

typedef struct s_event
{
	int			key;
	int			trig;
	t_func		func;

}	t_event;

struct s_context
{
	void	*mlx_image;
	void	*pixels;
	t_wh	size;
};

struct s_window
{
	void	*mlx_win;
	t_func	destr;
	t_ctx	*back_ctx;

	t_list	*pressed;
	t_list	*pres_curr;

	t_list	*kdo_lst;
	t_list	*kup_lst;
	t_list	*mdo_lst;
	t_list	*mup_lst;
	t_list	*mov_lst;
	t_list	*exp_lst;	
	t_list	*des_lst;

	t_bool	is_hook_init;
};

typedef union u_color
{
	t_uint32	raw;
	struct
	{
		t_uint8	blue;
		t_uint8	green;
		t_uint8	red;
		t_uint8	alpha;
	};
}	t_color;

typedef struct s_fcolor
{
	float	a;
	float	r;
	float	g;
	float	b;
}	t_fcolor;

t_swglob	*swglob(void);
int			init_swglob(void);

void		free_win(t_win *win);

int			kdo_hook(int keycode, t_win *win);
int			kup_hook(int keycode, t_win *win);
int			mdo_hook(int button, int x, int y, t_win *win);
int			mup_hook(int button, int x, int y, t_win *win);
int			mov_hook(int x, int y, t_win *win);
int			exp_hook(t_win *win);
int			des_hook(t_win *win);

t_uint32	*px(t_ctx *context, t_pos pos);
t_color		apha_compos(t_color back, t_color front);

#endif // SIMPLEWINDOW_INTERNAL_H