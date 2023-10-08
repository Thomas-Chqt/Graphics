/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_vec2f_rot.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 17:33:10 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 17:34:48 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vec2f	rot_vf2d(t_vec2f vec, float rad)
{
	return ((t_vec2f){
		.x = vec.x * cosf(rad) - vec.y * sinf(rad),
		.y = vec.x * sinf(rad) + vec.y * cosf(rad)
	});
}
