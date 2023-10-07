/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/05 17:46:55 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/07 19:07:44 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_uint32	*px(t_ctx *context, t_pos pos)
{
	return (((t_uint32 *)context->pixels) + context->size.w * pos.y + pos.x);
}

t_color	apha_compos(t_color back, t_color front)
{
	t_fcolor	fback;
	t_fcolor	ffront;
	t_fcolor	fres;

	fback = (t_fcolor){1 - ((float)back.alpha / 255),
		back.red, back.green, back.blue};
	ffront = (t_fcolor){1 - ((float)front.alpha / 255),
		front.red, front.green, front.blue};
	fres.a = ffront.a + fback.a * (1 - ffront.a);
	fres.r = roundf((ffront.r * ffront.a
				+ fback.r * fback.a * (1 - ffront.a)) / fres.a);
	fres.g = roundf((ffront.g * ffront.a
				+ fback.g * fback.a * (1 - ffront.a)) / fres.a);
	fres.b = roundf((ffront.b * ffront.a
				+ fback.b * fback.a * (1 - ffront.a)) / fres.a);
	fres.a = roundf((1 - fres.a) * 255);
	return ((t_color){
		.alpha = fres.a,
		.red = fres.r,
		.green = fres.g,
		.blue = fres.b});
}
