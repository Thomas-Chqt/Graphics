/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Graphics.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 21:18:18 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 13:49:53 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

static void	close_win_zero(void *data);

int	create_window(char *title, t_vec2i size)
{
	if (graph_init_mlx() != 0)
		return (-1);
	if (graph_init_window(title, size) != 0)
		return (-1);
	if (graph_init_ctxs(size) != 0)
		return (clean_graph(), -1);
	if (add_event(0, ON_DESTROY, &close_win_zero, NULL) != 0)
		return (clean_graph(), -1);
	if (add_event(ESC_KEY, ON_KEYDOWN, &close_win_zero, NULL) != 0)
		return (clean_graph(), -1);
	return (0);
}

void	set_destructor(void (*func)(void *), void *data)
{
	graph()->destr = func;
	graph()->destr_data = data;
}

void	set_back_color(t_uint32 color)
{
	clear_ctx(graph()->back_ctx);
	fill_ctx(graph()->back_ctx, color);
}

void	delete_window(int return_code)
{
	if (graph()->destr != NULL)
		graph()->destr(graph()->destr_data);
	ft_lstclear(&graph()->kdo_lst, &free_wrap);
	ft_lstclear(&graph()->kup_lst, &free_wrap);
	ft_lstclear(&graph()->mdo_lst, &free_wrap);
	ft_lstclear(&graph()->mup_lst, &free_wrap);
	ft_lstclear(&graph()->mov_lst, &free_wrap);
	ft_lstclear(&graph()->exp_lst, &free_wrap);
	ft_lstclear(&graph()->des_lst, &free_wrap);
	ft_lstclear(&graph()->pressed, &free_wrap);
	clean_graph();
	exit(return_code);
}

static void	close_win_zero(void *data)
{
	(void)data;
	delete_window(0);
}
