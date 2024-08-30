#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "led.h"
#include "main.h"
#include "math.h"
#include "tft_init.h"
#include "tft.h"
#include "key.h"
#include "ec11.h"
#include "brick.h"

#define BRK_GAME_READY  0
#define BRK_GAME_RUN  1
#define BRK_GAME_PAUSE  2
#define BRK_GAME_CLEAR  3
#define BRK_GAME_LOSE  4
#define BRK_GAME_FAIL  5
#define BRK_GAME_WIN  6

#define BRK_GAME_KEYA 0b00000001
#define BRK_GAME_KEYB 0b00000010
#define BRK_GAME_KEYD 0b00000100
#define BRK_GAME_KEY1 0b00001000
#define BRK_GAME_KEY2 0b00010000
#define BRK_GAME_KEY3 0b00100000

#define NUM_BRICK_PER_ROW 8
#define NUM_ROW 10
#define NUM_LEVEL 4

#define SPEED_MODE_1X 0
#define SPEED_MODE_2X 1
#define SPEED_MODE_4X 2
#define SPEED_MODE_8X 3

const double SPEED_MODOE_VAL[4] = { 1.0, 2.0, 4.0, 8.0};

// 小球参数
const int BALL_COLOR = WHITE;
const int BALL_OFFSET_FROM_BAR_X = 12;
const int BALL_OFFSET_FROM_BAR_Y = -4;
const double BALL_RAD = 1.5;

const int WALL_COLOR = LIGHTGREEN;
const int BACKGROUND_COLOR = BLACK;

const int DEATH_POSY = 158;

const int BAR_WIDTH = 25;
const int BAR_HIGHT = 2;
const int BAR_POSY = 150;
const int BAR_COLOR = YELLOW;

const int BRICK_WIDTH = 11; /// odd number 
const int BRICK_HEIGHT = 5; ///

const int BRICK_START_OFFSET_X = 3;
const int BRICK_START_OFFSET_Y = 3;

const uint8_t LEVEL_BRICKS[NUM_LEVEL][NUM_ROW] = {
     {
        0b00000000,
        0b00000000,
        0b10000001,
        0b11000011,
        0b11000011,
        0b11100111,
        0b11100111,
        0b11100111,
        0b00000000,
        0b00000000
    },
    {
        0b00000000,
        0b11111111,
        0b00000000,
        0b11111111,
        0b00010000,
        0b11111111,
        0b00000000,
        0b11111111,
        0b00000000,
        0b11111111
    },
    {
        0b10101010,
        0b01010101,
        0b10101010,
        0b01010101,
        0b10101010,
        0b01010101,
        0b10101010,
        0b01010101,
        0b10101010,
        0b01010101
    },
    {
        0b00000000,
        0b01100110,
        0b11111111,
        0b11111111,
        0b01111110,
        0b01111110,
        0b00111100,
        0b00111100,
        0b00011000,
        0b00000000
    }
};


// from tft_init.c  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏
extern int USE_HORIZONTAL;

// 点结构
typedef struct {
    double x;
    double y;
} Point;

// 线段结构
typedef struct {
    Point start;
    Point end;
    int type;
} Segment;

// 关卡结构
typedef struct {
    const uint8_t* bricks;
    uint8_t* dirtyBrickRowFlag;
    uint8_t* rtBricks;
    int currentLevel;
} Level;
Level mLevel;
uint8_t mDirtyFlag[NUM_ROW];
uint8_t mRtBricks[NUM_ROW];

// 托盘结构体
typedef struct {
    double x;
    double y;
    double dvx;
    double dvy;
    int lastDisplayX;
    int lastDisplayY;
    int dirty;
} Bar;
Bar mBar;

// 小球结构体
typedef struct {
    double x;
    double y;
    double dvx;
    double dvy;
    int lastDisplayX;
    int lastDisplayY;
    int dirty;
} Ball;
Ball mBall;


// 游戏结构体
typedef struct {
    Ball* ball;
    Bar* bar;
    uint8_t key;
    int status;
    Level* level;
    int hp;
    int isDead;
    int speedMode;
} Game;
Game game;// 全局


