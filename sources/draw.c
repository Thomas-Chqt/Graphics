/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:13:53 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 12:34:33 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"
#include "color.h"

static void	draw_line_more(t_ctx *ctx, t_vec2i curr, t_vec2i b, t_uint32 color);
static void	draw_line_less(t_ctx *ctx, t_vec2i curr, t_vec2i b, t_uint32 color);

void	draw_pixel(t_ctx *ctx, t_vec2i pos, t_uint32 color)
{
	static t_color	prev_b = {0};
	static t_color	prev_f = {0};
	static t_color	prev_res = {0};

	if (pos.x < 0 || pos.y < 0)
		return ;
	if (pos.x >= ctx->size.x || pos.y >= ctx->size.y)
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

void	draw_line(t_ctx *ctx, t_vec2i a, t_vec2i b, t_uint32 color)
{
	if (abs(b.x - a.x) >= abs(b.y - a.y))
		draw_line_more(ctx, a, b, color);
	else
		draw_line_less(ctx, a, b, color);
}

static void	draw_line_more(t_ctx *ctx, t_vec2i curr, t_vec2i b, t_uint32 color)
{
	int		slope;
	int		error;
	int		error_inc;
	t_vec2i	incr;

	slope = 2 * abs(b.y - curr.y);
	error = -abs(b.x - curr.x);
	error_inc = -2 * abs(b.x - curr.x);
	incr = (t_vec2i){-1 + (2 * ((b.x - curr.x) > 0)),
		-1 + (2 * ((b.y - curr.y) > 0))};
	while (curr.x != b.x)
	{
		draw_pixel(ctx, curr, color);
		error += slope;
		if (error >= 0)
		{
			curr.y += incr.y;
			error += error_inc;
		}
		curr.x += incr.x;
	}
}

static void	draw_line_less(t_ctx *ctx, t_vec2i curr, t_vec2i b, t_uint32 color)
{
	int		slope;
	int		error;
	int		error_inc;
	t_vec2i	incr;

	slope = 2 * abs(b.x - curr.x);
	error = -abs(b.y - curr.y);
	error_inc = -2 * abs(b.y - curr.y);
	incr = (t_vec2i){-1 + (2 * ((b.x - curr.x) > 0)),
		-1 + (2 * ((b.y - curr.y) > 0))};
	while (curr.y != b.y)
	{
		draw_pixel(ctx, curr, color);
		error += slope;
		if (error >= 0)
		{
			curr.x += incr.x;
			error += error_inc;
		}
		curr.y += incr.y;
	}
}
