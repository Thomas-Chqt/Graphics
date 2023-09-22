/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 14:32:00 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 19:57:37 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_intenal.h"

static t_window	*add_to_window_list(t_window *win);

t_window	*new_window(const char *title, unsigned int width,
				unsigned int height)
{
	t_window	*new_win;

	if (title == NULL || width > INT_MAX || height > INT_MAX)
		return (set_last_err_ptr(INPUT_ERROR, NULL));
	if (init_g_data() != 0)
		return (NULL);
	new_win = malloc(sizeof(t_window));
	if (new_win == NULL)
		return (set_last_err_ptr(MALLOC_ERROR, NULL));
	*new_win = (t_window){.image_h = height, .image_w = width};
	new_win->mlx_win = mlx_new_window(global_data()->mlx_ptr, (int)width,
			(int)height, (char *)title);
	if (new_win->mlx_win == NULL)
	{
		delete_window(new_win);
		return (set_last_err_ptr(WINDOW_CREATION_ERROR, NULL));
	}
	add_event(new_win, (t_eveact){.triggers = ON_DESTROY},
		(void (*)(void *))&delete_window, new_win);
	return (add_to_window_list(new_win));
}

static t_window	*add_to_window_list(t_window *win)
{
	t_list	*new_node;

	new_node = ft_lstnew(win);
	if (new_node == NULL)
	{
		delete_window(win);
		return (set_last_err_ptr(MALLOC_ERROR, NULL));
	}
	ft_lstadd_front(&(global_data()->window_list), new_node);
	return ((t_window *)win);
}

void	*get_pixel_buffer(t_window *window)
{
	int		bits_per_pixel;
	int		size_line;
	int		endian;

	if (window == NULL)
		return (set_last_err_ptr(INPUT_ERROR, NULL));
	if (window->pixels == NULL)
	{
		window->mlx_image = mlx_new_image(global_data()->mlx_ptr,
				window->image_w, window->image_h);
		if (window->mlx_image == NULL)
			return (set_last_err_ptr(IMAGE_CREATION_ERROR, NULL));
		window->pixels = mlx_get_data_addr(window->mlx_image, &bits_per_pixel,
				&size_line, &endian);
	}
	return (window->pixels);
}

void	add_destructor(t_window *window, void (*func)(void*), void *data)
{
	if (window == NULL)
		return ((void)set_last_err(INPUT_ERROR));
	window->destructor = func;
	window->destructor_data = data;
}