// 普通线段与水平竖直线段相交求解
int segmentCross(Segment* segIn, Segment* segHVIn, Point* hitPointOut, double* rate) {
    Point tempPoint;
    double tempRate;
    // 水平线 （法线向上）
    if (segHVIn->type == 1 && segIn->start.y < segHVIn->start.y && segIn->end.y > segHVIn->start.y ) {
        tempPoint.y = segHVIn->start.y;
        tempRate = (segHVIn->start.y - segIn->start.y) / (segIn->end.y - segIn->start.y);
        tempPoint.x = segIn->start.x + (segIn->end.x - segIn->start.x) * tempRate;
        if ( tempPoint.x > segHVIn->start.x  && tempPoint.x < segHVIn->end.x ) {
            hitPointOut->x = tempPoint.x;
            hitPointOut->y = tempPoint.y;
            *rate = tempRate;
            return 1;
        }
    } else if (segHVIn->type == 3 && segIn->start.y > segHVIn->start.y && segIn->end.y < segHVIn->start.y) {
        // 水平线 （法线向下）
        tempPoint.y = segHVIn->start.y;
        tempRate = (segHVIn->start.y - segIn->start.y) * 1.0  / (segIn->end.y - segIn->start.y);
        tempPoint.x = segIn->start.x + (segIn->end.x - segIn->start.x) * tempRate;
        if ( tempPoint.x > segHVIn->start.x  && tempPoint.x < segHVIn->end.x ) {
            hitPointOut->x = tempPoint.x;
            hitPointOut->y = tempPoint.y;
            *rate = tempRate;
            return 1;
        }
    } else if (segHVIn->type == 2 && segIn->start.x > segHVIn->start.x && segIn->end.x < segHVIn->start.x) {
        // 竖直线 （法线向右）
        tempPoint.x = segHVIn->start.x;
        tempRate = (segHVIn->start.x - segIn->start.x) * 1.0  / (segIn->end.x - segIn->start.x);
        tempPoint.y = segIn->start.y + (segIn->end.y - segIn->start.y) * tempRate;
        if ( tempPoint.y > segHVIn->start.y  && tempPoint.y < segHVIn->end.y ) {
            hitPointOut->x = tempPoint.x;
            hitPointOut->y = tempPoint.y;
            *rate = tempRate;
            return 1;
        }
    } else if (segHVIn->type == 4 && segIn->start.x < segHVIn->start.x && segIn->end.x > segHVIn->start.x) {
        // 水平线 （法线向左）
        tempPoint.x = segHVIn->start.x;
        tempRate = (segHVIn->start.x - segIn->start.x) * 1.0  / (segIn->end.x - segIn->start.x);
        tempPoint.y = segIn->start.y + (segIn->end.y - segIn->start.y) * tempRate;
        if ( tempPoint.y > segHVIn->start.y  && tempPoint.y < segHVIn->end.y ) {
            hitPointOut->x = tempPoint.x;
            hitPointOut->y = tempPoint.y;
            *rate = tempRate;
            return 1;
        }
    }

    return 0;
}

void gameResetKey()
{
    game.key = 0;
}


///////////////////////
void moveBallWithBar() {
    game.ball->x = game.bar->x + BAR_WIDTH / 2;
    game.ball->y = game.bar->y - BALL_RAD;
    game.ball->dirty = 1;
}

void computeBarMotion (double deltaTimeIn) {
    double direction = 0.0;
    if (game.key & BRK_GAME_KEYA) {
        direction = -1.0;
    } else if(game.key & BRK_GAME_KEYB) {
        direction = 1.0;
    }

    game.bar->dirty = (game.key & (BRK_GAME_KEYA | BRK_GAME_KEYB)) ? 1 : 0;
    game.bar->x = game.bar->x + deltaTimeIn * game.bar->dvx * direction;
    
    // limit position
    if (game.bar->x < 1) {
        game.bar->x = 1;
    } else if (game.bar->x > (101 - BAR_WIDTH)) {
        game.bar->x = 101 - BAR_WIDTH;
    }

}


