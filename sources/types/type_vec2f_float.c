/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_vec2f_float.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 17:21:42 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 17:23:14 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vec2f	mul_vf2f(t_vec2f vec, float f)
{
	return ((t_vec2f){
		.x = vec.x * f,
		.y = vec.y * f
	});
}
