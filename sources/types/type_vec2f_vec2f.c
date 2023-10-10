/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_vec2f_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 16:47:49 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 17:19:37 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vec2f	add_vf2vf2(t_vec2f a, t_vec2f b)
{
	return ((t_vec2f){
		.x = a.x + b.x,
		.y = a.y + b.y
	});
}

t_vec2f	sub_vf2vf2(t_vec2f a, t_vec2f b)
{
	return ((t_vec2f){
		.x = a.x - b.x,
		.y = a.y - b.y
	});
}

t_vec2f	mul_vf2vf2(t_vec2f a, t_vec2f b)
{
	return ((t_vec2f){
		.x = a.x * b.x,
		.y = a.y * b.y
	});
}

t_vec2f	div_vf2vf2(t_vec2f a, t_vec2f b)
{
	return ((t_vec2f){
		.x = a.x / b.x,
		.y = a.y / b.y
	});
}
