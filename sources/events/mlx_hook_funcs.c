/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hook_funcs.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 21:56:21 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/24 14:47:44 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Graphics_internal.h"
#include "events.h"

int	kdo_hook(int keycode, void *data)
{
	(void)data;
	lst_iterdata(
		graph()->kdo_lst,
		(void (*)(void *, void *)) & exec_event,
		&(t_event){.key = keycode});
	if (graph()->is_poll_key_active == false)
		return (0);
	if (lstint_chreq(graph()->pressed, keycode) == NULL)
		ft_lstadd_front(&graph()->pressed, lstint_new(keycode));
	return (0);
}

int	kup_hook(int keycode, void *data)
{
	(void)data;
	lst_iterdata(
		graph()->kup_lst,
		(void (*)(void *, void *)) & exec_event,
		&(t_event){.key = keycode});
	if (graph()->is_poll_key_active == false)
		return (0);
	lstint_delifeq(&graph()->pressed, keycode);
	return (0);
}

int	mdo_hook(int button, int x, int y, void *data)
{
	(void)x;
	(void)y;
	(void)data;
	button *= 1000;
	lst_iterdata(
		graph()->mdo_lst,
		(void (*)(void *, void *)) & exec_event,
		&(t_event){.key = button});
	if (button == W_UP || button == W_DOWN || button == 6 || button == 7)
		return (0);
	if (graph()->is_poll_key_active == false)
		return (0);
	if (lstint_chreq(graph()->pressed, button) == NULL)
		ft_lstadd_front(&graph()->pressed, lstint_new(button));
	return (0);
}

int	mup_hook(int button, int x, int y, void *data)
{
	(void)x;
	(void)y;
	(void)data;
	button *= 1000;
	lst_iterdata(
		graph()->mup_lst,
		(void (*)(void *, void *)) & exec_event,
		&(t_event){.key = button});
	if (graph()->is_poll_key_active == false)
		return (0);
	lstint_delifeq(&graph()->pressed, button);
	return (0);
}
