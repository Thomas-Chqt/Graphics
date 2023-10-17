/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 21:24:49 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/17 13:24:05 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTS_H
# define EVENTS_H

# include "Graphics_internal.h"

typedef struct s_event
{
	int			key;
	int			trig;
	void		(*func)(void *);
	void		*data;

}	t_event;

void	exec_event(t_event *el, t_event *data);
void	init_hooks(void);
t_list	*lstev_new(int key, int trig, void (*func)(void *), void *data);

int		kdo_hook(int keycode, void *data);
int		kup_hook(int keycode, void *data);
int		mdo_hook(int button, int x, int y, void *data);
int		mup_hook(int button, int x, int y, void *data);
int		mov_hook(int x, int y, void *data);
int		exp_hook(void *data);
int		des_hook(void *data);

#endif // EVENTS_H