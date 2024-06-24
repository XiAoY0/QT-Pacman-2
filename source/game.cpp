#include "game.h"
#include <QString>
#include <QFile>
#include <QRandomGenerator>
#define W (GameObject::Width)


int GHOST_RELEASE_TIME[] = {0, 200, 400, 600}; // 幽灵出笼时间间隔

Game::Game(int x, int y, int map_w, int map_h, QString map_src)
    : QGraphicsScene(x, y, W * map_w, W * map_h) // 初始化QGraphicsScene
{
    geo_x = x;
    geo_y = y;
    stat = Playing; // 设置游戏状态为Playing

    // 初始化地图
    map_size = map_w * map_h; // 设置地图大小
    map_width = map_w; // 设置地图宽度
    map_height = map_h; // 设置地图高度
    map = new GameObject**[map_height]; // 创建二维指针数组 初始化地图
    for (int i = 0; i < map_height; i++) {
        map[i] = new GameObject*[map_width];
        for (int j = 0; j < map_width; j++)
            map[i][j] = nullptr;
    }

    // 初始化地图元素
    ball_num = eat_num = score = 0; // 初始化豆子数量、吃掉数量和分数
    int ghost_i = 0; // 初始化幽灵索引
    QPixmap wallpix(":/game_objects/map_objects/wall.png");
    QPixmap ballpix(":/game_objects/map_objects/dot.png");
    QPixmap powerballpix(":/game_objects/map_objects/power_ball.png");
    QPixmap gatepix(":/game_objects/map_objects/gate.png");
    // QPixmap wallpix(":/icon/images/wall.png"); // 墙壁图片
    // QPixmap ballpix(":/icon/images/dot.png"); // 豆子图片
    // QPixmap powerballpix(":/icon/images/power_ball.png"); //大力丸图片
    // QPixmap gatepix(":/icon/images/gate.png"); // 门图片
    QPixmap blankpix; // 空白图片
    QFile mapfile(map_src); // 打开地图文件
    mapfile.open(QIODevice::ReadOnly|QIODevice::Text); // 以只读和文本模式打开文件

    pacman = new Pacman(); // 创建Pacman对象

    // 遍历地图
    for (int i = 0; i < map_h; i++) {
        QByteArray line = mapfile.readLine();
        for (int j = 0; j < map_w; j++) {
            int tmp_x = x + (j * W); // 计算x坐标
            int tmp_y = y + (i * W); // 计算y坐标
            switch (line[j]) { // 根据地图文件中的字符创建相应的游戏对象
            case '1':
                map[i][j] = new GameObject(GameObject::Wall, wallpix); // 创建墙壁对象
                map[i][j]->setPos(tmp_x, tmp_y); // 设置位置

                // 添加到场景 调用的是QGraphicsScene::addItem()
                addItem(map[i][j]);
                break;
            case '0':
                map[i][j] = new GameObject(GameObject::Ball, ballpix); // 创建豆子对象
                map[i][j]->set_score(BALL_SCORE); // 设置豆子分数
                map[i][j]->setPos(tmp_x, tmp_y); // 设置位置
                addItem(map[i][j]);
                ball_num++; // 增加豆子总数
                break;
            case '4':
                map[i][j] = new GameObject(GameObject::PowerBall, powerballpix); // 创建能量豆对象
                map[i][j]->set_score(POWERBALL_SCORE); // 设置分数
                map[i][j]->setPos(tmp_x, tmp_y);
                addItem(map[i][j]); // 添加到场景
                powerball.push_back(map[i][j]); // 添加到大力丸列表
                ball_num++;
                break;
            case '3':
                map[i][j] = new GameObject(GameObject::Blank, blankpix); // 创建空白对象
                break;
            case '2':
                gate = new GameObject(GameObject::Gate, gatepix); // 创建门对象
                gate->_x = j; // 设置x坐标
                gate->_y = i; // 设置y坐标
                gate->setPos(tmp_x, tmp_y);
                addItem(gate);

                //很关键的一步 这个位置后面会用到判断函数里
                map[i][j] = gate;
                break;
            case 'p':
                pacman = new Pacman(); // 创建Pacman对象
                pacman->game = this; // 设置游戏指针

                // 设置z值 决定item的堆叠顺序 上面的item默认Z值为0
                pacman->setZValue(2);
                pacman->setPos(tmp_x, tmp_y);
                addItem(pacman);
                map[i][j] = pacman;
                break;
            //ghost的item添加
            case 'g':
                map[i][j] = new GameObject(GameObject::Blank, blankpix); // 创建空白对象
                ghost[ghost_i] = new Ghost(ghost_i); // 创建幽灵对象
                ghost[ghost_i]->game = this; // 设置游戏指针
                ghost[ghost_i]->setZValue(2); // 设置z值

                // 设置出笼时间
                ghost[ghost_i]->release_time = GHOST_RELEASE_TIME[ghost_i];
                ghost[ghost_i]->_x = j; // 设置x坐标
                ghost[ghost_i]->_y = i; // 设置y坐标
                ghost[ghost_i]->set_score(GHOST_SCORE); // 设置幽灵分数
                ghost[ghost_i]->setPos(tmp_x, tmp_y); // 设置位置
                addItem(ghost[ghost_i]);

                // 增加幽灵索引 总共四只幽灵
                ghost_i++;
                break;
            }
            if (map[i][j]) { // 如果地图中有对象
                map[i][j]->_x = j; // 设置x坐标
                map[i][j]->_y = i; // 设置y坐标
            }
        }
    }

    //策略函数的实现在chasing_strategy.cpp文件中
    ghost[Ghost::Red]->chase_strategy = &strategy1; // 设置红色幽灵的追逐策略
    ghost[Ghost::Pink]->chase_strategy = &strategy2; // 设置粉色
    ghost[Ghost::Green]->chase_strategy = &strategy3; // 设置绿色
    ghost[Ghost::Yellow]->chase_strategy = &strategy4; // 设置黄色
}

