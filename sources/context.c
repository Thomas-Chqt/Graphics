/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 11:52:05 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 14:59:08 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_ctx	*new_context(t_vec2i size)
{
	t_ctx	*new_ctx;
	int		u;

	new_ctx = malloc(sizeof(t_ctx));
	if (new_ctx == NULL)
		return (NULL);
	new_ctx->mlx_image = mlx_new_image(graph()->mlx_ptr, size.x, size.y);
	if (new_ctx->mlx_image == NULL)
		return (free_context(new_ctx), NULL);
	new_ctx->pixels = mlx_get_data_addr(new_ctx->mlx_image, &u, &u, &u);
	if (new_ctx->pixels == NULL)
		return (free_context(new_ctx), NULL);
	new_ctx->size = size;
	return (new_ctx);
}

t_vec2i	ctx_size(t_ctx *ctx)
{
	return (ctx->size);
}

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

void	free_context(t_ctx *context)
{
	if (context == NULL)
		return ;
	if (context->mlx_image != NULL)
		mlx_destroy_image(graph()->mlx_ptr, context->mlx_image);
	free(context);
}
