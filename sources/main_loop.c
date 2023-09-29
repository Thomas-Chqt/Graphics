/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 21:19:19 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/29 18:49:49 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

static int	loop_hook(void **params);
static void	win_clear(t_win *win);
static void	win_put_image(t_win *win);

void	start_main_loop(t_func func)
{
	mlx_loop_hook(swglob()->mlx_ptr, (int (*)()) & loop_hook,
		(void *[2]){func.ptr, func.data});
	mlx_loop(swglob()->mlx_ptr);
}

static int	loop_hook(void **params)
{
	ft_lstiter(swglob()->win_lst, (void (*)(void *)) & win_clear);
	((void (*)(void *))params[0])(params[1]);
	ft_lstiter(swglob()->win_lst, (void (*)(void *)) & win_put_image);
	return (0);
}

static void	win_clear(t_win *win)
{
	if (win->pixels != NULL)
		ft_bzero(win->pixels, win->img_size.h * win->img_size.w
			* sizeof(t_uintpx));
	win->pres_curr = win->pressed;
}

static void	win_put_image(t_win *win)
{
	if (win->mlx_image != NULL)
		mlx_put_image_to_window(swglob()->mlx_ptr, win->mlx_win,
			win->mlx_image, 0, 0);
}
