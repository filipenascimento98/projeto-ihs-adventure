#include <gb/gb.h>
#include <stdio.h>
#include "backgrounds.c"
#include "structs.c"
#include "sprites.c"

void menu();
void startFase1();
void startFase2();
void startFase3();
void nextFase();
 
extern UINT8 playerName[15];
extern UINT8 faseName[3];
UINT8 i;
UINT8 j;
UINT8 isInMainScreen = 1;
UINT8 isInsHighscoreScreen = 0;
UINT8 currentFase = 1;//Começa na fase 1 e muda conforme passa de fase
UINT8 currentBoss = 0;//Armazena a informação de qual Boss está em tela atualmente
//UINT8 isInFase1 = 1;
UINT8 isInGame = 0;
UINT8 qtdBalas = 5;
UINT8 dirXBoss = 1;
UINT8 dirYBoss = 2;
UBYTE keyDown = 0;
UINT8 nameCharacterIndex;
UBYTE playerHasName = 0;
UBYTE spriteSize = 8;
const UINT8 mincursorx = 8;
const UINT8 mincursory = 80;
const UINT8 maxcursorx = 156;
const UINT8 maxcursory = 128;

Cursor cursor;
Cursor cursorKeyboard;

GameCharacterNave nave;
GameCharacterBoss boss[3];

GameBullet bulletNave[5];
GameBullet bulletBoss;

GameLifeBoss lifeBoss;

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

void drawPlayerName(){
    set_bkg_tiles(1, 4, 15, 1, playerName);
}

void drawPlayerNameInHighScore(){
    set_bkg_tiles(1, 4, 15, 1, playerName);
    set_bkg_tiles(16, 4, 3, 1, faseName);
}

void addToPlayerName(Cursor* cursor){
    UINT8 characterIndex = cursor->row * 10 + cursor->col + 1; // Indexa as sprites do keyboarBkg dando um shift pra esquerda, essa indexação serve para identificar qual letra foi selecionada

    if(nameCharacterIndex == 15) return; // Tamanho máximo do nome

    playerName[nameCharacterIndex] = characterIndex;
    nameCharacterIndex++;
}

void removeFromPlayerName(){
    if(nameCharacterIndex > 0){
        nameCharacterIndex--;
        playerName[nameCharacterIndex] = 0;
    }
}

UBYTE isWithinKeyboard(UINT8 x, UINT8 y){
    if(x == 136 && y == 144 || x == 152 && y == 144){
        return 1;
    }
    return x >= mincursorx && x <= maxcursorx && y >= mincursory && y <= maxcursory;
}

void updatePlayerName(Cursor* cursor){
    // Checa se o cursor está na opção de delete
    if(cursor->col==8 && cursor->row == 4){
        // delete
        removeFromPlayerName();
        drawPlayerName();
    }
    else if (cursor->col==9 && cursor->row == 4){
        // player finished
        playerHasName = 1;
    }
    else{
        addToPlayerName(cursor);
        drawPlayerName();
    }
}

void resetCharacterName(){
    for(i = 0; i != 15; i++){
        playerName[i] = 0x00;
    }
    playerHasName = 0;
}

