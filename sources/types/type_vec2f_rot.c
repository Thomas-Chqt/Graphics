/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type_vec2f_rot.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 17:33:10 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 17:02:37 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_vec2f	rot_vf2(t_vec2f vec, float rad)
{
	return ((t_vec2f){
		.x = vec.x * cosf(rad) - vec.y * sinf(rad),
		.y = vec.x * sinf(rad) + vec.y * cosf(rad)
	});
}