void Game::start()
{
    // 创建大力丸闪烁定时器
    powerball_flash_timer = new QTimer(this);

    // 连接定时器超时信号到能量豆闪烁槽函数
    connect(powerball_flash_timer, SIGNAL(timeout()), this , SLOT(powerball_flash()));
    powerball_flash_timer->start(FLASH_INTERVAL); // 启动定时器

    // 创建Pacman定时器
    pacman_timer = new QTimer(this);
    connect(pacman_timer, SIGNAL(timeout()), this , SLOT(pacman_handler()));
    pacman_timer->start(INTERVAL); // 启动定时器

    // 遍历所有幽灵
    for (int i = 0; i < Ghost::GhostNum; i++) {
        ghost_timer[i] = new QTimer(this);
        // 连接同时传参 ghost-id
        connect(ghost_timer[i], &QTimer::timeout, [=](){ghost_handler(i);} );
        ghost_timer[i]->start(NORMAL_INTERVAL);
    }
}

void Game::stop()
{
    pacman_timer->stop(); // 停止Pacman定时器
    powerball_flash_timer->stop(); // 停止能量豆闪烁定时器
    for (int i = 0; i < Ghost::GhostNum; i++) { // 遍历所有幽灵
        ghost_timer[i]->stop(); // 停止幽灵定时器
    }
}

//所有大力丸已经被吃掉 隐藏相关资源
//没有则显示
void Game::powerball_flash()
{
    if (powerball.empty()) {
        powerball_flash_timer->stop();
        return;
    }

    if (flash_tick) {
        for (int i = 0; i < powerball.size(); i++) {
            powerball.at(i)->hide();
        }
        flash_tick = 0;
    } else {
        for (int i = 0; i < powerball.size(); i++) { // 遍历所有能量豆
            powerball.at(i)->show(); // 显示能量豆
        }
        flash_tick = 1; // 设置闪烁标志
    }
}

void Game::pacman_handler()
{
    pacman->move();
    // 如果游戏状态为Win 停止游戏
    if (stat == Win) {
        stop();
    }
}

//多一个ghost id 指明哪个ghost
void Game::ghost_handler(int ghost_id)
{
    ghost[ghost_id]->move();
    // 如果游戏状态为Lose  停止游戏
    if (stat == Lose) {
        stop();
    }
}

// 设置Pacman的下一个方向
void Game::pacman_next_direction(GameObject::Dir d)
{
    pacman->set_next_dir(d);
}

int Game::get_score()
{
    return score;
}

Game::~Game()
{
    for (int i = 0; i < map_height; i++) { // 遍历地图高度
        for (int j = 0; j < map_width; j++) { // 遍历地图宽度
            if (map[i][j] != nullptr) // 如果地图中有对象
                delete map[i][j]; // 删除对象
        }
        delete[] map[i]; // 删除行指针数组
    }
    delete[] map; // 删除地图指针数组

    // 删除Pacman定时器
    delete pacman_timer;
    delete powerball_flash_timer;

    // 删除幽灵定时器
    for (int i = 0; i < Ghost::GhostNum; i++) {
        delete ghost_timer[i];
    }
}
