/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/10 17:14:45 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 17:19:56 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTEXT_H
# define CONTEXT_H

# include "Graphics_internal.h"

struct s_context
{
	void	*mlx_image;
	void	*pixels;
	t_vec2i	size;
};

t_uint32	*px(t_ctx *context, t_vec2i pos);
t_ctx		*ctx_from_mlx_img(void *mlx_img, t_vec2i size);

#endif // CONTEXT_H