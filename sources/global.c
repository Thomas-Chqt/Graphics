/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 16:51:10 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/21 18:38:40 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_intenal.h"

t_g_data	*global_data(void)
{
	static t_g_data	global_data = {};

	return (&global_data);
}

int	init_g_data(void)
{
	if (global_data()->mlx_ptr != NULL)
		return (0);
	global_data()->mlx_ptr = mlx_init();
	if (global_data()->mlx_ptr == NULL)
		return (set_last_err(MLX_INIT_ERROR));
	return (0);
}

void	clean_g_data(void)
{
}
