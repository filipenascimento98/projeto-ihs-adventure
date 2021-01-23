#include <gb/gb.h>
#include <stdio.h>
#include "backgrounds.c"
#include "structs.c"
#include "sprites.c"
 
unsigned char windowMap[7] = 
{
    0x02,0x0F,0x13,0x13,0x00,0x20,0x1F//Frase inicial BOSS 10
};

UINT8 i;
UINT8 isInMainScreen = 1;
UINT8 isInFase1 = 1;
UINT8 qtdBalas = 5;
UINT8 dirXBoss = 1;
UINT8 dirYBoss = 2;

Cursor cursor;
GameCharacterNave nave;
GameCharacterBoss boss1;
GameBullet bulletNave[5];
GameBullet bulletBoss;
UBYTE spriteSize = 8;

void performantdelay(UINT8 numloops){
    UINT8 ii;
    for(ii = 0; ii < numloops; ii++){
        wait_vbl_done();
    }     
}

UBYTE checkcollisionsNave(GameCharacterNave* one, GameBullet* two){
    return (one->x >= two->x && one->x <= two->x + two->width) && (one->y >= two->y && one->y <= two->y + two->height) || (two->x >= one->x && two->x <= one->x + one->width) && (two->y >= one->y && two->y <= one->y + one->height) ||
            (one->x >= two->x && one->x <= two->x + two->width) && (two->y >= one->y && two->y <= one->y + one->height) || (two->x >= one->x && two->x <= one->x + one->width) && (one->y >= two->y && one->y <= two->y + two->height);
}

UBYTE checkcollisionsBoss(GameCharacterBoss* one, GameBullet* two){
    return (one->x >= two->x && one->x <= two->x + two->width) && (one->y >= two->y && one->y <= two->y + two->height) || (two->x >= one->x && two->x <= one->x + one->width) && (two->y >= one->y && two->y <= one->y + one->height);
}

void resetWindowMap(){
    windowMap[0] = 0x02;
    windowMap[1] = 0x0F;
    windowMap[2] = 0x13;
    windowMap[3] = 0x13;
    windowMap[4] = 0x00;
    windowMap[5] = 0x20;
    windowMap[6] = 0x1F;
}
void setupHUD(UINT8 life){
    for(i = 0; i < life; i++){
        set_sprite_tile(26 + i, 22);
        move_sprite(26 + i, 20 + (8*i), 20);
    }
}

void moveGameBoss(GameCharacterBoss* character, UINT8 x, UINT8 y){
    move_sprite(character->spritids[0], x, y);
    move_sprite(character->spritids[1], x + spriteSize, y);
    move_sprite(character->spritids[2], x + (spriteSize * 2), y);
    move_sprite(character->spritids[3], x + (spriteSize * 3), y);
    move_sprite(character->spritids[4], x, y + spriteSize);
    move_sprite(character->spritids[5], x + spriteSize, y + spriteSize);
    move_sprite(character->spritids[6], x + (spriteSize * 2), y + spriteSize);
    move_sprite(character->spritids[7], x + (spriteSize * 3), y + spriteSize);
    move_sprite(character->spritids[8], x, y + (spriteSize * 2));
    move_sprite(character->spritids[9], x + spriteSize, y + (spriteSize * 2));
    move_sprite(character->spritids[10], x + (spriteSize * 2), y + (spriteSize * 2));
    move_sprite(character->spritids[11], x + (spriteSize * 3), y + (spriteSize * 2));
    move_sprite(character->spritids[12], x, y + (spriteSize * 3));
    move_sprite(character->spritids[13], x + spriteSize, y + (spriteSize * 3));
    move_sprite(character->spritids[14], x + (spriteSize * 2), y + (spriteSize * 3));
    move_sprite(character->spritids[15], x + (spriteSize * 3), y + (spriteSize * 3));
}