// bar wall bar death area, state
int computeBallMotion(double deltaTimeIn, double* leftTimeOut) {
    Point originStart;
    Point deltaEnd;
    
    Segment seg;
    Segment target;
    
    Point hitPoint;
    double rate;
    
    int colIndex, rowIndex;
  
    originStart.x = game.ball->x + (game.ball->dvx > 0 ? BALL_RAD : - BALL_RAD); //球心修正
    originStart.y = game.ball->y + (game.ball->dvy > 0 ? BALL_RAD : - BALL_RAD); //球心修正
    
    deltaEnd.x = game.ball->x + game.ball->dvx * deltaTimeIn  + (game.ball->dvx > 0 ? BALL_RAD : - BALL_RAD);//球心修正
    deltaEnd.y = game.ball->y + game.ball->dvy * deltaTimeIn  + (game.ball->dvy > 0 ? BALL_RAD : - BALL_RAD);//球心修正
    
    seg.start = originStart;
    seg.end = deltaEnd;
    seg.type = 0; // 普通
    
    game.ball->dirty = 1;
    

    // ball - death area hit test
    if (game.ball->y > DEATH_POSY) {
        game.isDead = 1;
        return 1;
    }

    // ball - wall hit test
    // 左
    target.start.x = 1;
    target.start.y = 1;
    target.end.x = 1;
    target.end.y = 159;
    target.type = 2;
    
    if (segmentCross(&seg, &target, &hitPoint, &rate)) {
        game.ball->x = hitPoint.x + (game.ball->dvx > 0 ? -BALL_RAD : BALL_RAD);//球心修正
        game.ball->y = hitPoint.y + (game.ball->dvy > 0 ? -BALL_RAD : BALL_RAD);//球心修正
        
        game.ball->dvx = -game.ball->dvx;
        *leftTimeOut = deltaTimeIn * (1 - rate);
        return 0;
    }
    
    // 上
    target.start.x = 1;
    target.start.y = 1;
    target.end.x = 101;
    target.end.y = 1;
    target.type = 3;
    
    if (segmentCross(&seg, &target, &hitPoint, &rate)) {
        game.ball->x = hitPoint.x + (game.ball->dvx > 0 ? -BALL_RAD : BALL_RAD);
        game.ball->y = hitPoint.y + (game.ball->dvy > 0 ? -BALL_RAD : BALL_RAD);//球心修正
        
        game.ball->dvy = -game.ball->dvy;
        *leftTimeOut = deltaTimeIn * (1 - rate);
        return 0;
    }
    
    // 右
    
    target.start.x = 101;
    target.start.y = 1;
    target.end.x = 101;
    target.end.y = 159;
    target.type = 4;
    
    if (segmentCross(&seg, &target, &hitPoint, &rate)) {
        game.ball->x = hitPoint.x + (game.ball->dvx > 0 ? -BALL_RAD : BALL_RAD);//球心修正
        game.ball->y = hitPoint.y + (game.ball->dvy > 0 ? -BALL_RAD : BALL_RAD);//球心修正
        *leftTimeOut = deltaTimeIn * (1 - rate);
        if (target.type == 2 || target.type ==4) {
            game.ball->dvx = - game.ball->dvx;
        } else {
            game.ball->dvy = - game.ball->dvy;
        }
        return 0;
    }

    
    // ball - bar hit test
    // 根据撞击位置偏转一定角度，
    target.start.x = game.bar->x;
    target.start.y = game.bar->y;
    target.end.x = game.bar->x + BAR_WIDTH;
    target.end.y = game.bar->y;
    target.type = 1;
    
    if (segmentCross(&seg, &target, &hitPoint, &rate)) {
        game.ball->x = hitPoint.x + (game.ball->dvx > 0 ? -BALL_RAD : BALL_RAD);//球心修正
        game.ball->y = hitPoint.y + (game.ball->dvy > 0 ? -BALL_RAD : BALL_RAD);//球心修正
        *leftTimeOut = deltaTimeIn * (1 - rate);
        if (target.type == 2 || target.type ==4) {
            game.ball->dvx = - game.ball->dvx;
        } else {
            game.ball->dvy = - game.ball->dvy;
        }
        return 0;
    }

    // ball - brick hit test
    // ball' motion length MUST shorter than a brick width
    // 

//    colIndex = (int)(round(game.ball->x - BRICK_START_OFFSET_X)) / (BRICK_WIDTH + 1);
//    rowIndex = (int)(round(game.ball->y - BRICK_START_OFFSET_Y)) / (BRICK_HEIGHT +1);
    
    colIndex = (int)(round(originStart.x - BRICK_START_OFFSET_X)) / (BRICK_WIDTH + 1);
    rowIndex = (int)(round(originStart.y - BRICK_START_OFFSET_Y)) / (BRICK_HEIGHT +1);
    
    if (game.level->rtBricks[rowIndex] & 1 << (7 - colIndex)) {
        game.level->dirtyBrickRowFlag[rowIndex] |=  1 << (7 - colIndex);
        game.level->rtBricks[rowIndex] &= ~(1 << (7 - colIndex));
        
        game.ball->dvx = - game.ball->dvx;
        game.ball->dvy = - game.ball->dvy;
        
        return 0;
    }
    

    if ( (game.ball->dvx > 0 && colIndex + 1 < NUM_BRICK_PER_ROW && rowIndex < NUM_ROW ) // 右侧方块 左边线
        || (game.ball->dvx < 0 && colIndex -1 >= 0 && rowIndex < NUM_ROW)) // 左侧方块 右边线
    { 
        int tempColIndex = colIndex + (game.ball->dvx > 0 ? 1 : -1) ;
        if (game.level->rtBricks[rowIndex] & 1 << (7 - tempColIndex) )
        {
            target.start.x = BRICK_START_OFFSET_X + colIndex * (BRICK_WIDTH + 1) + (game.ball->dvx > 0 ? (BRICK_WIDTH + 0.5) : -0.5);
            target.start.y = BRICK_START_OFFSET_Y + rowIndex * (BRICK_HEIGHT + 1);
            target.end.x = BRICK_START_OFFSET_X + colIndex * (BRICK_WIDTH + 1) + (game.ball->dvx > 0 ? (BRICK_WIDTH+ 0.5) : - 0.5);
            target.end.y = BRICK_START_OFFSET_Y + (rowIndex + 1) * (BRICK_HEIGHT + 1);
            target.type = game.ball->dvx > 0 ? 4 : 2;
            
             if (segmentCross(&seg, &target, &hitPoint, &rate)) {
                game.ball->x = hitPoint.x + (game.ball->dvx > 0 ? -BALL_RAD : BALL_RAD);//球心修正;
                game.ball->y = hitPoint.y + (game.ball->dvy > 0 ? -BALL_RAD : BALL_RAD);//球心修正
                 
                *leftTimeOut = deltaTimeIn * (1 - rate);
                if (target.type == 2 || target.type ==4) {
                    game.ball->dvx = - game.ball->dvx;
                } else {
                    game.ball->dvy = - game.ball->dvy;
                }
                
                game.level->dirtyBrickRowFlag[rowIndex] |=  1 << (7 - tempColIndex);
                game.level->rtBricks[rowIndex] &= ~(1 << (7 - tempColIndex));
                
                return 0;
            }
        }
    } 
    
    if ( (game.ball->dvy > 0 && colIndex < NUM_BRICK_PER_ROW && rowIndex + 1 < NUM_ROW ) // 下侧方块 上边线
        || (game.ball->dvy < 0 && colIndex < NUM_BRICK_PER_ROW && rowIndex < NUM_ROW && 0 <= rowIndex - 1)) // 上侧方块 下边线
    { 
        int tempRowIndex = rowIndex + (game.ball->dvy > 0 ? 1 : -1) ;
        if (game.level->rtBricks[tempRowIndex] & 1 << (7 - colIndex) )
        {
            target.start.x = BRICK_START_OFFSET_X + colIndex * (BRICK_WIDTH + 1);
            target.start.y = BRICK_START_OFFSET_Y + rowIndex * (BRICK_HEIGHT + 1) + (game.ball->dvy > 0 ? (BRICK_HEIGHT+ 0.5) : - 0.5);
            target.end.x = BRICK_START_OFFSET_X + (colIndex + 1) * (BRICK_WIDTH + 1);
            target.end.y = BRICK_START_OFFSET_Y + rowIndex * (BRICK_HEIGHT + 1) + (game.ball->dvy > 0 ? (BRICK_HEIGHT+ 0.5) : - 0.5);
            target.type = game.ball->dvy > 0 ? 1 : 3;
            
             if (segmentCross(&seg, &target, &hitPoint, &rate)) {
                game.ball->x = hitPoint.x + (game.ball->dvx > 0 ? -BALL_RAD : BALL_RAD);//球心修正
                game.ball->y = hitPoint.y + (game.ball->dvy > 0 ? -BALL_RAD : BALL_RAD);//球心修正
                 
                *leftTimeOut = deltaTimeIn * (1 - rate);
                if (target.type == 2 || target.type ==4) {
                    game.ball->dvx = - game.ball->dvx;
                } else {
                    game.ball->dvy = - game.ball->dvy;
                }
                
                game.level->dirtyBrickRowFlag[tempRowIndex] |=  1 << (7 - colIndex);
                game.level->rtBricks[tempRowIndex] &= ~(1 << (7 - colIndex));
                return 0;
            }
        }
    } 
    
    
    
    //// 无碰撞
    game.ball->x = game.ball->x + game.ball->dvx * deltaTimeIn;
    game.ball->y = game.ball->y + game.ball->dvy * deltaTimeIn;

    game.ball->dirty = 1;
    return 1;
}

