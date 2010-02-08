#ifndef __GAME_INFO_HH__
#define __GAME_INFO_HH__

#include <SDL.h>

/* This is just a link to some other file (filename is the link) */
#define F_IS_LINK (1 << 0)

struct game_info
{
	/* These two MUST stay the same */
	uint32_t sz;
	uint16_t version_magic;
	uint16_t flags;

	uint16_t author_off;
	uint16_t name_off;
	uint16_t screenshot_off; /* In PNG format */
	uint16_t filename_off;
	uint16_t score;
	uint16_t year;
	uint8_t data[]; /* 4-byte aligned */
};

class GameInfo
{
public:
	GameInfo(const char *filename = "unknown", const char *name = " ",
			const char *author = " ",
			SDL_Surface *image = NULL);

	GameInfo(GameInfo *gi);

	~GameInfo();

	void setAuthor(const char *author);

	void setName(const char *name);

	void setYear(uint16_t year);

	void setScreenshot(SDL_Surface *scr);

	void resetDefaults();

	/** Returns an allocated dump structure */
	struct game_info *dump();

	/** Fill in this game info object from a structure */
	bool fromDump(struct game_info *data);

	static GameInfo *loadFromFile(const char *fileName);

	/* Should perhaps be protected but I trust you - just be careful! */
	const char *name;
	const char *author;
	const char *filename;
	SDL_Surface *screenshot;

	uint16_t year;
	uint16_t score;
};

#endif /*__GAME_INFO_HH__ */