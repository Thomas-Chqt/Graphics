/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   put_line.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/06 19:52:59 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/06 19:53:26 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

static void	put_line_more(t_ctx *ctx, t_pos curr, t_pos b, t_uint32 color);
static void	put_line_less(t_ctx *ctx, t_pos curr, t_pos b, t_uint32 color);

void	put_line(t_ctx *ctx, t_pos curr, t_pos b, t_uint32 color)
{
	if (abs(b.x - curr.x) >= abs(b.y - curr.y))
		put_line_more(ctx, curr, b, color);
	else
		put_line_less(ctx, curr, b, color);
}

static void	put_line_more(t_ctx *ctx, t_pos curr, t_pos b, t_uint32 color)
{
	int		slope;
	int		error;
	int		error_inc;
	t_pos	incr;

	slope = 2 * abs(b.y - curr.y);
	error = -abs(b.x - curr.x);
	error_inc = -2 * abs(b.x - curr.x);
	incr = (t_pos){-1 + (2 * ((b.x - curr.x) > 0)),
		-1 + (2 * ((b.y - curr.y) > 0))};
	while (curr.x != b.x)
	{
		put_pixel(ctx, curr, color);
		error += slope;
		if (error >= 0)
		{
			curr.y += incr.y;
			error += error_inc;
		}
		curr.x += incr.x;
	}
}

static void	put_line_less(t_ctx *ctx, t_pos curr, t_pos b, t_uint32 color)
{
	int		slope;
	int		error;
	int		error_inc;
	t_pos	incr;

	slope = 2 * abs(b.x - curr.x);
	error = -abs(b.y - curr.y);
	error_inc = -2 * abs(b.y - curr.y);
	incr = (t_pos){-1 + (2 * ((b.x - curr.x) > 0)),
		-1 + (2 * ((b.y - curr.y) > 0))};
	while (curr.y != b.y)
	{
		put_pixel(ctx, curr, color);
		error += slope;
		if (error >= 0)
		{
			curr.x += incr.x;
			error += error_inc;
		}
		curr.y += incr.y;
	}
}