///////////////////////

// 旋钮转动触发函数，顺时针 1 ， 逆时针 -1
void OnEc11RotateBrick(int direct)    
{
   if (direct > 0) {
        game.key &= ~BRK_GAME_KEYA;
        game.key |= BRK_GAME_KEYB;
   }
   else
   {
        game.key &= ~BRK_GAME_KEYB;
        game.key |= BRK_GAME_KEYA;
   }
}
// 旋钮点击操作
void OnEc11ClickBrick()
{
    game.key |= BRK_GAME_KEYD;
}

void OnKey1ClickBrick(void)
{    
    game.speedMode  = (game.speedMode + 1) % 4;
    DrawUI();
}


void DrawWall()
{
    TFT_Fill(1, 1, 101, 2, WALL_COLOR);
    TFT_Fill(1, 1, 2, 160, WALL_COLOR);
    TFT_Fill(100, 1, 101, 160, WALL_COLOR);
}
void DrawBall()
{
    int ballx= 0;
    int bally =0;
    
    if (game.ball->dirty == 1) {
        ballx = (int)(round(game.ball->x));
        bally = (int)(round(game.ball->y));
        
            
        TFT_Fill(
            game.ball->lastDisplayX - BALL_RAD / 2,
            game.ball->lastDisplayY - BALL_RAD / 2 ,
        
            game.ball->lastDisplayX + BALL_RAD / 2 + 1,
            game.ball->lastDisplayY + BALL_RAD / 2 + 1,
            BACKGROUND_COLOR);
        
        TFT_Fill(
            ballx - BALL_RAD / 2,
            bally - BALL_RAD / 2,
                
            ballx + BALL_RAD / 2 + 1,
            bally + BALL_RAD / 2 + 1,
            BALL_COLOR);
        
        game.ball->lastDisplayX = ballx;
        game.ball->lastDisplayY = bally;
        game.ball->dirty = 0;
    }
}

