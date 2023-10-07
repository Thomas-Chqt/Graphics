/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 16:51:10 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 19:07:44 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_swglob	*swglob(void)
{
	static t_swglob	simple_window_global = {};

	return (&simple_window_global);
}

int	init_swglob(void)
{
	if (swglob()->mlx_ptr != NULL)
		return (0);
	swglob()->mlx_ptr = mlx_init();
	if (swglob()->mlx_ptr == NULL)
		return (1);
	return (0);
}
