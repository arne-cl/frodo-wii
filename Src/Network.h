#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <SDL.h>

#define NETWORK_UPDATE_SIZE  (256 * 1024)
enum
{
	HEADER             = 0,
	DISPLAY_UPDATE_RAW = 1,
	DISPLAY_UPDATE_RLE = 2,
	SOUND_UPDATE_RAW   = 3,
	SOUND_UPDATE_RLE   = 4,
	KEYBOARD_UPDATE    = 5,
	JOYSTICK_UPDATE    = 6,
};

struct NetworkDisplayUpdate
{
	Uint8 type;    /* raw data or RLE */
	Uint8 square;
	Uint16 size;
	Uint8 data[];
};

struct NetworkSoundUpdate
{
	Uint8 type;    /* raw data or RLE */
	Uint8 dummy;
	Uint16 size;
	Uint8 data[];
};

struct NetworkJoystickUpdate
{
	Uint8 type;
	Uint8 which;
	Uint16 size;
	Uint8 data;   /* New value */
};

struct NetworkUpdate 
{
	Uint8 type;
	Uint8 dummy;
	Uint16 size;

	/* The rest is just data of some type */
	Uint8 data[];
};

class Network
{
public:
	Network();

	~Network();

	/** Encode part of a screen into @a dst
	 * 
	 * @param dst the destination update structure
	 * @param screen the screen to encode
	 * @param square the square index of the screen to encode
	 *
	 * @return the size of the encoded message
	 */
	size_t EncodeDisplaySquare(struct NetworkDisplayUpdate *dst,
			Uint8 *screen, int square);

	size_t EncodeDisplay(Uint8 *master, Uint8 *remote);

	/**
	 * Encode the @a buf sound buffer into @a dst
	 *
	 * @param dst the destination update structure
	 * @param buf the buffer to encode
	 * @param len the length of the in-buffer
	 *
	 * @return the size of the encoded message
	 */
	size_t EncodeSoundBuffer(struct NetworkSoundUpdate *dst,
			Uint8 *buf, size_t len);

	void EncodeJoystickUpdate(struct NetworkJoystickUpdate *dst,
			Uint8 which, Uint8 v);

	/**
	 * Decode a display update message onto @a screen
	 *
	 * @param screen the screen to draw to
	 * @param src the message to decode
	 */
	bool DecodeDisplayUpdate(Uint8 *screen, struct NetworkDisplayUpdate *src);
	
	size_t DecodeSoundUpdate(struct NetworkSoundUpdate *src, char *buf);

	void ResetNetworkUpdate(void);

	bool SendUpdate(int sock);

	bool ReceiveUpdate(int sock);

	bool ReceiveUpdateBlock(int sock);

private:
	size_t EncodeDisplayRLE(struct NetworkDisplayUpdate *dst, Uint8 *screen,
			int x, int y);
	size_t EncodeDisplayRaw(struct NetworkDisplayUpdate *dst, Uint8 *screen,
			int x, int y);
	size_t EncodeSoundRLE(struct NetworkSoundUpdate *dst,
			Uint8 *buffer, size_t len);
	size_t EncodeSoundRaw(struct NetworkSoundUpdate *dst,
			Uint8 *buffer, size_t len);

	NetworkUpdate *IterateFirst(NetworkUpdate *p, unsigned int *cookie);

	NetworkUpdate *IterateNext(NetworkUpdate *p, unsigned int *cookie);

	void AddNetworkUpdate(struct NetworkUpdate *update)
	{
		this->cur_ud += update->size;
		this->ud->size += update->size;
	}
	/**
	 * Compare two display squares.
	 *
	 * @param a the first square (first byte)
	 * @param b the second square (first byte)
	 *
	 * @return true if they are equal
	 */
	bool CompareSquare(Uint8 *a, Uint8 *b);

	bool DecodeDisplayRLE(Uint8 *screen, struct NetworkDisplayUpdate *src,
			int x, int y);
	bool DecodeDisplayRaw(Uint8 *screen, struct NetworkDisplayUpdate *src,
			int x, int y);

	bool ReceiveUpdate(NetworkUpdate *dst, int sock, struct timeval *tv);

	void MarshalData(NetworkUpdate *ud);

	void DeMarshalData(NetworkUpdate *ud);

	NetworkUpdate *ud;
	Uint8 *cur_ud;
};

class NetworkClient : public Network
{
public:
	NetworkClient(int sock);

	~NetworkClient();

	NetworkClient(const char *hostname, int port);

	bool isConnected()
	{
		return this->sock >= 0;
	}
	
	bool SendUpdate()
	{
		return ((Network*)this)->SendUpdate(this->sock);
	}

	bool ReceiveUpdate()
	{
		return ((Network*)this)->ReceiveUpdate(this->sock);
	}


	bool ReceiveUpdateBlock()
	{
		return ((Network*)this)->ReceiveUpdateBlock(this->sock);
	}

	Uint8 *screen;
	int joystick_port;
private:
	int sock;
};

#define MAX_NETWORK_CLIENTS 8

class NetworkServer : public Network
{
public:
	NetworkServer(int port);

	bool CheckNewConnection();

	NetworkClient *clients[MAX_NETWORK_CLIENTS];
	int n_clients;

private:
	void AddClient(int sock);

	int listen_sock;
};

#endif /* NETWORK_H */