void DrawBar()
{
    int barx = 0;
    int bary = 0;
    int color = BAR_COLOR;
    
    if (game.bar->dvx > 0.0)
    {
        color = BLUE;
    }
    if(mBar.dirty == 1)
    {
        barx = (int)(round(game.bar->x));
        bary = (int)(round(game.bar->y));
        
        TFT_Fill(
            game.bar->lastDisplayX,
            game.bar->lastDisplayY,
            
            game.bar->lastDisplayX + BAR_WIDTH, 
            game.bar->lastDisplayY + BAR_HIGHT,
            BACKGROUND_COLOR);
        
        
        TFT_Fill(
            barx, 
            bary, 
            barx + BAR_WIDTH, 
            bary + BAR_HIGHT, 
            color);
        
        game.bar->lastDisplayX = barx;
        game.bar->lastDisplayY = bary;
        mBar.dirty = 0;
    }
}
void DrawUI() {
    
    char showData[32]={0};
    sprintf(showData,"LEVL");
    TFT_ShowString(103, 1, (uint8_t *)showData, WHITE, PURPLE,12, 0); 
    memset(showData,0, 32);
    
    sprintf(showData,"  %d ", game.level->currentLevel +1);
    TFT_ShowString(103, 14, (uint8_t *)showData, BLACK, GREEN,12, 0); 
    memset(showData,0, 32);
    
    
    sprintf(showData,"STAT");
    TFT_ShowString(103, 30, (uint8_t *)showData, WHITE, PURPLE,12, 0); 
    memset(showData,0, 32);
    switch (game.status) {
        case BRK_GAME_READY:
            sprintf(showData,"RDY ");
        break;
        case BRK_GAME_RUN:
            sprintf(showData,"RUN ");
        break;
        case BRK_GAME_PAUSE:
            sprintf(showData,"PAUS");
        break;
        case BRK_GAME_CLEAR:
            sprintf(showData,"CLR ");
        break;
        case BRK_GAME_LOSE:
            sprintf(showData,"LOSE ");
        break;
        case BRK_GAME_WIN:
            sprintf(showData,"WIN ");
        break;
    }
    
    TFT_ShowString(103, 43, (uint8_t *)showData, BLACK, GREEN,12, 0); 
    memset(showData,0, 32);
    
    ////////////
    
    
    sprintf(showData,"SPED");
    TFT_ShowString(103, 59, (uint8_t *)showData, WHITE, PURPLE,12, 0); 
    memset(showData,0, 32);
    
    switch (game.speedMode) {
        case SPEED_MODE_1X:
            sprintf(showData," 1X ");
        break;
        case SPEED_MODE_2X:
            sprintf(showData," 2X ");
        break;
        case SPEED_MODE_4X:
            sprintf(showData," 4X ");
        break;
        case SPEED_MODE_8X:
            sprintf(showData," 8X ");
        break;
    }    
    TFT_ShowString(103, 72, (uint8_t *)showData, BLACK, GREEN,12, 0); 
    memset(showData,0, 32);
    
    //////////////
    
    sprintf(showData,"LIFE");
    TFT_ShowString(103, 88, (uint8_t *)showData, WHITE, PURPLE,12, 0); 
    memset(showData,0, 32);
    
    sprintf(showData,"  %d ", game.hp);
    TFT_ShowString(103, 101, (uint8_t *)showData, BLACK, GREEN,12, 0); 
    memset(showData,0, 32);

}


