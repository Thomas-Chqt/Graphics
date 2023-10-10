/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 17:24:28 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 17:25:16 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

static void	draw_line_more(t_ctx *ctx, t_vec2i curr, t_vec2i b, t_uint32 color);
static void	draw_line_less(t_ctx *ctx, t_vec2i curr, t_vec2i b, t_uint32 color);

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
