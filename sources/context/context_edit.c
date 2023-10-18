/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_edit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 12:45:39 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/18 20:24:55 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "context.h"

void	fill_ctx(t_ctx *ctx, t_uint32 color)
{
	t_vec2i	pos;

	pos.x = 0;
	while (pos.x < ctx->size.x)
	{
		pos.y = 0;
		while (pos.y < ctx->size.y)
		{
			draw_pixel(ctx, pos, color);
			pos.y++;
		}
		pos.x++;
	}
}

void	clear_ctx(t_ctx *context)
{
	t_vec2i	pos;

	pos.x = 0;
	while (pos.x < context->size.x)
	{
		pos.y = 0;
		while (pos.y < context->size.y)
		{
			*(px(context, pos)) = TRANSP;
			pos.y++;
		}
		pos.x++;
	}
}

void	clear_pixels(t_ctx *context, t_uint32 _px)
{
	t_vec2i	pos;

	pos.x = 0;
	while (pos.x < context->size.x)
	{
		pos.y = 0;
		while (pos.y < context->size.y)
		{
			if (*(px(context, pos)) == _px)
			{
				*(px(context, pos)) = TRANSP;
				context->vstripes[pos.x][pos.y] = TRANSP;
			}
			pos.y++;
		}
		pos.x++;
	}
}
