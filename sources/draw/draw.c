/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:13:53 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/23 19:27:35 by tchoquet         ###   ########.fr       */
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

void	draw_vstripe(t_ctx *ctx, t_stripe stripe, t_vec2i start, t_vec2i end)
{
	float	stripe_step;
	t_vec2i	screen_curr;
	float	stripe_curr;

	if (stripe.px == NULL)
		return ;
	stripe_step = (float)stripe.size / (float)(end.y - start.y);
	if (start.y < 0)
	{
		screen_curr = (t_vec2i){.x = start.x, .y = 0};
		stripe_curr = (float)start.y * -stripe_step;
	}
	else
	{
		screen_curr = start;
		stripe_curr = 0.0f;
	}
	while (screen_curr.y < ctx->size.y && screen_curr.y < end.y)
	{
		if ((int)stripe_curr < stripe.size && (int)stripe_curr >= 0)
			draw_pixel(ctx, screen_curr, stripe.px[(int)stripe_curr]);
		screen_curr.y++;
		stripe_curr += stripe_step;
	}
}
