/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:43:43 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/16 19:12:17 by tchoquet         ###   ########.fr       */
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

void	set_mouse_blocking(t_bool new_state)
{
	graph()->m_is_block = new_state;
	if (new_state == true)
		mlx_mouse_hide();
	if (new_state == false)
		mlx_mouse_show();
}
