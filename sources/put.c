/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   put.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:34:38 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 13:45:42 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

void	put_context(t_ctx *ctx, t_vec2i pos)
{
	mlx_put_image_to_window(
		graph()->mlx_ptr,
		graph()->mlx_win,
		ctx->mlx_image,
		pos.x,
		pos.y
		);
}

void	put_pixel(t_vec2i pos, t_uint32 color)
{
	draw_pixel(
		graph()->draw_ctx,
		pos,
		color
		);
}

void	put_rect(t_vec2i pos, t_vec2i size, t_uint32 color)
{
	draw_rect(
		graph()->draw_ctx,
		pos,
		size,
		color
		);
}

void	put_line(t_vec2i a, t_vec2i b, t_uint32 color)
{
	draw_line(
		graph()->draw_ctx,
		a,
		b,
		color
		);
}
