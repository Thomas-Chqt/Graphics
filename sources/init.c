/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 14:32:00 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 18:46:21 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_intenal.h"

static t_window	*add_to_window_list(t_window *win);
static void		free_window(void *win);

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

void	delete_window(void *window)
{
	if (window == NULL)
		return ((void)set_last_err(INPUT_ERROR));
	lst_delif(&(global_data()->window_list), &free_window, &is_same, window);
	if (global_data()->window_list == NULL)
	{
		clean_g_data();
		exit(0);
	}
}

static void	free_window(void *win)
{
	if (((t_window *)win)->mlx_win != NULL)
		mlx_destroy_window(global_data()->mlx_ptr, ((t_window *)win)->mlx_win);
	if (((t_window *)win)->mlx_image != NULL)
		mlx_destroy_image(global_data()->mlx_ptr, ((t_window *)win)->mlx_image);
	ft_lstclear(&(((t_window *)win)->event_lists), &free_wrap);
	free(win);
}
