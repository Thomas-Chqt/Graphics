/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/06 11:32:49 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/06 15:30:10 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simpleWindow_internal.h"

t_pos	addpos(t_pos a, t_pos b)
{
	return ((t_pos){
		.x = a.x + b.x,
		.y = a.y + b.y
	});
}

t_wh	addhwi(t_wh a, int i)
{
	return ((t_wh){
		.w = a.w + i,
		.h = a.h + i
	});
}

t_pos	mulposwh(t_pos a, t_wh b)
{
	return ((t_pos){
		.x = a.x * b.w,
		.y = a.y * b.h
	});
}

t_wh	divwhi(t_wh a, int i)
{
	return ((t_wh){
		.w = a.w / i,
		.h = a.h / i
	});
}
