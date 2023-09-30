/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xpm.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/29 22:12:07 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/29 22:25:47 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

void	*load_xpm(char *file, t_wh *size)
{
	void	*mlx_image;
	int		useless;

	mlx_image = mlx_xpm_file_to_image(swglob()->mlx_ptr, file,
			(int *) & size->w, (int *) & size->h);
	if (mlx_image == NULL)
		return (NULL);
	return (mlx_get_data_addr(mlx_image, &useless, &useless, &useless));
}
