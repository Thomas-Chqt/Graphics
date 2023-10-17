/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 13:22:17 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/17 13:23:24 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"
#include "events.h"

void	exec_event(t_event *el, t_event *data)
{
	if (el->trig == ON_MOUSEMOVE)
		return ((void)el->func(el->data));
	if (el->trig == ON_EXPOSE)
		return ((void)el->func(el->data));
	if (el->trig == ON_DESTROY)
		return ((void)el->func(el->data));
	if (el->key == data->key)
		return ((void)el->func(el->data));
}

void	init_hooks(void)
{
	static t_bool	is_init = false;

	if (is_init == true)
		return ;
	mlx_hook(graph()->mlx_win, 2, 0, &kdo_hook, NULL);
	mlx_hook(graph()->mlx_win, 3, 0, &kup_hook, NULL);
	mlx_hook(graph()->mlx_win, 4, 0, &mdo_hook, NULL);
	mlx_hook(graph()->mlx_win, 5, 0, &mup_hook, NULL);
	mlx_hook(graph()->mlx_win, 6, 0, &mov_hook, NULL);
	mlx_hook(graph()->mlx_win, 12, 0, &exp_hook, NULL);
	mlx_hook(graph()->mlx_win, 17, 0, &des_hook, NULL);
	is_init = true;
}

t_list	*lstev_new(int key, int trig, void (*func)(void *), void *data)
{
	t_list	*new_node;

	new_node = ft_lstnew(NULL);
	if (new_node == NULL)
		return (NULL);
	new_node->data = malloc(sizeof(t_event));
	if (new_node->data == NULL)
		return (free(new_node), NULL);
	((t_event *)new_node->data)->key = key;
	((t_event *)new_node->data)->trig = trig;
	((t_event *)new_node->data)->func = func;
	((t_event *)new_node->data)->data = data;
	return (new_node);
}
