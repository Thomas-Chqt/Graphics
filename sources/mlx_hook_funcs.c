/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hook_funcs.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/22 13:56:58 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 18:47:07 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "events.h"

int	mlx_on_keydown_hook_func(int keycode, void *param)
{
	t_list	*watched;
	t_event	*tmp;

	watched = ((t_window *)param)->event_lists;
	while (((t_window *)param)->event_lists != NULL && watched != NULL)
	{
		tmp = (t_event *)watched->data;
		if ((tmp->act.triggers & ON_KEYDOWN) == ON_KEYDOWN
			&& tmp->act.key_code == keycode)
			tmp->func(tmp->data);
		watched = watched->next;
	}
	return (0);
}

int	mlx_on_keydup_hook_func(int keycode, void *param)
{
	t_list	*watched;
	t_event	*tmp;

	watched = ((t_window *)param)->event_lists;
	while (((t_window *)param)->event_lists != NULL && watched != NULL)
	{
		tmp = (t_event *)watched->data;
		if ((tmp->act.triggers & ON_KEYUP) == ON_KEYUP
			&& tmp->act.key_code == keycode)
			tmp->func(tmp->data);
		watched = watched->next;
	}
	return (0);
}

int	mlx_on_mousedown_hook_func(int button, int x, int y, void *param)
{
	t_list	*watched;
	t_event	*tmp;

	(void)x;
	(void)y;
	watched = ((t_window *)param)->event_lists;
	while (((t_window *)param)->event_lists != NULL && watched != NULL)
	{
		tmp = (t_event *)watched->data;
		if ((tmp->act.triggers & ON_KEYDOWN) == ON_KEYDOWN
			&& tmp->act.key_code == button)
			tmp->func(tmp->data);
		watched = watched->next;
	}
	return (0);
}

int	mlx_on_mouseup_hook_func(int button, int x, int y, void *param)
{
	t_list	*watched;
	t_event	*tmp;

	(void)x;
	(void)y;
	watched = ((t_window *)param)->event_lists;
	while (((t_window *)param)->event_lists != NULL && watched != NULL)
	{
		tmp = (t_event *)watched->data;
		if ((tmp->act.triggers & ON_KEYUP) == ON_KEYUP
			&& tmp->act.key_code == button)
			tmp->func(tmp->data);
		watched = watched->next;
	}
	return (0);
}
