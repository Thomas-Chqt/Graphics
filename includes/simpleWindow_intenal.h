/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simpleWindow_intenal.h                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 15:18:33 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 19:45:14 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLEWINDOW_INTENAL_H
# define SIMPLEWINDOW_INTENAL_H

# include <stdlib.h>
# include <limits.h>

# include <mlx.h>
# include <libft.h>

# include "simpleWindow.h"

# ifdef DEBUG
#  include <memory_leak_detector.h>
# endif // DEBUG

# define MALLOC_ERROR 10
# define MLX_INIT_ERROR 11
# define INPUT_ERROR 12
# define WINDOW_CREATION_ERROR 12
# define IMAGE_CREATION_ERROR 13

typedef struct s_g_datas	t_g_data;
typedef struct s_event		t_event;

struct s_g_datas
{
	void	*mlx_ptr;
	t_list	*window_list;
};

struct s_window
{
	void		*mlx_win;
	void		*mlx_image;
	t_uint32	image_h;
	t_uint32	image_w;
	void		*pixels;
	t_list		*event_lists;
	t_bool		is_envent_init;
	void		(*destructor)(void*);
	void		*destructor_data;
};

struct s_event
{
	int			id;
	t_eveact	act;
	void		(*func)(void *);
	void		*data;
};

t_g_data	*global_data(void);
int			init_g_data(void);

void		*set_last_err_ptr(int nbr, void *ret);
int			set_last_err(int nbr);

#endif // SIMPLEWINDOW_INTENAL_H