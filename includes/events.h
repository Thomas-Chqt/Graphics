/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   events.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/22 13:57:37 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 18:42:42 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTS_H
# define EVENTS_H

# include "simpleWindow_intenal.h"

int	mlx_on_keydown_hook_func(int keycode, void *param);
int	mlx_on_keydup_hook_func(int keycode, void *param);

int	mlx_on_mousedown_hook_func(int button, int x, int y, void *param);
int	mlx_on_mouseup_hook_func(int button, int x, int y, void *param);

int	mlx_on_mousemove_hook_func(int x, int y, void *param);

int	mlx_on_expose_hook_func(void *param);
int	mlx_on_destroy_hook_func(void *param);

#endif // EVENTS_H