/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Graphics_internal.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 15:18:33 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 12:42:35 by tchoquet         ###   ########.fr       */
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

struct s_context
{
	void	*mlx_image;
	void	*pixels;
	t_vec2i	size;
};

typedef struct s_graphics_global
{
	void		*mlx_ptr;
	void		*mlx_win;
	t_ctx		*back_ctx;
	t_ctx		*draw_ctx;

	void		(*destr)(void *);
	void		*destr_data;

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

t_graph		*graph(void);
int			graph_init_mlx(void);
int			graph_init_window(char *title, t_vec2i size);
int			graph_init_ctxs(t_vec2i win_size);
void		clean_graph(void);

t_uint32	*px(t_ctx *context, t_vec2i pos);

#endif // GRAPHICS_INTERNAL_H