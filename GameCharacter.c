#include <gb/gb.h>
#include "GameBullet.c"

//generical character structure: id, position, graphics
typedef struct{
	UBYTE spritids[4]; // all characters use 4 sprites
	GameBullet* bullet;
	UINT8 x;
	UINT8 y;
	UINT8 width;
	UINT8 height;
} GameCharacter;