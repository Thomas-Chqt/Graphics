/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/22 00:37:37 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 18:47:29 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_intenal.h"

t_mouse_pos	get_mouse_pos(t_window *window)
{
	t_mouse_pos	pos;

	mlx_mouse_get_pos(window->mlx_win, &pos.x, &pos.y);
	return (pos);
}
