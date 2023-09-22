/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/22 19:44:45 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 19:57:12 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_intenal.h"

static void	free_window(void *win);

void	delete_window(t_window *window)
{
	if (window == NULL)
		return ((void)set_last_err(INPUT_ERROR));
	lst_delif(&(global_data()->window_list), &free_window, &is_same, window);
	if (global_data()->window_list == NULL)
		exit(0);
}

static void	free_window(void *win)
{
	if (((t_window *)win)->mlx_win != NULL)
		mlx_destroy_window(global_data()->mlx_ptr, ((t_window *)win)->mlx_win);
	if (((t_window *)win)->mlx_image != NULL)
		mlx_destroy_image(global_data()->mlx_ptr, ((t_window *)win)->mlx_image);
	if (((t_window *)win)->destructor != NULL)
		((t_window *)win)->destructor(((t_window *)win)->destructor_data);
	ft_lstclear(&(((t_window *)win)->event_lists), &free_wrap);
	free(win);
}


