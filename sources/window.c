/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 21:15:16 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/05 19:06:10 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

t_win	*new_window(char *title, t_wh sz)
{
	t_win	*new_win;
	t_list	*new_node;

	if (init_swglob() != 0)
		return (NULL);
	new_win = malloc(sizeof(t_win));
	if (new_win == NULL)
		return (NULL);
	*new_win = (t_win){.back_ctx = new_context(sz)};
	if (new_win->back_ctx == NULL)
		return (delete_window(new_win), NULL);
	new_win->mlx_win = mlx_new_window(swglob()->mlx_ptr, sz.w, sz.h, title);
	if (new_win->mlx_win == NULL)
		return (delete_window(new_win), NULL);
	new_node = ft_lstnew(new_win);
	if (new_node == NULL)
		return (delete_window(new_win), NULL);
	ft_lstadd_front(&(swglob()->win_lst), new_node);
	add_event(new_win, 0, ON_DESTROY, (t_func){
		(void (*)(void *)) & delete_window, new_win});
	return (new_win);
}

void	set_destructor(t_win *window, t_func func)
{
	window->destr = func;
}

void	set_backcolor(t_win *window, t_uint32 color)
{
	fill_ctx(window->back_ctx, color);
}

void	delete_window(t_win *window)
{
	lst_delif(&(swglob()->win_lst),
		(void (*)(void *)) & free_win, &is_same, window);
	if (swglob()->win_lst == NULL)
		exit(0);
}
