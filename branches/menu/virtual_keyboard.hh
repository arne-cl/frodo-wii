/*********************************************************************
 *
 * Copyright (C) 2009,  Simon Kagstrom
 *
 * Filename:      VirtualKeyboard.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   A virtual keyboard
 *
 * $Id$
 *
 ********************************************************************/
#ifndef __VIRTUAL_KEYBORD_HH__
#define __VIRTUAL_KEYBORD_HH__

#include <SDL.h>

#include "widget.hh"
#include "gui_view.hh"
#include "font.hh"

struct virtkey; 

class KeyListener
{
public:
	~KeyListener();

	/* Each key is a string */
	virtual void keyCallback(bool shift, const char *str) = 0;
};

class StringListener
{
public:
	~StringListener();

	virtual void stringCallback(const char *str) = 0;
};

class VirtualKeyboard : public GuiView
{
public:
	VirtualKeyboard(Font *font);

	void registerKeyListener(KeyListener *kl);
	void registerStringListener(StringListener *sl);
	void unregisterKeyListener(KeyListener *kl);
	void unregisterStringListener(StringListener *sl);

	/* Conversions */
	const char *keycodeToString(int kc);
	const char keycodeToChar(int kc);
	int charToKeycode(char c);
	int stringToKeycode(const char *str);

	void activate();

	void setFont(Font *font)
	{
		this->font = font;
	}

	void deactivate();

	bool isActive()
	{
		return this->is_active;
	}

	virtual void updateTheme();

	void draw(SDL_Surface *where);

	void runLogic();

	void draw(SDL_Surface *where, int x, int y, int w, int h);

	/* Singleton object */
	static VirtualKeyboard *kbd;
private:
	KeyListener *keyListeners[8];
	StringListener *stringListeners[8];

	void selectNext(int dx, int dy);
	void toggleShift();

	void flushKeyListeners();

	Font *font;
	int sel_x;
	int sel_y;
	bool shift_on;

	bool is_active;
	char buf[255];
	unsigned buf_head;
};

#endif /* __VIRTUAL_KEYBORD_HH__ */