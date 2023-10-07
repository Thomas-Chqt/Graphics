/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 21:19:19 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 19:07:44 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

static int	loop_hook(void **params);
static void	win_clear(t_win *win);

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
	return (0);
}

static void	win_clear(t_win *win)
{
	put_ctx_to_win(win, win->back_ctx, (t_pos){0, 0});
	win->pres_curr = win->pressed;
}
