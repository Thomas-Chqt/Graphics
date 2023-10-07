/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/28 00:30:02 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 19:07:44 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

static void		init_hook(t_win *win);
static t_list	*lstev_new(int key, int trig, t_func func);

int	add_event(t_win *win, int key, int trig, t_func func)
{
	t_list	*new_node;

	init_hook(win);
	new_node = lstev_new(key, trig, func);
	if (new_node == NULL)
		return (1);
	if (key > 5 && trig == ON_KEYDOWN)
		ft_lstadd_front(&win->kdo_lst, new_node);
	if (key > 5 && trig == ON_KEYUP)
		ft_lstadd_front(&win->kup_lst, new_node);
	if (key < 6 && trig == ON_KEYDOWN)
		ft_lstadd_front(&win->mdo_lst, new_node);
	if (key < 6 && trig == ON_KEYUP)
		ft_lstadd_front(&win->mup_lst, new_node);
	if (trig == ON_MOUSEMOVE)
		ft_lstadd_front(&win->mov_lst, new_node);
	if (trig == ON_EXPOSE)
		ft_lstadd_front(&win->exp_lst, new_node);
	if (trig == ON_DESTROY)
		ft_lstadd_front(&win->des_lst, new_node);
	return (0);
}

int	poll_key(t_win *win, int *key)
{
	if (win->pres_curr == NULL)
		return (0);
	*key = *((int *)win->pres_curr->data);
	win->pres_curr = win->pres_curr->next;
	return (1);
}

static void	init_hook(t_win *win)
{
	if (win->is_hook_init == true)
		return ;
	mlx_hook(win->mlx_win, 2, 0, (int (*)()) & kdo_hook, win);
	mlx_hook(win->mlx_win, 3, 0, (int (*)()) & kup_hook, win);
	mlx_hook(win->mlx_win, 4, 0, (int (*)()) & mdo_hook, win);
	mlx_hook(win->mlx_win, 5, 0, (int (*)()) & mup_hook, win);
	mlx_hook(win->mlx_win, 6, 0, (int (*)()) & mov_hook, win);
	mlx_hook(win->mlx_win, 12, 0, (int (*)()) & exp_hook, win);
	mlx_hook(win->mlx_win, 17, 0, (int (*)()) & des_hook, win);
	win->is_hook_init = true;
}

static t_list	*lstev_new(int key, int trig, t_func func)
{
	t_list	*new_node;

	new_node = ft_lstnew(NULL);
	if (new_node != NULL)
	{
		new_node->data = malloc(sizeof(t_event));
		if (new_node->data != NULL)
		{
			((t_event *)new_node->data)->key = key;
			((t_event *)new_node->data)->trig = trig;
			((t_event *)new_node->data)->func = func;
			return (new_node);
		}
		free(new_node);
	}
	return (NULL);
}
