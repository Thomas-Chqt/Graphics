/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:15:08 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/10 17:30:09 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DRAW_H
# define DRAW_H

# include "Graphics_internal.h"

typedef union u_color
{
	t_uint32	raw;
	struct
	{
		t_uint8	blue;
		t_uint8	green;
		t_uint8	red;
		t_uint8	alpha;
	};
}	t_color;

typedef struct s_fcolor
{
	float	a;
	float	r;
	float	g;
	float	b;
}	t_fcolor;

t_color	apha_compos(t_color back, t_color front);

#endif // DRAW_H