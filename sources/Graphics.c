/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Graphics.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 21:18:18 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/18 14:19:08 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

static void	destructor_exit(void *data);

int	create_window(char *title, int width, int height)
{
	if (graph_init_mlx() != 0)
		return (-1);
	if (graph_init_window(title, (t_vec2i){width, height}) != 0)
		return (-1);
	if (graph_init_ctxs((t_vec2i){width, height}) != 0)
		return (clean_graph(), -1);
	if (add_event(0, ON_DESTROY, &destructor_exit, NULL) != 0)
		return (clean_graph(), -1);
	if (add_event(ESC_KEY, ON_KEYDOWN, &destructor_exit, NULL) != 0)
		return (clean_graph(), -1);
	return (0);
}

t_ctx	*back_ctx(void)
{
	return (graph()->back_ctx);
}

void	set_destructor(void (*func)(void *), void *data)
{
	graph()->destr = func;
	graph()->destr_data = data;
}

void	delete_window(void)
{
	ft_lstclear(&graph()->kdo_lst, &free_wrap);
	ft_lstclear(&graph()->kup_lst, &free_wrap);
	ft_lstclear(&graph()->mdo_lst, &free_wrap);
	ft_lstclear(&graph()->mup_lst, &free_wrap);
	ft_lstclear(&graph()->mov_lst, &free_wrap);
	ft_lstclear(&graph()->exp_lst, &free_wrap);
	ft_lstclear(&graph()->des_lst, &free_wrap);
	ft_lstclear(&graph()->pressed, &free_wrap);
	clean_graph();
}

static void	destructor_exit(void *data)
{
	(void)data;
	if (graph()->destr != NULL)
		graph()->destr(graph()->destr_data);
	exit(0);
}
