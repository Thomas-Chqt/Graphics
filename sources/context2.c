/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 12:45:39 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 14:23:32 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_ctx	*ctx_from_img(char *path)
{
	t_ctx	*new_ctx;
	void	*mlx_img;
	t_vec2i	size;

	if (ft_strrchr(path, '.') == NULL)
		return (NULL);
	if (str_cmp(ft_strrchr(path, '.'), ".xpm") == 0)
		mlx_img = mlx_xpm_file_to_image(
				graph()->mlx_ptr, path, &size.x, &size.y);
	else if (str_cmp(ft_strrchr(path, '.'), ".png") == 0)
		mlx_img = mlx_png_file_to_image(
				graph()->mlx_ptr, path, &size.x, &size.y);
	else
		return (NULL);
	if (mlx_img == NULL)
		return (NULL);
	new_ctx = ctx_from_mlx_img(mlx_img, size);
	if (new_ctx == NULL)
		return (mlx_destroy_image(graph()->mlx_ptr, mlx_img), NULL);
	return (new_ctx);
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

t_uint32	ctx_px(t_ctx *context, t_vec2i pos)
{
	return (*px(context, pos));
}
