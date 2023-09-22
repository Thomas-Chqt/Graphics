/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hook_funcs2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/22 14:01:43 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 18:47:24 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "events.h"

int	mlx_on_mousemove_hook_func(int x, int y, void *param)
{
	t_list	*watched;
	t_event	*tmp;

	(void)x;
	(void)y;
	watched = ((t_window *)param)->event_lists;
	while (((t_window *)param)->event_lists != NULL && watched != NULL)
	{
		tmp = (t_event *)watched->data;
		if ((tmp->act.triggers & ON_MOUSEMOVE) == ON_MOUSEMOVE)
			tmp->func(tmp->data);
		watched = watched->next;
	}
	return (0);
}

int	mlx_on_expose_hook_func(void *param)
{
	t_list	*watched;
	t_event	*tmp;

	watched = ((t_window *)param)->event_lists;
	while (((t_window *)param)->event_lists != NULL && watched != NULL)
	{
		tmp = (t_event *)watched->data;
		if ((tmp->act.triggers & ON_EXPOSE) == ON_EXPOSE)
			tmp->func(tmp->data);
		watched = watched->next;
	}
	return (0);
}

int	mlx_on_destroy_hook_func(void *param)
{
	t_list	*watched;
	t_event	*tmp;

	watched = ((t_window *)param)->event_lists;
	while (((t_window *)param)->event_lists != NULL && watched != NULL)
	{
		tmp = (t_event *)watched->data;
		if ((tmp->act.triggers & ON_DESTROY) == ON_DESTROY)
			tmp->func(tmp->data);
		watched = watched->next;
	}
	return (0);
}
