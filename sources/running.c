/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   running.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 18:34:58 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 18:48:07 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_intenal.h"

static int	main_loop_wrapper(void *param);
static void	window_clear(void *window);
static void	window_put_image(void *window);

void	start_main_loop(void (*func)(void *), void *data)
{
	if (func == NULL)
		return ;
	mlx_loop_hook(global_data()->mlx_ptr, &main_loop_wrapper,
		(void *[2]){func, data});
	mlx_loop(global_data()->mlx_ptr);
}

static int	main_loop_wrapper(void *param)
{
	ft_lstiter(global_data()->window_list, &window_clear);
	((void (*)(void *))(((void **)param)[0]))(((void **)param)[1]);
	ft_lstiter(global_data()->window_list, &window_put_image);
	return (0);
}

static void	window_clear(void *window)
{
	if (((t_window *)window)->pixels != NULL)
		ft_bzero(((t_window *)window)->pixels, ((t_window *)window)->image_h
			* ((t_window *)window)->image_w * 4);
}

static void	window_put_image(void *window)
{
	if (((t_window *)window)->mlx_image != NULL)
		mlx_put_image_to_window(global_data()->mlx_ptr,
			((t_window *)window)->mlx_win,
			((t_window *)window)->mlx_image, 0, 0);
}
