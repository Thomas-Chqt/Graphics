/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:43:43 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/17 13:09:05 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vec2i	mouse_pos(void)
{
	t_vec2i	mouse_pos;

	mlx_mouse_get_pos(graph()->mlx_win, &mouse_pos.x, &mouse_pos.y);
	return (mouse_pos);
}

void	set_mouse_pos(t_vec2i pos)
{
	mlx_mouse_move(graph()->mlx_win, pos.x, pos.y);
}

void	hide_mouse(void)
{
	mlx_mouse_hide();
}

void	show_mouse(void)
{
	mlx_mouse_show();
}
