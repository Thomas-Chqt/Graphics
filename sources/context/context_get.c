/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_get.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 15:16:17 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/23 19:30:49 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "context.h"

t_vec2i	ctx_size(t_ctx *ctx)
{
	return (ctx->size);
}

t_uint32	ctx_px(t_ctx *context, t_vec2i pos)
{
	return (*px(context, pos));
}

t_stripe	ctx_vstripe(t_ctx *context, int x)
{
	if (x < 0)
		return ((t_stripe){.px = NULL});
	if (x >= ctx_size(context).x)
		return ((t_stripe){.px = NULL});
	return ((t_stripe){context->vstripes[x], context->size.y});
}
