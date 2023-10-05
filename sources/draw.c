/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 17:44:25 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/05 19:05:52 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

void	put_pixel(t_ctx *ctx, t_pos pos, t_uint32 color)
{
	static t_color	prev_b = {0};
	static t_color	prev_f = {0};
	static t_color	prev_res = {0};

	if (pos.x >= (int)ctx->size.w || pos.y >= (int)ctx->size.h
		|| pos.x < 0 || pos.y < 0 || (t_color){color}.alpha == 255)
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