void drawOneBrick(int colIndex, int rowIndex)
{
    TFT_Fill(
        BRICK_START_OFFSET_X + colIndex * (BRICK_WIDTH + 1),
        BRICK_START_OFFSET_Y + rowIndex * (BRICK_HEIGHT + 1),
        BRICK_START_OFFSET_X + (colIndex + 1) * (BRICK_WIDTH + 1) - 1,
        BRICK_START_OFFSET_Y + (rowIndex + 1) * (BRICK_HEIGHT + 1) - 1,
        RED);
}

void clearOneBrick(int colIndex, int rowIndex)
{
    TFT_Fill(
        BRICK_START_OFFSET_X + colIndex * (BRICK_WIDTH + 1),
        BRICK_START_OFFSET_Y + rowIndex * (BRICK_HEIGHT + 1),
        BRICK_START_OFFSET_X + (colIndex + 1) * (BRICK_WIDTH + 1) - 1,
        BRICK_START_OFFSET_Y + (rowIndex + 1) * (BRICK_HEIGHT + 1) - 1,
        BACKGROUND_COLOR);
}

void ClearAllBricks() {
    TFT_Fill(
        2,
        2,
        100,
        140,
        BACKGROUND_COLOR);
}

// 绘制实时砖块
void DrawLevelRtBricks()
{
    int i,j;
    for(i=0;i<NUM_ROW;i++)
    {
        for(j=0;j<NUM_BRICK_PER_ROW;j++)
        {
           if(game.level->rtBricks[i] & 1 << (7 - j))
           {
                drawOneBrick(j, i);
           }
        }
    }
}

