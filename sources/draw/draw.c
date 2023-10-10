/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:13:53 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 17:30:41 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"
#include "draw.h"
#include "context.h"

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

void	draw_pixel(t_ctx *ctx, t_vec2i pos, t_uint32 color)
{
	static t_color	prev_b = {0};
	static t_color	prev_f = {0};
	static t_color	prev_res = {0};

	if (pos.x < 0 || pos.y < 0)
		return ;
	if (pos.x >= ctx_size(ctx).x || pos.y >= ctx_size(ctx).y)
		return ;
	if ((t_color){color}.alpha == 255)
		return ;
	if ((t_color){color}.alpha == 0)
		return ((void)(*(px(ctx, pos)) = color));
	if (*(px(ctx, pos)) != prev_b.raw || color != prev_f.raw)
	{
		prev_b = (t_color){
			*(px(ctx, pos))};
		prev_f = (t_color){color};
		prev_res = apha_compos(prev_b, prev_f);
	}
	*(px(ctx, pos)) = prev_res.raw;
}

void	draw_rect(t_ctx *ctx, t_vec2i pos, t_vec2i size, t_uint32 color)
{
	t_vec2i	curr;

	curr.x = 0;
	while (curr.x < size.x)
	{
		curr.y = 0;
		while (curr.y < size.y)
		{
			draw_pixel(ctx, add_vi2vi2(curr, pos), color);
			curr.y++;
		}
		curr.x++;
	}
}
