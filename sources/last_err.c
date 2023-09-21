/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   last_err.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 17:04:39 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/21 18:09:09 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_intenal.h"

int	*last_err(void)
{
	static int	last_error = 0;

	return (&last_error);
}

void	*set_last_err_ptr(int nbr, void *ret)
{
	*(last_err()) = nbr;
	return (ret);
}

int	set_last_err(int nbr)
{
	*(last_err()) = nbr;
	return (nbr);
}

char	*swin_strerr(void)
{
	static char	malloc_error[] = "Malloc error";
	static char	mlx_init_error[] = "Fail to initialize mlx";
	static char	input_error[] = "Bad input";
	static char	window_creation_error[] = "Fail to create a window";

	if (*(last_err()) == MALLOC_ERROR)
		return (malloc_error);
	if (*(last_err()) == MLX_INIT_ERROR)
		return (mlx_init_error);
	if (*(last_err()) == INPUT_ERROR)
		return (input_error);
	if (*(last_err()) == WINDOW_CREATION_ERROR)
		return (window_creation_error);
	return (NULL);
}