// 一关内更新砖块，就是清除被敲击的砖块
void UpdateBreakBricks ()
{
    int rowIndex, colIndex;
    for (rowIndex = 0 ; rowIndex < NUM_ROW ; rowIndex ++)
    {
        if (game.level->dirtyBrickRowFlag[rowIndex] == 0) 
        {
            continue;
        }
        
        for (colIndex = 0 ; colIndex < NUM_BRICK_PER_ROW; colIndex++)
        {
            if (game.level->dirtyBrickRowFlag[rowIndex] & 1 << (7 - colIndex)) // && game.level.bricks[i] & colMask)
            {
                // 清除砖块 , 列数，行数
                clearOneBrick(colIndex, rowIndex);
            }
        }
        game.level->dirtyBrickRowFlag[rowIndex] = 0;
    }
}




//////////////
void handleKeyInReady() {
    if (game.key & BRK_GAME_KEYD) {
        game.status = BRK_GAME_RUN;
        DrawUI();
    }
    
}

void handleKeyInRun () {
    if (game.key & BRK_GAME_KEYD) {
        game.status = BRK_GAME_PAUSE;
        DrawUI();
    }
}

void handleKeyInPause () {
    if (game.key & BRK_GAME_KEYD) {
        game.status = BRK_GAME_RUN;
        DrawUI();
    }
}

void handleKeyInClear () {
    if (game.key & BRK_GAME_KEYD) {
        game.level->currentLevel ++;
        
        game.ball->dvx = 0.707107;
        game.ball->dvy = -0.707107;
        game.ball->dirty = 1;
        
        game.bar->x = 50;
        game.bar->y = BAR_POSY;
        game.bar->dvx = 3;
        game.bar->dirty = 1;
        
        LoadLevel();
        moveBallWithBar();
        
        ClearAllBricks();
        DrawLevelRtBricks();
        
        game.status = BRK_GAME_READY;
        
        DrawBall();
        DrawBar();
        DrawUI();
    }
}

void handleKeyInLose () {
    if (game.key & BRK_GAME_KEYD) {
        
        game.ball->dvx = 0.707107;
        game.ball->dvy = -0.707107;
        
        game.bar->x = 50;
        game.bar->y = BAR_POSY;
        game.bar->dirty = 1;

        moveBallWithBar();
        game.status = BRK_GAME_READY;
        
        DrawBall();
        DrawBar();
        DrawUI();
    }
} 

void handleKeyInFail () {
    if (game.key & BRK_GAME_KEYD) {
        game.level->currentLevel = 0;
        game.hp = 3;
        
        game.ball->dvx = 0.707107;
        game.ball->dvy = -0.707107;
        game.ball->dirty = 1;
        
        game.bar->x = 50;
        game.bar->y = BAR_POSY;
        game.bar->dvx = 3;
        game.bar->dirty = 1;
        
        LoadLevel();
        moveBallWithBar();
        
        game.status = BRK_GAME_READY;
        
        ClearAllBricks();
        
        DrawLevelRtBricks();
        DrawBall();
        DrawBar();
        DrawUI();
    }
}

void handleKeyInWin () {
    if (game.key & BRK_GAME_KEYD) {
        
        game.level->currentLevel = 0;
        game.hp = 3;
        
        game.ball->dvx = 0.707107;
        game.ball->dvy = -0.707107;
        game.ball->dirty = 1;
        
        game.bar->x = 50;
        game.bar->y = BAR_POSY;
        game.bar->dvx = 3;
        game.bar->dirty = 1;
        
        LoadLevel();
        moveBallWithBar();
        
        game.status = BRK_GAME_READY;
        
        DrawBall();
        DrawBar();
        DrawUI();
    }
}
//////////////

void refresDisplay() 
{
    DrawBall();
    DrawBar();
    DrawWall();
    UpdateBreakBricks();
}


void LoadLevel()
{
    int i;
    game.level->bricks = LEVEL_BRICKS[game.level->currentLevel];
    
    for(i = 0 ; i < NUM_ROW; i++)
    {
        game.level->dirtyBrickRowFlag[i] = 0;
        game.level->rtBricks[i] = game.level->bricks[i];
    }
}


