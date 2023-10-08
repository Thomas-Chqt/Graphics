/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 12:08:29 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/08 14:01:18 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"

t_uint32	*px(t_ctx *context, t_vec2i pos)
{
	return (((t_uint32 *)context->pixels) + context->size.x * pos.y + pos.x);
}
