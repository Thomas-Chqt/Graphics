/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 21:19:19 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/17 13:03:35 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

static int	loop_hook(void *params);

void	start_main_loop(void (*func)(void *), void *data)
{
	mlx_loop_hook(graph()->mlx_ptr, &loop_hook, (void *[2]){func, data});
	mlx_loop(graph()->mlx_ptr);
}

static int	loop_hook(void *params)
{
	graph()->pres_curr = graph()->pressed;
	put_context(graph()->back_ctx, (t_vec2i){0, 0});
	((void (*)(void *))((void **)params)[0])(((void **)params)[1]);
	return (0);
}
