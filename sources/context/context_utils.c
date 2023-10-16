/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:08:29 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/16 17:23:05 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "context.h"

t_uint32	*px(t_ctx *context, t_vec2i pos)
{
	return (context->pixels + context->size.x * pos.y + pos.x);
}

t_ctx	*ctx_from_mlx_img(void *mlx_img, t_vec2i size)
{
	t_ctx	*new_ctx;
	int		u;

	new_ctx = malloc(sizeof(t_ctx));
	if (new_ctx == NULL)
		return (NULL);
	*new_ctx = (t_ctx){};
	new_ctx->mlx_image = mlx_img;
	new_ctx->pixels = (t_uint32 *)mlx_get_data_addr(new_ctx->mlx_image,
			&u, &u, &u);
	if (new_ctx->pixels == NULL)
		return (free(new_ctx), NULL);
	new_ctx->size = size;
	return (new_ctx);
}

int	compute_vstripes(t_ctx *ctx)
{
	int	i;

	if (ctx->vstripes != NULL)
		return (0);
	ctx->vstripes = malloc(sizeof(t_uint32 *) * ctx->size.x);
	if (ctx->vstripes == NULL)
		return (1);
	i = 0;
	while (i < ctx->size.x)
	{
		ctx->vstripes[i] = malloc(sizeof(t_uint32) * ctx->size.y);
		if (ctx->vstripes[i] == NULL)
			break ;
		i++;
	}
	if (i < ctx->size.x)
	{
		while (i > 0)
			free(ctx->vstripes[i--]);
		free(ctx->vstripes);
		ctx->vstripes = NULL;
		return (1);
	}
	fill_vstripes(ctx);
	return (0);
}

void	fill_vstripes(t_ctx *ctx)
{
	int	x;
	int	y;

	x = 0;
	while (x < ctx->size.x)
	{
		y = 0;
		while (y < ctx->size.y)
		{
			ctx->vstripes[x][y] = *(px(ctx, (t_vec2i){x, y}));
			y++;
		}
		x++;
	}
}