void moveGameNave(GameCharacterNave* character, UINT8 x, UINT8 y){
    move_sprite(character->spritids[0], x, y);
    move_sprite(character->spritids[1], x + spriteSize, y);
    move_sprite(character->spritids[2], x, y + spriteSize);
    move_sprite(character->spritids[3], x + spriteSize, y + spriteSize);
}

void moveGameBullet(GameBullet* bullet, UINT8 x, UINT8 y){
    move_sprite(bullet->spriteId, x, y);
}

void bossMoveFase1(){
    if(!dirXBoss){
            if(boss1.x - 4 <= 13){
                boss1.x = 13;
                dirXBoss = 1;
            }else{
                boss1.x -= 4;
                boss1.y += dirYBoss;
                dirYBoss *= -1;
            }
            moveGameBoss(&boss1, boss1.x, boss1.y);
       }
       if(dirXBoss){
            if(boss1.x + 36 >= 160){
                dirXBoss = 128;
                dirXBoss = 0;
            }else{
                boss1.x += 4;
                boss1.y += dirYBoss;
                dirYBoss *= -1;
            }
            moveGameBoss(&boss1, boss1.x, boss1.y);
       }
}

void setupBulletNave(){
    for(i = 0; i < qtdBalas; i++){
        bulletNave[i].x = 180;
        bulletNave[i].y = 150;
        bulletNave[i].width = 2;
        bulletNave[i].height = 8;
        bulletNave[i].disponivel = 1;
        bulletNave[i].spriteId = 4 + i;
        set_sprite_tile(bulletNave[i].spriteId, 4);
        moveGameBullet(&bulletNave[i], 180, 150);
    }
}

void setupBulletBoss(){
    bulletBoss.x = 180;
    bulletBoss.y = 150;
    bulletBoss.width = 8;
    bulletBoss.height = 8;

    set_sprite_tile(25, 21);
    bulletBoss.spriteId = 25;
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

    moveGameNave(&nave, nave.x, nave.y);
}

void setupBoss1(){
    setupBulletBoss();
    boss1.bullet = &bulletNave;
    boss1.x = 70;
    boss1.y = 50;
    boss1.width = 32;
    boss1.height = 32;
    boss1.life = 10;

    for(i = 0; i < 16; i++){
        set_sprite_tile(9 + i, 5 + i);
        boss1.spritids[i] = 9 + i;
    }

    moveGameBoss(&boss1, boss1.x, boss1.y);
}

void playShotSoundNave(){
    NR10_REG = 0x1E;
    NR11_REG = 0x10;
    NR12_REG = 0xF3;
    NR13_REG = 0x00;
    NR14_REG = 0x87;
}

void playLobbySound(){
    NR10_REG = 0x00;
    NR11_REG = 0x81;
    NR12_REG = 0x43;
    NR13_REG = 0x73;
    NR14_REG = 0x86;
}

void naveShotFire(GameBullet* bullet){
    if(!bullet->disponivel){
           if(bullet->y <= 5){
                bullet->disponivel = 1;
                moveGameBullet(bullet, 180, 150);
           }else{
                bullet->y += -5;
                moveGameBullet(bullet, bullet->x, bullet->y);
                if(checkcollisionsBoss(&boss1, bullet)){
                    boss1.life -= 1;
                    windowMap[5] = 0x1F;
                    windowMap[6] = 31 + boss1.life;
                    //updateHUD();
                    bullet->disponivel = 1;
                    moveGameBullet(bullet, 180, 180);
                    if(boss1.life == 0){
                        performantdelay(5);
                        isInFase1 = 0;
                        moveGameBoss(&boss1, 180, 180);
                        for(i = 0; i < qtdBalas; i++){
                            moveGameBullet(&bulletNave[i], 180, 180);
                        }
                        moveGameBullet(&bulletBoss, 180, 180);
                        moveGameNave(&nave, 180, 180);
                        move_win(7, 180);
                        resetWindowMap();
                    }
                }  
           }    
    }
}

