/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 16:51:10 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/17 13:33:06 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_graph	*graph(void)
{
	static t_graph	graphics_global = {
		.is_poll_key_active = true
	};

	return (&graphics_global);
}

int	graph_init_mlx(void)
{
	if (graph()->mlx_ptr != NULL)
		return (0);
	graph()->mlx_ptr = mlx_init();
	if (graph()->mlx_ptr == NULL)
		return (-1);
	return (0);
}

int	graph_init_window(char *title, t_vec2i size)
{
	void	**mlx_win_ptr;

	mlx_win_ptr = &(graph()->mlx_win);
	(*mlx_win_ptr) = mlx_new_window(graph()->mlx_ptr, size.x, size.y, title);
	if (graph()->mlx_win == NULL)
		return (-1);
	return (0);
}

int	graph_init_ctxs(t_vec2i win_size)
{
	graph()->back_ctx = new_context(win_size);
	if (graph()->back_ctx == NULL)
		return (-1);
	return (0);
}

void	clean_graph(void)
{
	if (graph()->mlx_ptr == NULL)
		return ;
	if (graph()->mlx_win != NULL)
		mlx_destroy_window(graph()->mlx_ptr, graph()->mlx_win);
	free_context(graph()->back_ctx);
}
