/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/22 13:34:57 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 23:46:01 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "events.h"

static void	init_events(t_window *window);

int	add_event(t_window *window, t_eveact activation, void (*func)(void *),
				void *data)
{
	static int	event_id = 0;
	t_list		*new_node;

	if (window == NULL || func == NULL)
	{
		(void)set_last_err(INPUT_ERROR);
		return (0);
	}
	init_events(window);
	new_node = ft_lstnew(NULL);
	if (new_node != NULL)
	{
		new_node->data = malloc(sizeof(t_event));
		if (new_node->data != NULL)
		{
			*((t_event *)new_node->data) = (t_event){.id = event_id++,
				.act = activation, .func = func, . data = data
			};
			ft_lstadd_front(&window->event_lists, new_node);
			return (((t_event *)new_node->data)->id);
		}
		free(new_node);
	}
	(void)set_last_err(MALLOC_ERROR);
	return (0);
}

static void	init_events(t_window *window)
{
	if (window->is_envent_init)
		return ;
	mlx_hook(window->mlx_win, 2, 0, &mlx_on_keydown_hook_func, window);
	mlx_hook(window->mlx_win, 3, 0, &mlx_on_keydup_hook_func, window);
	mlx_hook(window->mlx_win, 4, 0, &mlx_on_mousedown_hook_func, window);
	mlx_hook(window->mlx_win, 5, 0, &mlx_on_mouseup_hook_func, window);
	mlx_hook(window->mlx_win, 6, 0, &mlx_on_mousemove_hook_func, window);
	mlx_hook(window->mlx_win, 12, 0, &mlx_on_expose_hook_func, window);
	mlx_hook(window->mlx_win, 17, 0, &mlx_on_destroy_hook_func, window);
	window->is_envent_init = true;
}
