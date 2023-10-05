/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hook_funcs.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/29 13:30:21 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/05 18:01:17 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

int	kdo_hook(int keycode, t_win *win)
{
	t_list	*curr;
	t_event	*tmp;

	curr = win->kdo_lst;
	while (curr != NULL)
	{
		tmp = (t_event *)curr->data;
		if (tmp->key == keycode)
			tmp->func.ptr(tmp->func.data);
		curr = curr->next;
	}
	if (lstint_chreq(win->pressed, keycode) != NULL)
		return (0);
	ft_lstadd_front(&win->pressed, lstint_new(keycode));
	return (0);
}

int	kup_hook(int keycode, t_win *win)
{
	t_list	*curr;
	t_event	*tmp;

	curr = win->kup_lst;
	while (curr != NULL)
	{
		tmp = (t_event *)curr->data;
		if (tmp->key == keycode)
			tmp->func.ptr(tmp->func.data);
		curr = curr->next;
	}
	lstint_delifeq(&win->pressed, keycode);
	return (0);
}

int	mdo_hook(int button, int x, int y, t_win *win)
{
	t_list	*curr;
	t_event	*tmp;

	(void)x;
	(void)y;
	curr = win->mdo_lst;
	while (curr != NULL)
	{
		tmp = (t_event *)curr->data;
		if (tmp->key == button)
			tmp->func.ptr(tmp->func.data);
		curr = curr->next;
	}
	if (button == W_UP || button == W_DOWN)
		return (0);
	if (lstint_chreq(win->pressed, button) != NULL)
		return (0);
	ft_lstadd_front(&win->pressed, lstint_new(button));
	return (0);
}

int	mup_hook(int button, int x, int y, t_win *win)
{
	t_list	*curr;
	t_event	*tmp;

	(void)x;
	(void)y;
	curr = win->mup_lst;
	while (curr != NULL)
	{
		tmp = (t_event *)curr->data;
		if (tmp->key == button)
			tmp->func.ptr(tmp->func.data);
		curr = curr->next;
	}
	lstint_delifeq(&win->pressed, button);
	return (0);
}
