/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:08:29 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 17:20:02 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "context.h"

t_uint32	*px(t_ctx *context, t_vec2i pos)
{
	return (((t_uint32 *)context->pixels) + context->size.x * pos.y + pos.x);
}

t_ctx	*ctx_from_mlx_img(void *mlx_img, t_vec2i size)
{
	t_ctx	*new_ctx;
	int		u;

	new_ctx = malloc(sizeof(t_ctx));
	if (new_ctx == NULL)
		return (NULL);
	new_ctx->mlx_image = mlx_img;
	new_ctx->pixels = mlx_get_data_addr(new_ctx->mlx_image, &u, &u, &u);
	if (new_ctx->pixels == NULL)
		return (free(new_ctx), NULL);
	new_ctx->size = size;
	return (new_ctx);
}
