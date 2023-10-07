/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_vect2d.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/06 19:33:21 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 19:07:44 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vect2d	addvect2df(t_vect2d vect2d, float f)
{
	return ((t_vect2d){
		.x = vect2d.x + f,
		.y = vect2d.y + f,
	});
}

t_vect2d	addvect2dpos(t_vect2d vect2d, t_pos pos)
{
	return ((t_vect2d){
		.x = vect2d.x + pos.x,
		.y = vect2d.y + pos.y
	});
}

t_vect2d	mulvect2dwh(t_vect2d a, t_wh b)
{
	return ((t_vect2d){
		.x = a.x * b.w,
		.y = a.y * b.h
	});
}

t_vect2d	tovect2d(t_pos pos)
{
	return ((t_vect2d){
		.x = (float)pos.x,
		.y = (float)pos.y
	});
}

t_pos	topos(t_vect2d vect2d)
{
	return ((t_pos){
		.x = (int)vect2d.x,
		.y = (int)vect2d.y
	});
}
