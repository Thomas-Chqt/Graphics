/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 16:59:30 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/06 14:10:44 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

t_ctx	*new_context(t_wh size)
{
	void	*mlx_img;
	t_ctx	*new_ctx;
	int		useless;

	mlx_img = mlx_new_image(swglob()->mlx_ptr, size.w, size.h);
	if (mlx_img == NULL)
		return (NULL);
	new_ctx = malloc(sizeof(t_ctx));
	if (new_ctx == NULL)
		return (NULL);
	new_ctx->mlx_image = mlx_img;
	new_ctx->size = size;
	new_ctx->pixels = mlx_get_data_addr(mlx_img, &useless, &useless, &useless);
	if (new_ctx->pixels == NULL)
	{
		mlx_destroy_image(swglob()->mlx_ptr, mlx_img);
		free(new_ctx);
		return (NULL);
	}
	clear_ctx(new_ctx);
	return (new_ctx);
}

void	fill_ctx(t_ctx *ctx, t_uint32 color)
{
	int	x;
	int	y;

	x = 0;
	while (x < ctx->size.w)
	{
		y = 0;
		while (y < ctx->size.h)
		{
			put_pixel(ctx, (t_pos){x, y}, color);
			y++;
		}
		x++;
	}
}

void	clear_ctx(t_ctx *context)
{
	t_pos	pos;

	pos.x = 0;
	while (pos.x < context->size.w)
	{
		pos.y = 0;
		while (pos.y < context->size.h)
		{
			*(px(context, pos)) = TRANSP;
			pos.y++;
		}
		pos.x++;
	}
}

void	put_ctx_to_win(t_win *window, t_ctx *ctx, t_pos pos)
{
	mlx_put_image_to_window(swglob()->mlx_ptr,
		window->mlx_win, ctx->mlx_image, pos.x, pos.y);
}

void	delete_ctx(t_ctx *context)
{
	if (context == NULL)
		return ;
	mlx_destroy_image(swglob()->mlx_ptr, context->mlx_image);
	free(context);
}