void saveGame(){
    //Só entra no if se durante o jogo não houve nenhum save e se no arquivo não tem nenhum save salvo
    if(!playerHasName && playerName[0] > 0x28){
        resetCharacterName();
        set_sprite_data(0, 1, cursorKeyboardSprite);//Armazena o cursor do teclado
        set_sprite_tile(0, 0);

        cursorKeyboard.x = 8;
        cursorKeyboard.y = 80;
        cursorKeyboard.col = 0;
        cursorKeyboard.row = 0;
        move_sprite(0, cursorKeyboard.x, cursorKeyboard.y);

        set_bkg_tiles(0, 0, 20, 18, keyboardBkg);//Armazena o background de teclado
        move_bkg(0, 0);
        //scroll_bkg(-4, 0);

        SHOW_BKG;
        SHOW_SPRITES;
        DISPLAY_ON;

        while(!playerHasName){
        if(keyDown){
            waitpadup();
            keyDown = 0;
        }

        switch (joypad())
        {
            case J_UP:
                if(isWithinKeyboard(cursorKeyboard.x, cursorKeyboard.y - 16)){
                    cursorKeyboard.y -= 16;
                    cursor.row--;
                    keyDown = 1;
                    scroll_sprite(0, 0, -16);
                }  
                break;
            case J_DOWN:
                if(isWithinKeyboard(cursorKeyboard.x, cursorKeyboard.y + 16)){
                    cursorKeyboard.y += 16;
                    cursor.row++;
                    keyDown = 1;
                    scroll_sprite(0, 0, 16);
                }
                break;
            case J_RIGHT:
                if(isWithinKeyboard(cursorKeyboard.x + 16, cursorKeyboard.y)){
                    cursorKeyboard.x += 16;
                    cursor.col++;
                    keyDown = 1;
                    scroll_sprite(0, 16, 0);
                }
                break;    
            case J_LEFT:
                if(isWithinKeyboard(cursorKeyboard.x - 16, cursorKeyboard.y)){
                cursorKeyboard.x -= 16;
                    cursor.col--;
                    keyDown = 1;
                    scroll_sprite(0, -16, 0); 
                }
                break;
            case J_A:
                updatePlayerName(&cursor);
                keyDown = 1;
                break;    
            }

            performantdelay(2);
        }
    }

    
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
            if(boss[currentBoss].x - 4 <= 13){
                boss[currentBoss].x = 13;
                dirXBoss = 1;
            }else{
                boss[currentBoss].x -= 4;
                boss[currentBoss].y += dirYBoss;
                dirYBoss *= -1;
            }
            moveGameBoss(&boss[currentBoss], boss[currentBoss].x, boss[currentBoss].y);
       }
       if(dirXBoss){
            if(boss[currentBoss].x + 36 >= 160){
                dirXBoss = 128;
                dirXBoss = 0;
            }else{
                boss[currentBoss].x += 4;
                boss[currentBoss].y += dirYBoss;
                dirYBoss *= -1;
            }
            moveGameBoss(&boss[currentBoss], boss[currentBoss].x, boss[currentBoss].y);
       }
}


