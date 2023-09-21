/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simpleWindow.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 14:32:15 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/21 18:40:56 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLEWINDOW_H
# define SIMPLEWINDOW_H

typedef struct s_window	t_window;

t_window	*new_window(const char *title, unsigned int width, unsigned int height);
void		*get_pixel_buffer(t_window *window);
void		delete_window(t_window *window);

void		start_main_loop(void (*func)(void *), void *data);

char		*swin_strerr(void);

#endif // SIMPLEWINDOW_H