/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 18:42:18 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/05 19:06:05 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

void	free_win(t_win *win)
{
	if (win->mlx_win != NULL)
		mlx_destroy_window(swglob()->mlx_ptr, win->mlx_win);
	ft_lstclear(&win->kdo_lst, &free_wrap);
	ft_lstclear(&win->kup_lst, &free_wrap);
	ft_lstclear(&win->mdo_lst, &free_wrap);
	ft_lstclear(&win->mup_lst, &free_wrap);
	ft_lstclear(&win->mov_lst, &free_wrap);
	ft_lstclear(&win->exp_lst, &free_wrap);
	ft_lstclear(&win->des_lst, &free_wrap);
	ft_lstclear(&win->pressed, &free_wrap);
	if (win->destr.ptr != NULL)
		win->destr.ptr(win->destr.data);
	free(win);
}
