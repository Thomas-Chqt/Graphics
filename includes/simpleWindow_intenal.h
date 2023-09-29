/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simpleWindow_intenal.h                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 15:18:33 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/29 15:22:13 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLEWINDOW_INTENAL_H
# define SIMPLEWINDOW_INTENAL_H

# include <stdlib.h>

# include <libft.h>
# include <mlx.h>

# include "simpleWindow.h"

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

struct s_window
{
	void	*mlx_win;
	void	*mlx_image;
	t_wh	img_size;
	void	*pixels;
	t_func	destr;

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

t_swglob	*swglob(void);
int			init_swglob(void);

int			kdo_hook(int keycode, t_win *win);
int			kup_hook(int keycode, t_win *win);
int			mdo_hook(int button, int x, int y, t_win *win);
int			mup_hook(int button, int x, int y, t_win *win);
int			mov_hook(int x, int y, t_win *win);
int			exp_hook(t_win *win);
int			des_hook(t_win *win);

#endif // SIMPLEWINDOW_INTENAL_H