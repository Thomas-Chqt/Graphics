/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 21:24:21 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/24 14:47:34 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"
#include "events.h"

int	add_event(int key, int trig, void (*func)(void *), void *data)
{
	t_list	*new_node;

	init_hooks();
	new_node = lstev_new(key, trig, func, data);
	if (new_node == NULL)
		return (1);
	if (key < L_CLICK && trig == ON_KEYDOWN)
		ft_lstadd_front(&graph()->kdo_lst, new_node);
	if (key < L_CLICK && trig == ON_KEYUP)
		ft_lstadd_front(&graph()->kup_lst, new_node);
	if (key >= L_CLICK && trig == ON_KEYDOWN)
		ft_lstadd_front(&graph()->mdo_lst, new_node);
	if (key >= L_CLICK && trig == ON_KEYUP)
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

void	desac_poll_key(void)
{
	graph()->is_poll_key_active = false;
	ft_lstclear(&graph()->pressed, &free_wrap);
	graph()->pres_curr = NULL;
}

void	react_poll_key(void)
{
	graph()->is_poll_key_active = true;
}
