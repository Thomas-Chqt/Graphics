/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_vec2f.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 15:23:16 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 16:48:35 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vec2i	vf2tovi2(t_vec2f vecf)
{
	return ((t_vec2i){
		.x = (int)vecf.x,
		.y = (int)vecf.y
	});
}

t_vec2f	add_vf2vi2(t_vec2f vecf, t_vec2i veci)
{
	return ((t_vec2f){
		.x = vecf.x + veci.x,
		.y = vecf.y + veci.y
	});
}

t_vec2f	mul_vf2vi2(t_vec2f vecf, t_vec2i veci)
{
	return ((t_vec2f){
		.x = vecf.x * veci.x,
		.y = vecf.y * veci.y
	});
}
