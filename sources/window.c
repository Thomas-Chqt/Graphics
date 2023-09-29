/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 21:15:16 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/29 18:50:00 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

static void	free_win(t_win *win);

t_win	*new_window(char *title, t_wh sz)
{
	t_win	*new_win;
	t_list	*new_node;

	if (init_swglob() != 0)
		return (NULL);
	new_win = malloc(sizeof(t_win));
	if (new_win == NULL)
		return (NULL);
	*new_win = (t_win){.img_size = sz};
	new_win->mlx_win = mlx_new_window(swglob()->mlx_ptr, sz.w, sz.h, title);
	if (new_win->mlx_win == NULL)
	{
		delete_window(new_win);
		return (NULL);
	}
	new_node = ft_lstnew(new_win);
	if (new_node == NULL)
	{
		delete_window(new_win);
		return (NULL);
	}
	ft_lstadd_front(&(swglob()->win_lst), new_node);
	add_event(new_win, 0, ON_DESTROY, (t_func){
		(void (*)(void *)) & delete_window, new_win});
	return (new_win);
}

void	add_destructor(t_win *window, t_func func)
{
	window->destr = func;
}

void	*get_pixel_buffer(t_win *win)
{
	int	bpp;
	int	sl;
	int	en;

	if (win->pixels == NULL)
	{
		win->mlx_image = mlx_new_image(swglob()->mlx_ptr,
				win->img_size.w, win->img_size.h);
		if (win->mlx_image == NULL)
			return (NULL);
		win->pixels = mlx_get_data_addr(win->mlx_image, &bpp, &sl, &en);
	}
	return (win->pixels);
}

void	delete_window(t_win *window)
{
	lst_delif(&(swglob()->win_lst),
		(void (*)(void *)) & free_win, &is_same, window);
	if (swglob()->win_lst == NULL)
		exit(0);
}

static void	free_win(t_win *win)
{
	if (win->mlx_win != NULL)
		mlx_destroy_window(swglob()->mlx_ptr, win->mlx_win);
	if (win->mlx_image != NULL)
		mlx_destroy_image(swglob()->mlx_ptr, win->mlx_image);
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
