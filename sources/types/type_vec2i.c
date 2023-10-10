/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_vec2i.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:27:40 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/09 22:09:26 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vec2f	vi2tovf2(t_vec2i veci)
{
	return ((t_vec2f){
		.x = (float)veci.x,
		.y = (float)veci.y
	});
}

t_vec2i	add_vi2vi2(t_vec2i a, t_vec2i b)
{
	return ((t_vec2i){
		.x = a.x + b.x,
		.y = a.y + b.y
	});
}

t_vec2i	sub_vi2vi2(t_vec2i a, t_vec2i b)
{
	return ((t_vec2i){
		.x = a.x - b.x,
		.y = a.y - b.y
	});
}

t_vec2i	mul_vi2vi2(t_vec2i a, t_vec2i b)
{
	return ((t_vec2i){
		.x = a.x * b.x,
		.y = a.y * b.y
	});
}

t_vec2i	div_vi2vi2(t_vec2i a, t_vec2i b)
{
	return ((t_vec2i){
		.x = (int)((float)a.x / (float)b.x),
		.y = (int)((float)a.y / (float)b.y)
	});
}
