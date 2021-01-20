#include <gb/gb.h>

typedef struct Cursor{
    UINT8 x;
    UINT8 y;
    UINT8 col;
    UINT8 row;
} Cursor;

//generical character structure: id, position, graphics
typedef struct{
	UBYTE spriteId; // all characters use 1 sprites
	UINT8 x;
	UINT8 y;
	UINT8 width;
	UINT8 height;
	UINT8 disponivel;
} GameBullet;

//generical character structure: id, position, graphics
typedef struct{
	UBYTE spritids[4]; // all characters use 4 sprites
	GameBullet* bullet;
	UINT8 x;
	UINT8 y;
	UINT8 width;
	UINT8 height;
} GameCharacterNave;

typedef struct{
	UBYTE spritids[16]; // all characters use 4 sprites
	GameBullet* bullet;
	UINT8 x;
	UINT8 y;
	UINT8 width;
	UINT8 height;
	UINT8 life;
} GameCharacterBoss;