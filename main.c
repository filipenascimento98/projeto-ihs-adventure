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
UINT8 oitava = 5;
UINT8 i;
UINT8 tocou = 0;
UINT8 j;
UINT8 isInMainScreen = 1;
UINT8 isInsHighscoreScreen = 0;
UINT8 currentFase = 1; //Começa na fase 1 e muda conforme passa de fase
UINT8 currentBoss = 0; //Armazena a informação de qual Boss está em tela atualmente
UINT8 isInGame = 0;
UINT8 qtdBalas = 5;
UINT8 dirXBoss = 1;
UINT8 dirYBoss = 2;
UINT8 dir3var = 0;
UINT8 valDif = 5;
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
GameBullet bulletBoss1;
GameBullet bulletBoss2[3];

GameLifeBoss lifeBoss;

void performantdelay(UINT8 numloops)
{
    UINT8 ii;
    for (ii = 0; ii < numloops; ii++)
    {
        wait_vbl_done();
    }
}

UBYTE checkcollisionsNave(GameCharacterNave *one, GameBullet *two)
{
    return (one->x >= two->x && one->x <= two->x + two->width) && (one->y >= two->y && one->y <= two->y + two->height) || (two->x >= one->x && two->x <= one->x + one->width) && (two->y >= one->y && two->y <= one->y + one->height) ||
           (one->x >= two->x && one->x <= two->x + two->width) && (two->y >= one->y && two->y <= one->y + one->height) || (two->x >= one->x && two->x <= one->x + one->width) && (one->y >= two->y && one->y <= two->y + two->height);
}

UBYTE checkcollisionsBoss(GameCharacterBoss *one, GameBullet *two)
{
    return (one->x >= two->x && one->x <= two->x + two->width) && (one->y >= two->y && one->y <= two->y + two->height) || (two->x >= one->x && two->x <= one->x + one->width) && (two->y >= one->y && two->y <= one->y + one->height);
}

//Começa save com teclado
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
    if(currentFase == 1){
        faseName[2] = 32; //Pega a sprite do número 1, que corresponde a fase atual
    }else{
        faseName[2] = 33; //Pega a sprite do número 2, que corresponde a fase atual
    }
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
        faseName[0] = 30; //Pega a sprite de espaço em branco
        faseName[1] = 6; //Pega a sprite da letra F
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
    if(faseName[2] == 32 && currentFase == 2 && faseName[2] != 0x0E){
        faseName[2] = 33; //Pega a sprite do número 1, que corresponde a fase atual
        resetCharacterName();
        playerHasName = 0;
        playerName[0] = 0x29;
        nameCharacterIndex = 0;
    }
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
//Termina save com teclado