void InitData()
{
    game.level = &mLevel;
    game.ball = &mBall;
    game.bar = &mBar;
    game.hp = 3;
    game.status = BRK_GAME_READY;
    
    game.level->currentLevel = 0;
    game.level->dirtyBrickRowFlag = mDirtyFlag;
    game.level->rtBricks = mRtBricks;
    
    game.ball->dvx = 0.707107;
    game.ball->dvy = -0.707107;
    game.ball->dirty = 1;
    
    game.bar->x = 50;
    game.bar->lastDisplayX = 50;
    game.bar->y = BAR_POSY;
    game.bar->dvx = 3;
    game.bar->lastDisplayY = BAR_POSY;
    game.bar->dirty = 1;
    
    game.speedMode = SPEED_MODE_1X;
    
    LoadLevel();
    moveBallWithBar();
}

void InitGame(void)
{
    //	初始化滴答定时器
    systick_config();
    
    //初始化LED引脚
    Init_LED_GPIO();
    
    //设置竖屏
    USE_HORIZONTAL = 0
    ;
    //初始化TFT屏幕引脚及默认配置
    TFT_Init();
    
    //TFT 用背景色清屏
    TFT_Fill(0,0,128,160, BACKGROUND_COLOR);
    
    //初始化EC11引脚
    Init_EC11_GPIO();
    Init_EC11_EXITGPIO();
    //初始化按键123引脚
    Init_Key_GPIO();
    
    Set_EC11_Rotate_Handler(OnEc11RotateBrick);
    Set_EC11_Click_Handler(OnEc11ClickBrick);
    Set_Key1_Click_Handler(OnKey1ClickBrick);
    
    Close_LED(1);    
    Close_LED(2);
    
    InitData();
    
    DrawWall();
    DrawBar();
    
    DrawLevelRtBricks();
    DrawUI();
}

void RunGame(void)
{   
    double deltaTime = 0.5;
    double leftTime = 0.0;
    double rate;
    Point hitPoint;
    int isBallComputed = 0;
    
    while(1)
    {
        deltaTime = 0.5 * SPEED_MODOE_VAL[game.speedMode];
        
        if (game.status == BRK_GAME_READY) {
            handleKeyInReady();
            computeBarMotion(deltaTime);
            moveBallWithBar();
        } else if (game.status == BRK_GAME_RUN) {
            handleKeyInRun();

            // compute bar motion first
            computeBarMotion(deltaTime);

            isBallComputed = computeBallMotion(deltaTime, &leftTime);
            while (!isBallComputed) 
            {
                isBallComputed = computeBallMotion(leftTime, &leftTime);
            }

            if (game.isDead) {
                game.isDead = 0;
                game.hp = game.hp - 1;

                if (game.hp > 0) {
                    game.status = BRK_GAME_LOSE;
                } else {
                    game.status = BRK_GAME_FAIL;
                }
                DrawUI();
            }
            
            
            // 判断本关是否完成
            {
                int hasRemainBrick = 0;
                int i;
                for ( i = 0 ; i < NUM_ROW; i++)
                {
                    hasRemainBrick += game.level->rtBricks[i];
                }
                if (hasRemainBrick == 0)
                {
                    refresDisplay();
                    if(game.level->currentLevel == NUM_LEVEL - 1)
                    {
                        game.status = BRK_GAME_WIN; 
                    }
                    else
                    {
                        game.status = BRK_GAME_CLEAR;
                    }
                    DrawUI();
                
                }
            }
            
            //
        } else if (game.status == BRK_GAME_PAUSE) {
            handleKeyInPause();
        } else if (game.status == BRK_GAME_CLEAR) {
            handleKeyInClear();
        } else if (game.status == BRK_GAME_LOSE) {
            handleKeyInLose();
        } else if (game.status == BRK_GAME_FAIL) {
            handleKeyInFail();
        } else if (game.status == BRK_GAME_WIN) {
            handleKeyInWin();

        }
        

        //let completeTime = getSysCurTimeMS()

        gameResetKey();
        refresDisplay();

//        let frameEndTimeMS = getSysCurTimeMS()
//        let frameRemainTimeMS = Math.max(FRAME_TIME_LENGTH_MS - (frameEndTimeMS - frameStartTimeMS), 1)
//        await delay_ms(frameRemainTimeMS)
        
        delay_1ms(15);
        
        
    }
}