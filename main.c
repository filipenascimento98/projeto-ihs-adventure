#include <gb/gb.h>
#include <stdio.h>
#include "keyboarddata.c"
#include "mainScreen.c"
#include "sprites.c"
#include "cursor.c"
#include "fontMainScreen.c"
#include "GameCharacter.c"
#include "GameSprites.c"
#include "GameBullet.c"
#include "backgroundFase1.c"
#include "backgroundFase1Sprites.c"

UINT8 i;
UINT8 isInMainScreen = 1;
UINT8 qtdBalas = 5;

Cursor cursor;
GameCharacter nave;
GameCharacter boss1;
GameBullet bulletNave[5];
GameBullet bulletBoss;
UBYTE spriteSize = 8;

void performantdelay(UINT8 numloops){
    UINT8 ii;
    for(ii = 0; ii < numloops; ii++){
        wait_vbl_done();
    }     
}

UBYTE checkcollisions(GameCharacter* one, GameBullet* two){
    return (one->x >= two->x && one->x <= two->x + two->width) && (one->y >= two->y && one->y <= two->y + two->height) || (two->x >= one->x && two->x <= one->x + one->width) && (two->y >= one->y && two->y <= one->y + one->height);
}

void moveGameCharacter(GameCharacter* character, UINT8 x, UINT8 y){
    move_sprite(character->spritids[0], x, y);
    move_sprite(character->spritids[1], x + spriteSize, y);
    move_sprite(character->spritids[2], x, y + spriteSize);
    move_sprite(character->spritids[3], x + spriteSize, y + spriteSize);
}

void moveGameBullet(GameBullet* bullet, UINT8 x, UINT8 y){
    move_sprite(bullet->spriteId, x, y);
}

void setupBulletNave(){
    for(i = 0; i < qtdBalas; i++){
        bulletNave[i].x = 180;
        bulletNave[i].y = 150;
        bulletNave[i].width = 8;
        bulletNave[i].height = 8;
        bulletNave[i].disponivel = 1;
        bulletNave[i].spriteId = 8 + i;
        set_sprite_tile(bulletNave[i].spriteId, 8);
        moveGameBullet(&bulletNave[i], 180, 150);
    }
}

void setupBulletBoss(){
    bulletBoss.x = 0;
    bulletBoss.y = 0;
    bulletBoss.width = 8;
    bulletBoss.height = 8;

    set_sprite_tile(13, 9);
    bulletBoss.spriteId = 9;
}

void setupNave(){
    setupBulletNave();
    nave.bullet = &bulletNave;
    nave.x = 80;
    nave.y = 144;
    nave.width = 16;
    nave.height = 16;

    // load sprites for nave
    set_sprite_tile(0, 0);
    nave.spritids[0] = 0;
    set_sprite_tile(1, 1);
    nave.spritids[1] = 1;
    set_sprite_tile(2, 2);
    nave.spritids[2] = 2;
    set_sprite_tile(3, 3);
    nave.spritids[3] = 3;

    moveGameCharacter(&nave, nave.x, nave.y);
}

void setupBoss1(){
    setupBulletBoss();
    boss1.bullet = &bulletNave;
    boss1.x = 30;
    boss1.y = 16;
    boss1.width = 16;
    boss1.height = 16;

    set_sprite_tile(4, 4);
    boss1.spritids[0] = 4;
    set_sprite_tile(5, 5);
    boss1.spritids[1] = 5;
    set_sprite_tile(6, 6);
    boss1.spritids[2] = 6;
    set_sprite_tile(7, 7);
    boss1.spritids[3] = 7;

    moveGameCharacter(&boss1, boss1.x, boss1.y);
}

void naveShotFire(GameBullet* bullet){
    if(!bullet->disponivel){
           if(bullet->y <= 5){
                bullet->disponivel = 1;
                moveGameBullet(bullet, 180, 150);
           }else{
                bullet->y += -5;
                moveGameBullet(bullet, bullet->x, bullet->y);
           }    
    }
}

void inGaming(){

    while(1){
       if(joypad() & J_LEFT){
           nave.x -= 2;
           moveGameCharacter(&nave, nave.x, nave.y);
       }
       if(joypad() & J_RIGHT){
           nave.x += 2;
           moveGameCharacter(&nave, nave.x, nave.y);
       }
       if(joypad() & J_UP){
           for(i = 0; i < qtdBalas; i++){
               if(bulletNave[i].disponivel){
                    bulletNave[i].disponivel = 0;
                    bulletNave[i].x = nave.x + 3;
                    bulletNave[i].y = nave.y - 3;
                    break;
               }
           }           
       }

        
       for(i = 0; i < qtdBalas; i++){
           naveShotFire(&bulletNave[i]);
       }

       boss1.y += 5;
       
       if(boss1.y >= 144){
           boss1.y=0;
           boss1.x = nave.x;
       }

       moveGameCharacter(&boss1, boss1.x, boss1.y);

       performantdelay(5);      
    }
}

void startFase1(){
    set_sprite_data(0, 10, GameSprites);
    set_bkg_data(45, 3, backgroundFase1Sprit);
    set_bkg_tiles(0, 0, 20, 18, backgroundFase1);

    setupNave();
    setupBoss1();

    inGaming();

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

void screenHighscore(){

}

void fadeout(){
	for(i=0;i<4;i++){
		switch(i){
			case 0:
				BGP_REG = 0xE4;
				break;
			case 1:
				BGP_REG = 0xF9;
				break;
			case 2:
				BGP_REG = 0xFE;
				break;
			case 3:
				BGP_REG = 0xFF;	
				break;						
		}
		performantdelay(10);
	}
}

void fadein(){
	for(i=0;i<3;i++){
		switch(i){
			case 0:
				BGP_REG = 0xFE;
				break;
			case 1:
				BGP_REG = 0xF9;
				break;
			case 2:
				BGP_REG = 0xE4;
				break;					
		}
		performantdelay(10);
	}
}

void main(){
    //set sprite do cursor
    set_sprite_data(0, 1, sprites);
    set_sprite_tile(0, 0);

    cursor.x = 32;
    cursor.y = 96;
    cursor.col = 0;
    cursor.row = 0;
    move_sprite(0, cursor.x, cursor.y);

    set_bkg_data(0, 45, fontMainScreen);
    set_bkg_tiles(0, 0, 20, 18, bgmainscreen);

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    while(isInMainScreen){
        switch (joypad())
        {
        case J_UP:
            if(cursor.y - 16 < 96){
                cursor.y = 96;
            }else{
                cursor.y -= 16;
            }
            move_sprite(0, cursor.x, cursor.y);
            break;
        case J_DOWN:
            if(cursor.y + 16 > 112){
                cursor.y = 112;
            }else{
                cursor.y += 16;
            }
            move_sprite(0, cursor.x, cursor.y);
            break;
        case J_START:
            if(cursor.y == 96){//Inicia o jogo
                isInMainScreen = 0;
                startFase1();
            }else if(cursor.y == 112){//Vai para a tela de Highscore
                
            }
        }
        performantdelay(2);
    }
}