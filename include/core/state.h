#pragma once

#include <stdbool.h>

// 游戏状态枚举
typedef enum {
  GAME_STATE_MENU,
  GAME_STATE_PLAYING,
  GAME_STATE_PAUSED,
  GAME_STATE_GAME_OVER
} GameState;

// 游戏方向枚举
typedef enum {
  DIRECTION_UP,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
  DIRECTION_RIGHT
} Direction;

// 游戏配置结构体
typedef struct {
  int gridWidth;          // 网格宽度
  int gridHeight;         // 网格高度
  int gridSize;           // 网格单元大小
  int initialSnakeLength; // 初始蛇长度
  int maxFoodCount;       // 最大食物数量
  float moveInterval;     // 移动间隔（秒）
} GameConfig;

// 游戏状态结构体
typedef struct {
  GameState currentState;     // 当前游戏状态
  GameConfig config;          // 游戏配置
  int score;                  // 当前得分
  float timeSinceLastMove;    // 上次移动后的时间
  bool directionChanged;      // 方向是否已改变
  Direction currentDirection; // 当前移动方向
  Direction nextDirection;    // 下一个移动方向
} GameStateData;

/**
 * @brief 初始化游戏状态
 * @param state 游戏状态指针
 * @param config 游戏配置
 */
void init_game_state(GameStateData *state, const GameConfig *config);

/**
 * @brief 更新游戏状态
 * @param state 游戏状态指针
 * @param deltaTime 时间增量（秒）
 * @return 如果蛇需要移动，返回true；否则返回false
 */
bool update_game_state(GameStateData *state, float deltaTime);

/**
 * @brief 改变蛇的移动方向
 * @param state 游戏状态指针
 * @param direction 新的方向
 */
void change_direction(GameStateData *state, Direction direction);

/**
 * @brief 开始游戏
 * @param state 游戏状态指针
 */
void start_game(GameStateData *state);

/**
 * @brief 暂停游戏
 * @param state 游戏状态指针
 */
void pause_game(GameStateData *state);

/**
 * @brief 恢复游戏
 * @param state 游戏状态指针
 */
void resume_game(GameStateData *state);

/**
 * @brief 游戏结束
 * @param state 游戏状态指针
 */
void game_over(GameStateData *state);

/**
 * @brief 重置游戏
 * @param state 游戏状态指针
 */
void reset_game(GameStateData *state);