void bossShotFire(GameBullet* bullet){
    if(!bullet->disponivel){
           if(bullet->y >= 155){
                bullet->disponivel = 1;
                moveGameBullet(bullet, 180, 150);
           }else{
                bullet->y += 5;
                moveGameBullet(bullet, bullet->x, bullet->y);
                if(checkcollisionsNave(&nave, bullet)){
                    performantdelay(5);
                    isInFase1 = 0;
                    moveGameBoss(&boss1, 180, 180);
                    for(i = 0; i < qtdBalas; i++){
                        moveGameBullet(&bulletNave[i], 180, 180);
                    }
                    moveGameBullet(&bulletBoss, 180, 180);
                    moveGameNave(&nave, 180, 180);
                    move_win(7, 180);
                    resetWindowMap();
                }  
           }    
    }
}

void inGaming(){

    while(isInFase1){
        scroll_bkg(1,1);
       if(joypad() & J_LEFT){
            if(nave.x - 4 <= 13){
                nave.x = 13;
            }else{
                nave.x -= 4;
            }
            moveGameNave(&nave, nave.x, nave.y);
       }
       if(joypad() & J_RIGHT){
            if(nave.x + 4 >= 147){
                nave.x = 147;
            }else{
                nave.x += 4;
            }
            moveGameNave(&nave, nave.x, nave.y);
       }
       if(joypad() & J_UP){
           for(i = 0; i < qtdBalas; i++){
               if(bulletNave[i].disponivel){
                    playShotSoundNave();
                    bulletNave[i].disponivel = 0;
                    bulletNave[i].x = nave.x + 7;
                    bulletNave[i].y = nave.y - 3;
                    break;
               }
           }           
       }
       bossMoveFase1();
       if(isInFase1){
            for(i = 0; i < qtdBalas; i++){
                naveShotFire(&bulletNave[i]);
            }
       }
       if(isInFase1){
            if(bulletBoss.disponivel == 1){
                bulletBoss.disponivel = 0;
                bulletBoss.x = boss1.x + 12;
                bulletBoss.y = boss1.y + 26;
            }
            bossShotFire(&bulletBoss);
       }

       performantdelay(5);      
    }
    
}

void startFase1(){
    UINT8 j = 0;
    set_sprite_data(0, 5, naveSprite);//Carregando sprites da nave
    set_sprite_data(5, 17, boss1Sprite);//Carregando sprites do boss
    set_sprite_data(22, 1, heartSprite);
    //set_bkg_data(45, 3, backgroundFase1Sprite);//Carregando background da fase 1
    set_bkg_data(45, 0x2D, bkg_data);
    
    for(i = 0; i < 32; i+=8)
        for(j = 0; j < 32; j+=8)
            set_bkg_tiles(i, j, 8, 8, bkg_tiles);
    //set_bkg_tiles(0, 0, 20, 18, fase1Bkg);

    //setupHUD(10);//Coloca na tela 10 corações

    setupNave();//Carrega as características da nave
    setupBoss1();//Carrega as características do boss1

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    inGaming();
    move_bkg(0, 0);//Como o background estará se movendo, ele retorna a posição original
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


void menu(){
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

    isInMainScreen = 1;

    while(isInMainScreen){
        switch (joypad())
        {
        case J_UP:
            if(cursor.y - 16 < 96){
                cursor.y = 96;
            }else{
                playLobbySound();
                cursor.y -= 16;
            }
            move_sprite(0, cursor.x, cursor.y);
            break;
        case J_DOWN:
            if(cursor.y + 16 > 112){
                cursor.y = 112;
            }else{
                playLobbySound();
                cursor.y += 16;
            }
            move_sprite(0, cursor.x, cursor.y);
            break;
        case J_START:
            if(cursor.y == 96){//Inicia o jogo
                isInMainScreen = 0;
                isInFase1 = 1;
                startFase1();
            }else if(cursor.y == 112){//Vai para a tela de Highscore
                
            }
        }
        performantdelay(2);
    }
}

void main(){
    NR52_REG = 0x80;
    NR51_REG = 0x11;
    NR50_REG = 0x77;

    while(1){
        menu();
    }
   
}