void setupLifeBoss(){
    switch (currentBoss)
    {
        case 0:
            
            lifeBoss.x = 20;
            lifeBoss.y = 150;
            lifeBoss.life = 10 ;
            lifeBoss.tipoBoss = currentBoss;
            set_sprite_tile(28 , 24);
            lifeBoss.spritids[0] = 28;
            for(i = 0; i < 8; i++){
                set_sprite_tile(29 + i, 25);
                lifeBoss.spritids[i+1] = 29 + i;
            }
            set_sprite_tile(37, 26);
            lifeBoss.spritids[9] = 37;
            
            for(i = 0; i < 10; i++){
                move_sprite(lifeBoss.spritids[i], lifeBoss.x + (spriteSize * i), lifeBoss.y);
            }
            
            break;
        case 1:
           lifeBoss.x = 20;
            lifeBoss.y = 20;
            lifeBoss.life = 10 ;
            lifeBoss.tipoBoss = currentBoss;
            set_sprite_tile(28 , 24);
            lifeBoss.spritids[0] = 28;
            for(i = 0; i < 8; i++){
                set_sprite_tile(29 + i, 25);
                lifeBoss.spritids[i+1] = 29 + i;
            }
            set_sprite_tile(37, 26);
            lifeBoss.spritids[9] = 37;
            
            for(i = 0; i < 10; i++){
                move_sprite(lifeBoss.spritids[i], lifeBoss.x + (spriteSize * i), lifeBoss.y);
            }
            break;
        case 2:
            lifeBoss.x = 20;
            lifeBoss.y = 20;
            lifeBoss.life = 10 ;
            lifeBoss.tipoBoss = currentBoss;
            set_sprite_tile(28 , 24);
            lifeBoss.spritids[0] = 28;
            for(i = 0; i < 8; i++){
                set_sprite_tile(29 + i, 25);
                lifeBoss.spritids[i+1] = 29 + i;
            }
            set_sprite_tile(37, 26);
            lifeBoss.spritids[9] = 37;
            
            for(i = 0; i < 10; i++){
                move_sprite(lifeBoss.spritids[i], lifeBoss.x + (spriteSize * i), lifeBoss.y);
            }
            break;
        default:
            break;
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
    boss[0].bullet = &bulletNave;
    boss[0].x = 70;
    boss[0].y = 50;
    boss[0].width = 32;
    boss[0].height = 32;
    boss[0].life = 10;

    for(i = 0; i < 16; i++){
        set_sprite_tile(9 + i, 5 + i);
        boss[0].spritids[i] = 9 + i;
    }

    moveGameBoss(&boss[0], boss[0].x, boss[0].y);
}

void setupBoss2(){
    setupBulletBoss();
    boss[1].bullet = &bulletNave;
    boss[1].x = 70;
    boss[1].y = 50;
    boss[1].width = 32;
    boss[1].height = 32;
    boss[1].life = 10;

    for(i = 0; i < 16; i++){
        set_sprite_tile(9 + i, 5 + i);
        boss[1].spritids[i] = 9 + i;
    }

    moveGameBoss(&boss[1], boss[1].x, boss[1].y);
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
                if(checkcollisionsBoss(&boss[currentBoss], bullet)){
                    boss[currentBoss].life -= 1;
                    //updateHUD();
                    bullet->disponivel = 1;
                    moveGameBullet(bullet, 180, 180);
                    if(boss[currentBoss].life == 0){
                        performantdelay(5);
                        //isInFase1 = 0;
                        moveGameBoss(&boss[currentBoss], 180, 180);
                        for(i = 0; i < qtdBalas; i++){
                            moveGameBullet(&bulletNave[i], 180, 180);
                        }
                        moveGameBullet(&bulletBoss, 180, 180);
                        moveGameNave(&nave, 180, 180);
                        //saveGame();
                        nextFase();
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
                    isInGame = 0;
                    moveGameBoss(&boss[currentBoss], 180, 180);
                    for(i = 0; i < qtdBalas; i++){
                        moveGameBullet(&bulletNave[i], 180, 180);
                    }
                    moveGameBullet(&bulletBoss, 180, 180);
                    moveGameNave(&nave, 180, 180);
                    move_win(7, 180);
                    saveGame();
                }  
           }    
    }
}

void inGaming(){

    //while(isInFase1) Tava assim antes
    while(isInGame){  
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
       //if(isInFase1) tava assim antes
       if(isInGame){
            for(i = 0; i < qtdBalas; i++){
                naveShotFire(&bulletNave[i]);
            }
       }
       //if(isInFase1) tava assim antes
       if(isInGame){
            if(bulletBoss.disponivel == 1){
                bulletBoss.disponivel = 0;
                bulletBoss.x = boss[currentBoss].x + 12;
                bulletBoss.y = boss[currentBoss].y + 26;
            }
            bossShotFire(&bulletBoss);
       }

       performantdelay(5);      
    }
    
}

void nextFase(){
    currentFase++;
    switch (currentFase)
    {
    case 1:
        startFase1();
        break;
    
    case 2:
        startFase2();
        break;
    
    case 3:
        menu();
        break;

    case 4:
        //Significa que matou o último boss
        //Implementar a ação de salvar quando zerar o jogo
        //saveGame();
        break;    
    }
}

