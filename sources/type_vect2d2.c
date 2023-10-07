/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_vect2d2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/06 19:41:45 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 19:07:44 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vect2d	addvect2d(t_vect2d a, t_vect2d b)
{
	return ((t_vect2d){
		.x = a.x + b.x,
		.y = a.y + b.y
	});
}

t_vect2d	rotvect2d(t_vect2d a, float rad)
{
	return ((t_vect2d){
		.x = a.x * cosf(rad) - a.y * sinf(rad),
		.y = a.x * sinf(rad) + a.y * cosf(rad)
	});
}

t_vect2d	multvect2df(t_vect2d vect, float f)
{
	return ((t_vect2d){
		.x = vect.x * f,
		.y = vect.y * f
	});
}