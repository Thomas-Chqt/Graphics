/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:43:43 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 10:45:37 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vi2d	mouse_pos(void)
{
	t_vi2d	mouse_pos;

	mlx_mouse_get_pos(graph()->mlx_win, &mouse_pos.x, &mouse_pos.y);
		return (mouse_pos);
}

void	set_mouse_pos(t_vi2d pos)
{
	mlx_mouse_move(graph()->mlx_win, pos.x, pos.y);
}
