/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Graphics.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchoquet <tchoquet@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 14:32:15 by tchoquet          #+#    #+#             */
/*   Updated: 2023/10/24 19:59:40 by tchoquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GRAPHICS_H
# define GRAPHICS_H

# define L_CLICK		1000
# define R_CLICK		2000
# define M_CLICK		3000

# define W_DOWN			4000
# define W_UP			5000

# define A_KEY			0
# define S_KEY			1
# define D_KEY			2
# define F_KEY			3
# define W_KEY			13
# define R_KEY			15

# define ONE_KEY		18
# define TWO_KEY		19
# define THREE_KEY		20
# define FOUR_KEY		21
# define FIVE_KEY		23

# define PLUS_KEY		24
# define NINUS_KEY		27

# define ENTER_KEY		36
# define SPACE_KEY		49
# define ESC_KEY		53

# define LEFT_KEY		123
# define RIGHT_KEY		124
# define UP_KEY			126
# define DOWN_KEY		125

# define CMD_L_KEY		259

# define ON_KEYDOWN		0b00000001
# define ON_KEYUP		0b00000010
# define ON_MOUSEMOVE	0b00010000
# define ON_EXPOSE		0b00100000
# define ON_DESTROY		0b01000000

# define RED	0x00FF0000
# define GREEN	0x0000FF00
# define BLUE	0x000000FF
# define BLACK	0x00000000
# define WHITE	0x00FFFFFF
# define TRANSP	0xFF000000

# ifndef UINT32_TYPE
#  define UINT32_TYPE

typedef unsigned int		t_uint32;

# endif // UINT32_TYPE

typedef struct s_vec2i
{
	int	x;
	int	y;

}	t_vec2i;

typedef struct s_vec2f
{
	float	x;
	float	y;

}	t_vec2f;

typedef struct s_stripe
{
	t_uint32	*px;
	int			size;

}	t_stripe;

typedef struct s_context	t_ctx;

int			create_window(char *title, int width, int height);
t_ctx		*back_ctx(void);
void		set_destructor(void (*func)(void *), void *data);
void		delete_window(void);

void		start_main_loop(void (*func)(void *), void *data);

t_vec2i		mouse_pos(void);
void		set_mouse_pos(t_vec2i pos);
void		hide_mouse(void);
void		show_mouse(void);

t_ctx		*new_context(t_vec2i size);
t_ctx		*ctx_from_img(char *path);
void		free_context(t_ctx *context);

t_vec2i		ctx_size(t_ctx *ctx);
t_uint32	ctx_px(t_ctx *context, t_vec2i pos);
t_stripe	ctx_vstripe(t_ctx *context, int x);

void		fill_ctx(t_ctx *ctx, t_uint32 color);
void		clear_ctx(t_ctx *context);
void		clear_pixels(t_ctx *context, t_uint32 px);

void		put_context(t_ctx *ctx, t_vec2i pos);
void		draw_pixel(t_ctx *ctx, t_vec2i pos, t_uint32 color);
void		draw_rect(t_ctx *ctx, t_vec2i pos, t_vec2i size, t_uint32 color);
void		draw_line(t_ctx *ctx, t_vec2i a, t_vec2i b, t_uint32 color);
void		draw_vstripe(t_ctx *ctx, t_stripe stripe,
				t_vec2i start, t_vec2i end);

int			add_event(int key, int trig, void (*func)(void *), void *data);
int			poll_key(int *key);
void		desac_poll_key(void);
void		react_poll_key(void);

t_vec2f		vi2tovf2(t_vec2i veci);
t_vec2i		add_vi2vi2(t_vec2i a, t_vec2i b);
t_vec2i		sub_vi2vi2(t_vec2i a, t_vec2i b);
t_vec2i		mul_vi2vi2(t_vec2i a, t_vec2i b);
t_vec2i		div_vi2vi2(t_vec2i a, t_vec2i b);

t_vec2i		vf2tovi2(t_vec2f vecf);
t_vec2f		add_vf2vi2(t_vec2f vecf, t_vec2i veci);
t_vec2f		mul_vf2vi2(t_vec2f vecf, t_vec2i veci);

t_vec2f		add_vf2vf2(t_vec2f a, t_vec2f b);
t_vec2f		sub_vf2vf2(t_vec2f a, t_vec2f b);
t_vec2f		mul_vf2vf2(t_vec2f a, t_vec2f b);
t_vec2f		div_vf2vf2(t_vec2f a, t_vec2f b);

t_vec2f		mul_vf2f(t_vec2f vec, float f);

t_vec2f		rot_vf2(t_vec2f vec, float rad);

#endif // GRAPHICS_H