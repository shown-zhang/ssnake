#include "core/state.h"
#include <SDL3/SDL.h>

void init_game_state(GameStateData* state, const GameConfig* config) {
    if (state == NULL || config == NULL) {
        return;
    }
    
    state->currentState = GAME_STATE_MENU;
    state->config = *config;
    state->score = 0;
    state->timeSinceLastMove = 0.0f;
    state->directionChanged = false;
    state->currentDirection = DIRECTION_RIGHT;
    state->nextDirection = DIRECTION_RIGHT;
}

bool update_game_state(GameStateData* state, float deltaTime) {
    if (state == NULL || state->currentState != GAME_STATE_PLAYING) {
        return false;
    }
    
    // 更新时间计数器
    state->timeSinceLastMove += deltaTime;
    
    // 检查是否到达移动间隔
    if (state->timeSinceLastMove >= state->config.moveInterval) {
        // 重置时间计数器
        state->timeSinceLastMove = 0.0f;
        
        // 应用方向改变
        if (state->directionChanged) {
            state->currentDirection = state->nextDirection;
            state->directionChanged = false;
        }
        
        // 返回true表示需要移动蛇
        return true;
    }
    
    return false;
}

void change_direction(GameStateData* state, Direction direction) {
    if (state == NULL || state->currentState != GAME_STATE_PLAYING) {
        return;
    }
    
    // 防止反向移动（例如：当前向右时不能立即向左）
    if ((state->currentDirection == DIRECTION_UP && direction == DIRECTION_DOWN) ||
        (state->currentDirection == DIRECTION_DOWN && direction == DIRECTION_UP) ||
        (state->currentDirection == DIRECTION_LEFT && direction == DIRECTION_RIGHT) ||
        (state->currentDirection == DIRECTION_RIGHT && direction == DIRECTION_LEFT)) {
        return;
    }
    
    state->nextDirection = direction;
    state->directionChanged = true;
}

void start_game(GameStateData* state) {
    if (state == NULL) {
        return;
    }
    
    state->currentState = GAME_STATE_PLAYING;
    state->score = 0;
    state->timeSinceLastMove = 0.0f;
    state->currentDirection = DIRECTION_RIGHT;
    state->nextDirection = DIRECTION_RIGHT;
    state->directionChanged = false;
    
    SDL_Log("游戏开始！");
}

void pause_game(GameStateData* state) {
    if (state == NULL || state->currentState != GAME_STATE_PLAYING) {
        return;
    }
    
    state->currentState = GAME_STATE_PAUSED;
    SDL_Log("游戏暂停");
}

void resume_game(GameStateData* state) {
    if (state == NULL || state->currentState != GAME_STATE_PAUSED) {
        return;
    }
    
    state->currentState = GAME_STATE_PLAYING;
    SDL_Log("游戏继续");
}

void game_over(GameStateData* state) {
    if (state == NULL) {
        return;
    }
    
    state->currentState = GAME_STATE_GAME_OVER;
    SDL_Log("游戏结束！最终得分: %d", state->score);
}

void reset_game(GameStateData* state) {
    if (state == NULL) {
        return;
    }
    
    state->currentState = GAME_STATE_MENU;
    state->score = 0;
    state->timeSinceLastMove = 0.0f;
    state->currentDirection = DIRECTION_RIGHT;
    state->nextDirection = DIRECTION_RIGHT;
    state->directionChanged = false;
    
    SDL_Log("游戏重置");
}