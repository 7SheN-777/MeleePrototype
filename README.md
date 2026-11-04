# MeleePrototype

基于 **Unreal Engine 5.8.3** 和 **C++** 开发的第三人称近战动作游戏原型。

本项目以学习 Unreal Engine Gameplay Framework、C++ 游戏架构和近战战斗系统为主要目标，通过实际开发逐步实现角色控制、动画驱动攻击、碰撞检测、伤害处理及基础 UI 等功能。


## 有一个重大问题是关于美术资源和动画资源的缺失，我目前无法做出MotionMatching一样流畅的动画和动作，各位大神有招的话给我支点招吧，想完成一个项目，喵！！！


## 项目介绍

MeleePrototype 是一个以第三人称近战战斗为核心的个人学习项目。

项目采用 C++ 与 Unreal Engine 蓝图相结合的开发方式，将角色控制、战斗逻辑、动画表现和 UI 进行模块化组织。

开发过程中注重对 Unreal Engine 底层机制和 Gameplay Framework 的理解，尽量将具体功能封装为可复用的组件，为后续扩展战斗系统和游戏玩法打下基础。

## 技术栈

| 技术 | 用途 |
|---|---|
| Unreal Engine 5.8.3 | 游戏引擎 |
| C++ | 核心 gameplay 逻辑 |
| Blueprint | UI、动画及部分功能配置 |
| Enhanced Input | 输入系统 |
| Animation Montage | 攻击及受击动画 |
| Animation Notify | 动画事件与战斗逻辑通信 |
| Collision / Trace | 近战攻击检测 |
| Git / GitHub | 版本管理 |

## 已实现功能

### 1. 角色控制系统

- 基于 Unreal Engine Character 类实现玩家角色。
- 使用 Enhanced Input 处理角色输入。
- 实现角色移动、跳跃及视角控制。
- 通过 C++ 管理角色基础行为。

### 2. 近战攻击系统

- 使用 Animation Montage 驱动角色攻击动作。
- 通过动画通知状态（AnimNotifyState）控制攻击检测时机。
- 使用 Sphere Trace 实现近战攻击范围检测。
- 通过 HitActors 集合记录单次攻击中已经命中的目标，避免重复造成伤害。
- 使用 Montage End Delegate 监听攻击动画结束事件。

### 3. 战斗组件化设计

创建独立的 `UMeleeCombatComponent`，将近战攻击相关逻辑从角色类中分离。

主要职责：

- 执行近战攻击检测。
- 管理攻击配置参数。
- 记录单次攻击的命中对象。
- 控制攻击开始及命中记录重置。

通过组件化设计提高战斗逻辑的复用性，降低角色类与具体攻击实现之间的耦合。

### 4. 伤害与敌人系统

- 创建独立的 C++ 敌人 Actor。
- 实现基础生命值管理。
- 使用 Unreal Engine Damage System 处理伤害传递。
- 通过 `ApplyDamage` 传递攻击伤害。
- 通过 `TakeDamage` 接收并处理伤害。
- 实现敌人受击动画及死亡逻辑。
- 使用生命值状态控制受击和死亡分支。

### 5. 主菜单与 UI

- 创建独立主菜单关卡。
- 使用 UMG 构建主菜单界面。
- 实现开始游戏、设置、制作人员及退出等菜单入口。
- 使用水墨风格作为 UI 的主要视觉设计方向。

## 项目结构

```text
MeleePrototype/
├── Config/
├── Content/
│   ├── Characters/
│   ├── Animations/
│   ├── UI/
│   ├── Maps/
│   └── ...
├── Source/
│   └── MeleePrototype/
│       ├── Character/
│       ├── Combat/
│       ├── ...
│       └── MeleePrototype.Build.cs
└── README.md
```

> 注：以上为功能模块的逻辑组织示意，实际目录以项目源码为准。

## 开发环境

- Unreal Engine：5.8.3
- IDE：Visual Studio
- Programming Language：C++
- Platform：Windows

## 后续开发计划

- [ ] 完善角色战斗状态管理。
- [ ] 扩展攻击类型及连击系统。
- [ ] 完善武器装备与攻击配置。
- [ ] 扩展敌人 AI 与战斗行为。
- [ ] 完善角色动画状态机。
- [ ] 优化战斗组件的通用性与可扩展性。
- [ ] 完善主菜单及游戏流程。
- [ ] 增加更多战斗场景与交互内容。

## 项目目标

本项目不仅是一个动作游戏原型，也是一个持续学习 Unreal Engine C++ 开发的实践项目。

希望通过项目开发深入理解：

- Unreal Engine Gameplay Framework
- C++ 面向对象设计与组件化架构
- 动画系统与 Gameplay 逻辑的交互
- 碰撞检测与近战战斗实现
- Unreal Engine Damage System
- 委托、事件及模块间通信机制

---

**Project:** MeleePrototype  
**Engine:** Unreal Engine 5.8.3  
**Language:** C++ / Blueprint  
**Status:** In Development
