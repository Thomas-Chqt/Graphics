/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 21:24:21 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 12:26:08 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"
#include "event.h"

static void		init_hooks(void);
static t_list	*lstev_new(int key, int trig, void (*func)(void *), void *data);

int	add_event(int key, int trig, void (*func)(void *), void *data)
{
	t_list	*new_node;

	init_hooks();
	new_node = lstev_new(key, trig, func, data);
	if (new_node == NULL)
		return (1);
	if (key < W_UP && trig == ON_KEYDOWN)
		ft_lstadd_front(&graph()->kdo_lst, new_node);
	if (key < W_UP && trig == ON_KEYUP)
		ft_lstadd_front(&graph()->kup_lst, new_node);
	if (key >= W_UP && trig == ON_KEYDOWN)
		ft_lstadd_front(&graph()->mdo_lst, new_node);
	if (key >= W_UP && trig == ON_KEYUP)
		ft_lstadd_front(&graph()->mup_lst, new_node);
	if (trig == ON_MOUSEMOVE)
		ft_lstadd_front(&graph()->mov_lst, new_node);
	if (trig == ON_EXPOSE)
		ft_lstadd_front(&graph()->exp_lst, new_node);
	if (trig == ON_DESTROY)
		ft_lstadd_front(&graph()->des_lst, new_node);
	return (0);
}

int	poll_key(int *key)
{
	if (graph()->pres_curr == NULL)
		return (0);
	*key = *((int *)graph()->pres_curr->data);
	graph()->pres_curr = graph()->pres_curr->next;
	return (1);
}

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

static void	init_hooks(void)
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

static t_list	*lstev_new(int key, int trig, void (*func)(void *), void *data)
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
