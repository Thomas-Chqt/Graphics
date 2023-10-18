/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 11:52:05 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/18 16:46:57 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "context.h"

t_ctx	*new_context(t_vec2i size)
{
	t_ctx	*new_ctx;
	int		u;

	new_ctx = malloc(sizeof(t_ctx));
	if (new_ctx == NULL)
		return (NULL);
	*new_ctx = (t_ctx){};
	new_ctx->mlx_image = mlx_new_image(graph()->mlx_ptr, size.x, size.y);
	if (new_ctx->mlx_image == NULL)
		return (free_context(new_ctx), NULL);
	new_ctx->pixels = (t_uint32 *)mlx_get_data_addr(new_ctx->mlx_image,
			&u, &u, &u);
	if (new_ctx->pixels == NULL)
		return (free_context(new_ctx), NULL);
	new_ctx->size = size;
	clear_ctx(new_ctx);
	if (compute_vstripes(new_ctx) != 0)
		return (free_context(new_ctx), NULL);
	return (new_ctx);
}

t_ctx	*ctx_from_img(char *path)
{
	t_ctx	*new_ctx;
	void	*mlx_img;
	t_vec2i	size;

	if (ft_strrchr(path, '.') == NULL)
		return (errno = ENOENT, NULL);
	if (str_cmp(ft_strrchr(path, '.'), ".xpm") == 0)
		mlx_img = mlx_xpm_file_to_image(
				graph()->mlx_ptr, path, &size.x, &size.y);
	else if (str_cmp(ft_strrchr(path, '.'), ".png") == 0)
		mlx_img = mlx_png_file_to_image(
				graph()->mlx_ptr, path, &size.x, &size.y);
	else
		return (errno = ENOENT, NULL);
	if (mlx_img == NULL)
		return (NULL);
	new_ctx = ctx_from_mlx_img(mlx_img, size);
	if (new_ctx == NULL)
		return (mlx_destroy_image(graph()->mlx_ptr, mlx_img),
			errno = ENOMEM, NULL);
	if (compute_vstripes(new_ctx) != 0)
		return (free_context(new_ctx), NULL);
	return (new_ctx);
}

void	free_context(t_ctx *context)
{
	int	i;

	if (context == NULL)
		return ;
	if (context->mlx_image != NULL)
		mlx_destroy_image(graph()->mlx_ptr, context->mlx_image);
	if (context->vstripes != NULL)
	{
		i = 0;
		while (i < context->size.x)
			free(context->vstripes[i++]);
		free(context->vstripes);
	}
	free(context);
}