void moveGameBoss(GameCharacterBoss *character, UINT8 x, UINT8 y)
{
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

void moveGameNave(GameCharacterNave *character, UINT8 x, UINT8 y)
{
    move_sprite(character->spritids[0], x, y);
    move_sprite(character->spritids[1], x + spriteSize, y);
    move_sprite(character->spritids[2], x, y + spriteSize);
    move_sprite(character->spritids[3], x + spriteSize, y + spriteSize);
}

void moveGameBullet(GameBullet *bullet, UINT8 x, UINT8 y)
{
    move_sprite(bullet->spriteId, x, y);
}



void bossMove()
{
    switch (currentBoss)
    {
    case 0:
        if (!dirXBoss)
        {
            if (boss[currentBoss].x - 4 <= 13)
            {
                boss[currentBoss].x = 13;
                dirXBoss = 1;
            }
            else
            {
                boss[currentBoss].x -= 4;
                boss[currentBoss].y += dirYBoss;
                dirYBoss *= -1;
            }
            moveGameBoss(&boss[currentBoss], boss[currentBoss].x, boss[currentBoss].y);
        }
        if (dirXBoss)
        {
            if (boss[currentBoss].x + 36 >= 160)
            {
                dirXBoss = 128;
                dirXBoss = 0;
            }
            else
            {
                boss[currentBoss].x += 4;
                boss[currentBoss].y += dirYBoss;
                dirYBoss *= -1;
            }
            moveGameBoss(&boss[currentBoss], boss[currentBoss].x, boss[currentBoss].y);
        }
        break;
    case 1:
        if (!dirXBoss)
        {
            if (boss[currentBoss].x - 4 <= 13)
            {
                boss[currentBoss].x = 13;
                dirXBoss = 1;
            }
            else
            {
                boss[currentBoss].x -= 6;
                boss[currentBoss].y += dirYBoss;
                dir3var++;
                if(dir3var%3 == 0){
                    dir3var = 0;
                    dirYBoss *= -1;
                }
            }
            moveGameBoss(&boss[currentBoss], boss[currentBoss].x, boss[currentBoss].y);
        }
        if (dirXBoss)
        {
            if (boss[currentBoss].x + 36 >= 160)
            {
                dirXBoss = 128;
                dirXBoss = 0;
            }
            else
            {
                boss[currentBoss].x += 6;
                boss[currentBoss].y += dirYBoss;
                dir3var++;
                if(dir3var%3 == 0){
                    dir3var = 0;
                    dirYBoss *= -1;
                }
            }
            moveGameBoss(&boss[currentBoss], boss[currentBoss].x, boss[currentBoss].y);
        }
        break;

    default:
        break;
    }

}


void setupBulletNave()
{
    for (i = 0; i < qtdBalas; i++)
    {
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

void setupNave()
{
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

void setupBulletBoss1()
{
    bulletBoss1.x = 180;
    bulletBoss1.y = 150;
    bulletBoss1.width = 8;
    bulletBoss1.height = 8;

    set_sprite_tile(25, 21);
    bulletBoss1.spriteId = 25;
}

void setupBoss1()
{
    setupBulletBoss1();
    boss[0].bullet = &bulletBoss1;
    boss[0].x = 70;
    boss[0].y = 50;
    boss[0].width = 32;
    boss[0].height = 32;
    boss[0].life = 10;

    for (i = 0; i < 16; i++)
    {
        set_sprite_tile(9 + i, 5 + i);
        boss[0].spritids[i] = 9 + i;
    }

    moveGameBoss(&boss[0], boss[0].x, boss[0].y);
}

void setupBulletBoss2()
{
    for (i = 0; i < 3; i++)
    {
        bulletBoss2[i].x = 180;
        bulletBoss2[i].y = 150;
        bulletBoss2[i].width = 8;
        bulletBoss2[i].height = 8;
        bulletBoss2[i].disponivel = 1;
        bulletBoss2[i].spriteId = 25 + i;
        set_sprite_tile(bulletBoss2[i].spriteId, 21);
        moveGameBullet(&bulletBoss2[i], 180, 150);
    }
}

void setupBoss2()
{
    setupBulletBoss2();
    boss[1].bullet = &bulletBoss2;
    boss[1].x = 70;
    boss[1].y = 50;
    boss[1].width = 32;
    boss[1].height = 32;
    boss[1].life = 15;

    for (i = 0; i < 16; i++)
    {
        set_sprite_tile(9 + i, 5 + i);
        boss[1].spritids[i] = 9 + i;
    }

    moveGameBoss(&boss[1], boss[1].x, boss[1].y);
}

void playShotSoundNave()
{
    NR10_REG = 0x1E;
    NR11_REG = 0x10;
    NR12_REG = 0xF3;
    NR13_REG = 0x00;
    NR14_REG = 0x87;
}
/* void DO(){
    switch (oitava)
    {
        case 5:
            NR10_REG = 0x00;//Dó5
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x10;
            NR14_REG = 0x82;
            break;
        case 6:
            NR10_REG = 0x00;//Dó6
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x20;
            NR14_REG = 0x84;
            break;

        NR10_REG = 0x00;//Dó#5
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0x2F;
        NR14_REG = 0x82;    
    }
}

void RE(){
    switch (oitava)
    {
        case 5:
            NR10_REG = 0x00;//Ré5
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x51;
            NR14_REG = 0x82;
            break;
        case 6:
            NR10_REG = 0x00;//Ré6
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0xA1;
            NR14_REG = 0x84;
            break;

        NR10_REG = 0x00;//Ré#5
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0x74;
        NR14_REG = 0x82;

        NR10_REG = 0x00;//Ré#6
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0xE7;
        NR14_REG = 0x84;    
    }
    
}

void MI(){
    switch (oitava)
    {
        case 5:
            NR10_REG = 0x00;//Mi5
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x9A;
            NR14_REG = 0x82;
            break;
        case 6:
            NR10_REG = 0x00;//Mi6
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x33;
            NR14_REG = 0x85;
            break;    
    }
    
}

void FA(){
    switch (oitava)
    {
        case 5:
            NR10_REG = 0x00;//Fá5
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0xC1;
            NR14_REG = 0x82;
            break;
        case 6:
            NR10_REG = 0x00;//Fá6
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x82;
            NR14_REG = 0x85;
            break;
        NR10_REG = 0x00;//Fá#5
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0xEA;
        NR14_REG = 0x82;   

        NR10_REG = 0x00;//Fá#6
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0xD5;
        NR14_REG = 0x85;   
    }
    
}

void SOL(){
    switch (oitava)
    {
        case 5:
            NR10_REG = 0x00;//Sol5
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x17;
            NR14_REG = 0x83;
            break;
        case 6:
            NR10_REG = 0x00;//Sol6
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x2E;
            NR14_REG = 0x86;
            break;    

        NR10_REG = 0x00;//Sol#5
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0x46;
        NR14_REG = 0x83;

        NR10_REG = 0x00;//Sol#6
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0x8C;
        NR14_REG = 0x86;   
    }
    
}

void LA(){
    switch (oitava)
    {
        case 5:
            NR10_REG = 0x00;//Lá5
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0x78;
            NR14_REG = 0x83;
            break;
        case 6:
            NR10_REG = 0x00;//Lá6
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0xF1;
            NR14_REG = 0x86;
            break;

        NR10_REG = 0x00;//Lá#5
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0xAD;
        NR14_REG = 0x83;

        NR10_REG = 0x00;//Lá#6
        NR11_REG = 0x81;
        NR12_REG = 0x43;
        NR13_REG = 0x59;
        NR14_REG = 0x87;        
    }
    
}

void SI(){
    switch (oitava)
    {
        case 5:
            NR10_REG = 0x00;//Si5
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0xE5;
            NR14_REG = 0x83;
            break;
        case 6:
            NR10_REG = 0x00;//Si6
            NR11_REG = 0x81;
            NR12_REG = 0x43;
            NR13_REG = 0xCA;
            NR14_REG = 0x87;
            break;    
    }
} */

void playLobbySound()
{
    NR10_REG = 0x00;
    NR11_REG = 0x81;
    NR12_REG = 0x43;
    NR13_REG = 0x73;
    NR14_REG = 0x86;
}

void playWinGameSound()
{
    for(j = 0; j < 2; j++){
        for (i = 0; i < 2; i++){
            NR10_REG = 0x00; //Sol5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x17;
            NR14_REG = 0x83;
            performantdelay(20);
            NR10_REG = 0x00; //Muta
            NR11_REG = 0x00;
            NR12_REG = 0x00;
            NR13_REG = 0x00;
            NR14_REG = 0x80;
            performantdelay(10);
            NR10_REG = 0x00; //Sol5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x17;
            NR14_REG = 0x83;
            performantdelay(30);
            NR10_REG = 0x00; //Si5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0xE5;
            NR14_REG = 0x83;
            performantdelay(20);
            NR10_REG = 0x00; //Dó6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x20;
            NR14_REG = 0x84;
            performantdelay(20);
            NR10_REG = 0x00; //Sol5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x17;
            NR14_REG = 0x83;
            performantdelay(20);
            NR10_REG = 0x00; //Muta
            NR11_REG = 0x00;
            NR12_REG = 0x00;
            NR13_REG = 0x00;
            NR14_REG = 0x80;
            performantdelay(10);
            NR10_REG = 0x00; //Sol5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x17;
            NR14_REG = 0x83;
            performantdelay(30);
            NR10_REG = 0x00; //Fá5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0xC1;
            NR14_REG = 0x82;
            performantdelay(20);
            NR10_REG = 0x00; //Fá#5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0xEA;
            NR14_REG = 0x82;
            performantdelay(20);
        }

        if(tocou == 0){
            NR10_REG = 0x00;//Lá#6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x59;
            NR14_REG = 0x87; 
            performantdelay(10);
            NR10_REG = 0x00;//Sol6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x2E;
            NR14_REG = 0x86; 
            performantdelay(10);
            NR10_REG = 0x00;//Ré6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0xA1;
            NR14_REG = 0x84;
            performantdelay(80);
            NR10_REG = 0x00;//Lá#6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x59;
            NR14_REG = 0x87; 
            performantdelay(10);
            NR10_REG = 0x00;//Sol6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x2E;
            NR14_REG = 0x86; 
            performantdelay(10);
            NR10_REG = 0x00;//Dó#5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x2F;
            NR14_REG = 0x82; 
            performantdelay(80);
            NR10_REG = 0x00;//Lá#6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x59;
            NR14_REG = 0x87; 
            performantdelay(10);
            NR10_REG = 0x00;//Sol6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x2E;
            NR14_REG = 0x86;  
            performantdelay(10);
            NR10_REG = 0x00;//Dó6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x20;
            NR14_REG = 0x84;
            performantdelay(80);
            NR10_REG = 0x00;//Si5
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0xE5;
            NR14_REG = 0x83;
            performantdelay(10);
            NR10_REG = 0x00;//Dó6
            NR11_REG = 0x81;
            NR12_REG = 0x4B;
            NR13_REG = 0x20;
            NR14_REG = 0x84;
            performantdelay(10);
            NR10_REG = 0x00; //Muta
            NR11_REG = 0x00;
            NR12_REG = 0x00;
            NR13_REG = 0x00;
            NR14_REG = 0x80;
            performantdelay(40);
            tocou = 1;
        }
    }

    NR10_REG = 0x00; //Sol5
    NR11_REG = 0x81;
    NR12_REG = 0x4B;
    NR13_REG = 0x17;
    NR14_REG = 0x83;
    performantdelay(20);
    NR10_REG = 0x00; //Muta
    NR11_REG = 0x00;
    NR12_REG = 0x00;
    NR13_REG = 0x00;
    NR14_REG = 0x80;
    tocou = 0;
}

void naveShotFire(GameBullet *bullet)
{
    if (!bullet->disponivel)
    {
        if (bullet->y <= 5)
        {
            bullet->disponivel = 1;
            moveGameBullet(bullet, 180, 150);
        }
        else
        {
            bullet->y += -5;
            moveGameBullet(bullet, bullet->x, bullet->y);
            if (checkcollisionsBoss(&boss[currentBoss], bullet))
            {
                boss[currentBoss].life -= 1;
                bullet->disponivel = 1;
                moveGameBullet(bullet, 180, 180);
                if (boss[currentBoss].life == 0)
                {
                    performantdelay(5);
                    moveGameBoss(&boss[currentBoss], 180, 180);
                    for (i = 0; i < qtdBalas; i++)
                    {
                        moveGameBullet(&bulletNave[i], 180, 180);
                    }
                    if(currentBoss == 0)
                        moveGameBullet(&bulletBoss1, 180, 180);
                    else{
                        moveGameBullet(&bulletBoss2[0], 180, 180);
                        moveGameBullet(&bulletBoss2[1], 180, 180);
                        moveGameBullet(&bulletBoss2[2], 180, 180);
                    }
                    moveGameNave(&nave, 180, 180);
                    nextFase();
                }
            }
        }
    }
}

void bossShotFire()
{
    switch (currentBoss)
    {
    case 0:
        if (!bulletBoss1.disponivel)
        {
            if (bulletBoss1.y >= 155)
            {
                bulletBoss1.disponivel = 1;
                moveGameBullet(&bulletBoss1, 180, 150);
            }
            else
            {
                bulletBoss1.y += valDif;
                moveGameBullet(&bulletBoss1, bulletBoss1.x, bulletBoss1.y);
                if (checkcollisionsNave(&nave, &bulletBoss1))
                {
                    performantdelay(5);
                    isInGame = 0;
                    moveGameBoss(&boss[currentBoss], 180, 180);
                    for (i = 0; i < qtdBalas; i++)
                    {
                        moveGameBullet(&bulletNave[i], 180, 180);
                    }
                    moveGameBullet(&bulletBoss1, 180, 180);
                    moveGameNave(&nave, 180, 180);
                    saveGame();
                }
            }
        }
        break;
    case 1:
        if (!bulletBoss2[0].disponivel)
        {
            if (bulletBoss2[0].y >= 155)
            {
                bulletBoss2[0].disponivel = 1;
                moveGameBullet(&bulletBoss2[0], 180, 150);
                moveGameBullet(&bulletBoss2[1], 180, 150);
                moveGameBullet(&bulletBoss2[2], 180, 150);
            }
            else
            {
                bulletBoss2[0].y += (valDif + 2);
                bulletBoss2[1].y += (valDif + 2);
                bulletBoss2[2].y += (valDif + 2);

                moveGameBullet(&bulletBoss2[0], bulletBoss2[0].x, bulletBoss2[0].y);
                moveGameBullet(&bulletBoss2[1], bulletBoss2[1].x, bulletBoss2[1].y);
                moveGameBullet(&bulletBoss2[2], bulletBoss2[2].x, bulletBoss2[2].y);

                if (checkcollisionsNave(&nave, &bulletBoss2[0]) || checkcollisionsNave(&nave, &bulletBoss2[1]) || checkcollisionsNave(&nave, &bulletBoss2[2]))
                {
                    performantdelay(5);
                    isInGame = 0;
                    moveGameBoss(&boss[currentBoss], 180, 180);
                    for (i = 0; i < qtdBalas; i++)
                    {
                        moveGameBullet(&bulletNave[i], 180, 180);
                    }
                    
                    moveGameBullet(&bulletBoss2[0], 180, 180);
                    moveGameBullet(&bulletBoss2[1], 180, 180);
                    moveGameBullet(&bulletBoss2[2], 180, 180);

                    moveGameNave(&nave, 180, 180);
                    saveGame();
                }
            }
        }
        break;
    default:
        break;
    }
}

void inGaming()
{

    while (isInGame)
    {
        scroll_bkg(1, 1);
        if (joypad() & J_LEFT)
        {
            if (nave.x - 4 <= 13)
            {
                nave.x = 13;
            }
            else
            {
                nave.x -= 4;
            }
            moveGameNave(&nave, nave.x, nave.y);
        }
        if (joypad() & J_RIGHT)
        {
            if (nave.x + 4 >= 147)
            {
                nave.x = 147;
            }
            else
            {
                nave.x += 4;
            }
            moveGameNave(&nave, nave.x, nave.y);
        }
        if (joypad() & J_UP)
        {
            for (i = 0; i < qtdBalas; i++)
            {
                if (bulletNave[i].disponivel)
                {
                    playShotSoundNave();
                    bulletNave[i].disponivel = 0;
                    bulletNave[i].x = nave.x + 7;
                    bulletNave[i].y = nave.y - 3;
                    break;
                }
            }
        }

        bossMove();

        if (isInGame)
        {
            for (i = 0; i < qtdBalas; i++)
            {
                naveShotFire(&bulletNave[i]);
            }
        }
        switch (currentFase)
        {
        case 1:
            if (isInGame)
            {
                if (bulletBoss1.disponivel == 1)
                {
                    bulletBoss1.disponivel = 0;
                    bulletBoss1.x = boss[currentBoss].x + 12;
                    bulletBoss1.y = boss[currentBoss].y + 26;
                }
                bossShotFire(&bulletBoss1);
            }
            break;
        case 2:
            if (isInGame)
            {
                if (bulletBoss2[0].disponivel == 1)
                {
                    bulletBoss2[0].disponivel = 0;
                    bulletBoss2[0].x = boss[currentBoss].x + 2;
                    bulletBoss2[0].y = boss[currentBoss].y + 26;

                    bulletBoss2[1].x = boss[currentBoss].x + 12;
                    bulletBoss2[1].y = boss[currentBoss].y + 26;

                    bulletBoss2[2].x = boss[currentBoss].x + 22;
                    bulletBoss2[2].y = boss[currentBoss].y + 26;
                }
                bossShotFire(&bulletBoss2);
            }
        default:
            break;
        }

        performantdelay(5);
    }
}

void nextFase()
{
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
        move_bkg(0,0);
        set_bkg_data(0, 45, keyboardData);
        set_bkg_tiles(0,0,20,18, tela_final);
        playWinGameSound();
        waitpad(J_START);
        saveGame();
        faseName[0] = 23;
        faseName[1] = 9;
        faseName[2] = 0x0E;
        menu();
        break;
    }
}

void startFase1()
{
    if(faseName[2] != 33 && faseName[2] != 0x0E){
        faseName[2] = 32; //Pega a sprite do número 1, que corresponde a fase atual
    }
    isInGame = 1;
    currentBoss = 0; //Boss 0 na fase 1
    j = 0;
    set_sprite_data(0, 5, naveSprite);   //Carregando sprites da nave
    set_sprite_data(5, 17, boss1Sprite); //Carregando sprites do boss

    //set_sprite_data(24,9,lifeBossSprite);
    //set_sprite_data(22, 1, heartSprite);
    //set_bkg_data(45, 3, backgroundFase1Sprite);//Carregando background da fase 1

    set_bkg_data(45, 0x2D, bkg_data);

    for (i = 0; i < 32; i += 8)
        for (j = 0; j < 32; j += 8)
            set_bkg_tiles(i, j, 8, 8, bkg_tiles);

    

    setupNave();  //Carrega as características da nave
    setupBoss1(); //Carrega as características do boss1

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    inGaming();
    move_bkg(0, 0); //Como o background estará se movendo, ele retorna a posição original
}

void startFase2()
{
    isInGame = 1;
    currentBoss = 1; //Boss 1 na fase 2
    HIDE_BKG;
    HIDE_SPRITES;
    j = 0;
    set_sprite_data(5, 17, boss2Sprite); //Carregando sprites do boss

    setupNave();  //Carrega as características da nave
    setupBoss2(); //Carrega as características do boss1

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    inGaming();
    move_bkg(0, 0); //Como o background estará se movendo, ele retorna a posição original
}

void startFase3()
{
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


void fadeout()
{
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
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

void fadein()
{
    for (i = 0; i < 3; i++)
    {
        switch (i)
        {
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

void menu()
{

    ENABLE_RAM_MBC1; //Funciona de forma semelhante a abrir e fechar um arquivo em C
    //set sprite do cursor
    set_sprite_data(0, 1, cursorSprite);
    set_sprite_tile(0, 0);

    move_bkg(0, 0);
    currentFase = 1;
    cursor.x = 32;
    cursor.y = 96;
    cursor.col = 0;
    cursor.row = 0;
    move_sprite(0, cursor.x, cursor.y);

    set_bkg_data(0, 45, keyboardData);

    set_bkg_tiles(0, 0, 20, 18, mainScreenBkg);
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    isInMainScreen = 1;

    while (isInMainScreen)
    {
        switch (joypad())
        {
        case J_UP:
            if (cursor.y - 16 < 96)
            {
                cursor.y = 96;
            }
            else
            {
                //playWinGameSound();
                playLobbySound();
                cursor.y -= 16;
            }
            move_sprite(0, cursor.x, cursor.y);
            break;
        case J_DOWN:
            if (cursor.y + 16 > 112)
            {
                cursor.y = 112;
            }
            else
            {
                playLobbySound();
                cursor.y += 16;
            }
            move_sprite(0, cursor.x, cursor.y);
            break;
        case J_START:
            if (cursor.y == 96)
            { //Inicia o jogo
                isInMainScreen = 0;
                startFase1();
            }
            else if (cursor.y == 112)
            { //Vai para a tela de Highscore
               screenHighscore();
            }
        }
        performantdelay(2);
    }
    DISABLE_RAM_MBC1;
}

void main()
{
    NR52_REG = 0x80;
    NR51_REG = 0x11;
    NR50_REG = 0x77;

    while (1)
    {
        menu();
    }
}