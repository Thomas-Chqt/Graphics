/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Graphics_internal.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 15:18:33 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 21:17:36 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GRAPHICS_INTERNAL_H
# define GRAPHICS_INTERNAL_H

# include <stdlib.h>
# include <math.h>

# include <libft.h>
# include <mlx.h>

# include "Graphics.h"

# ifdef DEBUG
#  include <memory_leak_detector.h>
# endif // DEBUG

typedef struct s_graphics_global
{
	void		*mlx_ptr;
	void		*mlx_win;
	t_vi2d		win_size;

	t_list		*pressed;
	t_list		*pres_curr;

	t_list		*kdo_lst;
	t_list		*kup_lst;
	t_list		*mdo_lst;
	t_list		*mup_lst;
	t_list		*mov_lst;
	t_list		*exp_lst;
	t_list		*des_lst;

}	t_graph;

t_graph	*graph(void);
int		graph_init_mlx(void);
int		graph_init_window(char *title, t_vi2d size);
void	clean_graph(void);

#endif // GRAPHICS_INTERNAL_H