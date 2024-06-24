#ifndef GAME_H
#define GAME_H


#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <QPair>
#include "gameobject.h"

/* 可变游戏选项 */
#define BALL_SCORE      10          //小球10分
#define POWERBALL_SCORE 30          //大力丸30分
#define GHOST_SCORE     50          //吃一个鬼魂50分
#define INTERVAL        10          //吃豆人移动间隔
#define NORMAL_INTERVAL 10          //normal ghosts 移动间隔
#define PANNIC_INTERVAL 15          //pannic ghosts 移动间隔
#define RUNNING_INTERVAL 5          //running ghosts 移动间隔
#define PANNIC_TIME     1000        // pannic持续时间
#define FLASH_INTERVAL 200          // 大力丸闪光时间间隔


class Game : public QGraphicsScene
{
    Q_OBJECT
public:
    //游戏状态枚举
    enum GameStatus {Playing , Win , Lose , Pause};

    //构造函数 实现在cpp中 位置 宽和高 地图路径
    Game(int,int,int,int,QString);

    ~Game();

    //游戏开始
    void start();

    //游戏暂停
    void stop();

    //Panman的下一个移动方向
    void pacman_next_direction(GameObject::Dir);

    //获取当前分数
    int get_score();

    //游戏地图
    GameObject ***map;

    //ghost笼子的门
    GameObject *gate;

    //Pacman
    Pacman *pacman;

    //Ghost数组对象
    Ghost *ghost[Ghost::GhostNum];

    //大力丸数组对象
    QVector<GameObject*>powerball;

    //当前游戏状态
    GameStatus stat;

    //Pacman类友元声明 Pacman 类可以访问 Game 类的私有和保护成员
    friend class Pacman;

    //Ghost类友元声明 Ghost 类可以访问 Game 类的私有和保护成员
    friend class Ghost;

private slots:
    //pacman移动处理
    void pacman_handler();

    //大力丸闪烁
    void powerball_flash();

    //ghost移动
    void ghost_handler(int);

private:
    //地图高度和宽度
    int map_height,map_width;

    //地图大小 长乘宽
    int map_size;

    //豆子总数量 、 吃掉的豆子数量 、 当前分数
    int ball_num,eat_num,score;

    //坐标
    int geo_x,geo_y;

    //ghost计时器 每个ghost有其单独计时器
    QTimer *ghost_timer[Ghost::GhostNum];

    //pacman的计时器
    QTimer *pacman_timer;

    //大力丸闪烁计时器
    QTimer *powerball_flash_timer;

    //是否闪烁
    bool flash_tick;
};

#endif // GAME_H
