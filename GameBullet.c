#include <gb/gb.h>

//generical character structure: id, position, graphics
typedef struct{
	UBYTE spriteId; // all characters use 1 sprites
	UINT8 x;
	UINT8 y;
	UINT8 width;
	UINT8 height;
	UINT8 disponivel;
} GameBullet;