void startFase1(){
    isInGame = 1;
    currentBoss = 0;//Boss 0 na fase 1
    j = 0;
    set_sprite_data(0, 5, naveSprite);//Carregando sprites da nave
    set_sprite_data(5, 17, boss1Sprite);//Carregando sprites do boss
    //set_sprite_data(24,9,lifeBossSprite);
    //set_sprite_data(22, 1, heartSprite);
    //set_bkg_data(45, 3, backgroundFase1Sprite);//Carregando background da fase 1
    set_bkg_data(45, 0x2D, bkg_data);
    
    for(i = 0; i < 32; i+=8)
        for(j = 0; j < 32; j+=8)
            set_bkg_tiles(i, j, 8, 8, bkg_tiles);
    //set_bkg_tiles(0, 0, 20, 18, fase1Bkg);

    //setupHUD(10);//Coloca na tela 10 corações

    setupNave();//Carrega as características da nave
    setupBoss1();//Carrega as características do boss1

    faseName[0] = 30; //Pega a sprite de espaço em branco
    faseName[1] = 6; //Pega a sprite da letra F
    faseName[2] = 32; //Pega a sprite do número 1, que corresponde a fase atual

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    inGaming();
    move_bkg(0, 0);//Como o background estará se movendo, ele retorna a posição original
}

void startFase2(){
    isInGame = 1;
    currentBoss = 1;//Boss 1 na fase 2
    HIDE_BKG;
    HIDE_SPRITES;
    j = 0;
    //set_sprite_data(0, 5, naveSprite);//Já foi carregado em startFase1()
    set_sprite_data(5, 17, boss2Sprite);//Carregando sprites do boss
    //set_bkg_data(45, 3, backgroundFase1Sprite);//Carregando background da fase 1
    //set_bkg_data(45, 0x2D, bkg_data); Já foi carregado na fase anterior
    
    //for(i = 0; i < 32; i+=8)
        //for(j = 0; j < 32; j+=8)
            //set_bkg_tiles(i, j, 8, 8, bkg_tiles);
    //set_bkg_tiles(0, 0, 20, 18, fase1Bkg);

    //setupHUD(10);//Coloca na tela 10 corações

    setupNave();//Carrega as características da nave
    setupBoss2();//Carrega as características do boss1

    faseName[0] = 30; //Pega a sprite de espaço em branco
    faseName[1] = 6; //Pega a sprite da letra F
    faseName[2] = 33; //Pega a sprite do número 2, que corresponde a fase atual

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    inGaming();
    move_bkg(0, 0);//Como o background estará se movendo, ele retorna a posição original
}

void startFase3(){
    isInGame = 1;
}

void showScores(){
    isInsHighscoreScreen = 1;
    //set_bkg_data(0, 45, keyboardData);
    move_bkg(0, 0);
    set_bkg_tiles(0, 0, 20, 18, screenHighscoreBkg);

    drawPlayerNameInHighScore();

    SHOW_BKG;
    HIDE_SPRITES;
    DISPLAY_ON;

    while(isInsHighscoreScreen){
        switch (joypad()){
        case J_A:
            isInsHighscoreScreen = 0;
            menu();
            break;
        }
        performantdelay(2);
    }
}

void screenHighscore(){
    if(playerName[0] > 0x28){//0x28 em decimal é 40 e se o primeiro valos nesse vetor for maior que 40 significa que nele está armazenado lixo de memória e, portanto, nada foi salvo até o momento
        //Por enquanto não faz nada, porque se não tem nada salvo, então não abre a tela de highscore
    }else{
        showScores();
    }
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
    ENABLE_RAM_MBC1;//Funciona de forma semelhante a abrir e fechar um arquivo em C
    //set sprite do cursor
    set_sprite_data(0, 1, cursorSprite);
    set_sprite_tile(0, 0);

    move_bkg(0,0);
    currentFase = 1;
    cursor.x = 32;
    cursor.y = 96;
    cursor.col = 0;
    cursor.row = 0;
    move_sprite(0, cursor.x, cursor.y);

    //set_bkg_data(0, 45, fontMainScreen);
    set_bkg_data(0, 45, keyboardData);
    //set_bkg_tiles(0, 0, 20, 18, bgmainscreen);
    set_bkg_tiles(0, 0, 20, 18, mainScreenBkg);
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
                startFase1();
            }else if(cursor.y == 112){//Vai para a tela de Highscore
                screenHighscore();
            }
        }
        performantdelay(2);
    }
    DISABLE_RAM_MBC1;
}

void main(){
    NR52_REG = 0x80;
    NR51_REG = 0x11;
    NR50_REG = 0x77;

    while(1){
        menu();
    }
   
}