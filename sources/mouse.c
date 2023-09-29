/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/22 00:37:37 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/27 22:20:20 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_intenal.h"

t_pos	get_mouse_pos(t_win *win)
{
	t_pos	mouse_pos;

	mlx_mouse_get_pos(win->mlx_win, &mouse_pos.x, &mouse_pos.y);
	return (mouse_pos);
}

void	set_mouse_pos(t_win *win, t_pos pos)
{
	mlx_mouse_move(win->mlx_win, pos.x, pos.y);
}
