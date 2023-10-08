/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Graphics.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 21:18:18 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 22:47:12 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

static void	close_win_zero(void *data);

int	create_window(char *title, t_vi2d size)
{
	if (graph_init_mlx() != 0)
		return (-1);
	if (graph_init_window(title, size) != 0)
		return (-1);
	if (add_event(0, ON_DESTROY, &close_win_zero, NULL) != 0)
		return (clean_graph(), -1);
	return (0);
}

void	delete_window(int return_code)
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
	exit(return_code);
}

static void	close_win_zero(void *data)
{
	(void)data;
	delete_window(0);
}
