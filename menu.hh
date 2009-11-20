/*********************************************************************
 *
 * Copyright (C) 2004, 2008,  Simon Kagstrom
 *
 * Filename:      menu.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:
 *
 * $Id$
 *
 ********************************************************************/
#ifndef __MENU_H__
#define __MENU_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdint.h>

enum {
	EVENT_NONE   = 0,
	KEY_UP       = 1,
	KEY_DOWN     = 2,
	KEY_LEFT     = 4,
	KEY_RIGHT    = 8,
	KEY_SELECT   = 16,
	KEY_ESCAPE   = 32,
	KEY_PAGEDOWN = 64,
	KEY_PAGEUP   = 128,
	KEY_HELP     = 256,
};


typedef struct
{
	int n_entries;
	int index;
	int sel;
} submenu_t;

typedef int event_t;

class Menu
{
public:
	Menu(TTF_Font *font, SDL_Color clr,	int w, int h);

	void setText(const char **messages);

	void pushEvent(SDL_Event *ev);

	void runLogic();

	void draw(SDL_Surface *where,
			int x, int y);

	~Menu();

protected:
	virtual void selectCallback(int which);

	virtual void escapeCallback(int which);

	submenu_t *findSubmenu(int index);

	int getNextEntry(int dy);

	void selectOne(int which);

	void selectNext(int dx, int dy);

	void selectNext(event_t ev);

	void pushEvent(event_t ev);

	event_t popEvent();

	const char *title;
	const char **pp_msgs;
	TTF_Font *font;
	SDL_Color text_color;

	/* Width and height */
	int w, h;

	/* Relative to this menu */
	int		   mouse_x, mouse_y;

	int        n_submenus;
	submenu_t *p_submenus;

	int        cur_sel; /* Main selection */
	int        start_entry_visible;
	int        n_entries;

	int 	   ev_head, ev_tail;
	event_t	   event_stack[8];
};

#endif /* !__MENU_H__ */