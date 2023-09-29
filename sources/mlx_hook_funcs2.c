/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hook_funcs2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/29 13:30:46 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/29 18:49:55 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

int	mov_hook(int x, int y, t_win *win)
{
	t_list	*curr;
	t_event	*tmp;

	(void)x;
	(void)y;
	curr = win->mov_lst;
	while (curr != NULL)
	{
		tmp = (t_event *)curr->data;
		tmp->func.ptr(tmp->func.data);
		curr = curr->next;
	}
	return (0);
}

int	exp_hook(t_win *win)
{
	t_list	*curr;
	t_event	*tmp;

	curr = win->exp_lst;
	while (curr != NULL)
	{
		tmp = (t_event *)curr->data;
		tmp->func.ptr(tmp->func.data);
		curr = curr->next;
	}
	return (0);
}

int	des_hook(t_win *win)
{
	t_list	*curr;
	t_event	*tmp;

	curr = win->des_lst;
	while (curr != NULL)
	{
		tmp = (t_event *)curr->data;
		tmp->func.ptr(tmp->func.data);
		curr = curr->next;
	}
	return (0);
}
