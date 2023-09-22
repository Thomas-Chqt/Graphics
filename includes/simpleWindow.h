/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simpleWindow.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 14:32:15 by tchoquet          #+#    #+#             */
/*   Updated: 2023/09/22 19:57:10 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIMPLEWINDOW_H
# define SIMPLEWINDOW_H

# define ESC_KEY 53
# define SPACE_KEY 49
# define ONE_KEY 18
# define TWO_KEY 19
# define THREE_KEY 20
# define FOUR_KEY 21
# define FIVE_KEY 23
# define LEFT_KEY 123
# define RIGHT_KEY 124
# define UP_KEY 126
# define DOWN_KEY 125
# define ENTER_KEY 36
# define R_KEY 15
# define PLUS_KEY 24
# define NINUS_KEY 27
# define L_CLICK 1
# define R_CLICK 2
# define M_CLICK 3
# define W_DOWN 4
# define W_UP 5

# define ON_KEYDOWN 0b00000001
# define ON_KEYUP 0b00000010
# define ON_MOUSEMOVE 0b00010000
# define ON_EXPOSE 0b00100000
# define ON_DESTROY 0b01000000

typedef struct s_window		t_window;
typedef struct s_mouse_pos	t_mouse_pos;
typedef struct s_eveact		t_eveact;

struct s_mouse_pos
{
	int	x;
	int	y;
};

struct s_eveact
{
	int	key_code;
	int	triggers;
};

t_window	*new_window(const char *title, unsigned int width,
				unsigned int height);
void		add_destructor(t_window *window,
				void (*func)(void*), void *data);
void		*get_pixel_buffer(t_window *window);
void		delete_window(t_window *window);
void		start_main_loop(void (*func)(void *), void *data);

int			add_event(t_window *window, t_eveact activation,
				void (*func)(void *), void *data);

t_mouse_pos	get_mouse_pos(t_window *window);

char		*swin_strerr(void);

#endif // SIMPLEWINDOW_H