#pragma once

#include <stdbool.h>
#include <stdint.h>

// 前置声明
typedef struct FSM FSM;
typedef struct State State;
typedef struct Transition Transition;

// 状态机上下文数据，由用户定义
typedef void *FSMContext;

// 状态回调函数类型定义
// 进入状态时调用
typedef void (*StateEnterFunc)(FSM *fsm, FSMContext context);
// 状态更新时调用
typedef void (*StateUpdateFunc)(FSM *fsm, FSMContext context, float deltaTime);
// 退出状态时调用
typedef void (*StateExitFunc)(FSM *fsm, FSMContext context);

// 转换条件函数类型，返回true则执行转换
typedef bool (*TransitionConditionFunc)(FSM *fsm, FSMContext context);

// 状态转换结构
struct Transition {
  const State *targetState;          // 目标状态
  TransitionConditionFunc condition; // 转换条件
};

// 状态结构
struct State {
  const char *name;              // 状态名称，用于调试
  StateEnterFunc enter;          // 进入状态回调
  StateUpdateFunc update;        // 更新状态回调
  StateExitFunc exit;            // 退出状态回调
  const Transition *transitions; // 状态转换数组
  uint8_t transitionCount;       // 转换数量
};

// 有限状态机结构
struct FSM {
  const State *currentState;  // 当前状态
  const State *previousState; // 上一个状态
  FSMContext context;         // 上下文数据
  bool isRunning;             // 状态机是否运行
};

// 初始化状态机
void FSM_Init(FSM *fsm, const State *initialState, FSMContext context);

// 更新状态机
void FSM_Update(FSM *fsm, float deltaTime);

// 强制状态机转换到指定状态
void FSM_ChangeState(FSM *fsm, const State *newState);

// 获取当前状态
const State *FSM_GetCurrentState(const FSM *fsm);

// 获取上一个状态
const State *FSM_GetPreviousState(const FSM *fsm);

// 获取当前状态名称
const char *FSM_GetCurrentStateName(const FSM *fsm);

// 启动状态机
void FSM_Start(FSM *fsm);

// 停止状态机
void FSM_Stop(FSM *fsm);

// 检查状态机是否在运行
bool FSM_IsRunning(const FSM *fsm);
