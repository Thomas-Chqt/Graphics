/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hook_funcs2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 22:03:21 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 17:28:13 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"
#include "events.h"

int	mov_hook(int x, int y, void *data)
{
	(void)x;
	(void)y;
	(void)data;
	lst_iterdata(
		graph()->mov_lst,
		(void (*)(void *, void *)) & exec_event,
		&(t_event){});
	return (0);
}

int	exp_hook(void *data)
{
	(void)data;
	lst_iterdata(
		graph()->exp_lst,
		(void (*)(void *, void *)) & exec_event,
		&(t_event){});
	return (0);
}

int	des_hook(void *data)
{
	(void)data;
	lst_iterdata(
		graph()->des_lst,
		(void (*)(void *, void *)) & exec_event,
		&(t_event){});
	return (0);
}
