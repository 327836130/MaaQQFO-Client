# MaaQQFO 总体架构设计方案（V1.0）

> **文档性质：** 总体设计讨论留档 / 后续项目实施基线  
> **形成日期：** 2026 年 11 月 18 日  
> **阶段状态：** 总体方向已基本审定；尚未创建新项目、Fork 仓库、删除旧仓库或启动迁移开发  
> **本地项目根目录：** `F:\各类脚本\2025.12.08-qq幻想脚本开发\22.MaaQQFO`  
> **新项目名称：** `MaaQQFO`  
> **总体目标：** 基于新版稳定 Maa 生态，将既有技术研究成果持续集成到统一项目中，形成以 Maa Pipeline 为策略中心、以自定义 Controller 为操作底座、以统一游戏状态读取为数据底座、以可持续扩展的 C++ Custom Recognition / Custom Action 为能力模块的 QQ幻想自动化平台。

---

## 一、项目定位与总体原则

MaaQQFO 不是另起炉灶编写一套游戏自动化调度器，也不是把现有脚本简单打包成 GUI。项目以 **MaaFramework 作为唯一的任务编排与执行中心**，在最新稳定的客户端生态之上建立 QQ幻想专属功能体系。

### 1.1 已确定的架构原则

1. **Maa 为核心。** Pipeline 决定识别、条件分支、动作顺序、异常处理、停止与任务组合。不得在 Agent 内再建立与 Maa 并行的完整任务调度系统。
2. **一个统一 C++ Agent 程序作为模块入口。** Agent 内部按功能划分模块，统一注册 Custom Recognition 和 Custom Action；不为每个新增功能创建独立 Agent 项目。
3. **尊重 Maa 自身的实例隔离。** 每个 Maa 游戏实例持有其 Controller / Tasker 等运行上下文；不人为规定“每个角色必须独立启动一个 Agent 进程”。实际 Agent 进程组织方式以所选客户端与框架的运行机制为准。
4. **游戏状态按游戏进程隔离。** GameStateReader 绑定对应 `qqfo.exe` 进程与有效会话；同一进程内的多个模块共享状态数据，不同游戏进程之间不共享可变游戏状态。
5. **Controller 统一操作游戏。** Maa 原生 Click、按键、截图等操作通过 QQ幻想专属 Controller；Proto Input 为已实机验证成功的后台鼠标输入底层，不保留 DM 和 Proto Input 两套并行的默认点击实现。
6. **读取与操作分离。** GameStateReader 负责只读获取/整理人物、地图和场景对象状态；Custom Action 通过 Controller 执行操作；策略仍由 Pipeline 决定。
7. **继承成果而不继承历史包袱。** 新项目以官方最新稳定代码为基线，旧项目中的有效设计、源码、数据、测试和资源逐项迁入；不直接把旧客户端整体当作新仓库起点。
8. **所有开发工具遵守同一坐标规范。** 正式运行、编辑器、测试页面采用一致的截图、缩放、DPI 与点击坐标定义。
9. **持续迭代与 GitHub 归档。** 采用上游可追踪的 Fork、版本锁定、代码审查、回归测试、分阶段集成，长期新增可复用的 Custom Action 与 Recognition。

### 1.2 目标使用体验

用户启动 MaaQQFO 后，看到的是已识别的**游戏角色列表**，而非难懂的窗口句柄列表；选择角色和对应任务策略，即可由 Maa Pipeline 组织现有能力完成识别、寻路、交互、战斗、拾取、Buff、钓鱼等操作。

- 用户在界面配置 Controller、截图分辨率、任务参数和具体角色。
- Pipeline 使用 Maa 原生识图/OCR/Click 节点及项目自定义 Recognition/Action。
- 新增功能时主要扩展 Agent 模块与 Pipeline 配置，而不是修改底层调度架构。

---

## 二、技术基线与 GitHub 策略

### 2.1 版本选型（截至 2026-11-18 核查）

| 组件 | 当前选定基线 | 在项目中的定位 | 注意事项 |
|---|---|---|---|
| [MFAAvalonia](https://github.com/MaaXYZ/MFAAvalonia) | **v2.16.4 稳定版** | 长期维护的客户端 Fork 主体 | 以稳定标签建基线，不直接以可能变化的 `master/main` HEAD 代表稳定版 |
| [MaaFramework](https://github.com/MaaXYZ/MaaFramework) | **v5.16.1 稳定版（目标）** | Pipeline/识别/控制器/Agent 核心引擎 | 最终仍须核对 MFAAvalonia 依赖与 Agent 协议兼容，不得硬性混装 |
| [MaaPipelineEditor](https://github.com/kqcoxn/MaaPipelineEditor) | **v2.1.3 稳定版** | 编辑 Pipeline、调试识别与 Agent | 先以独立工具接入；确有必要修改编辑器源码时再单独 Fork |
| Proto Input | P8.3 所用固定提交 `dd29bf8d22349605d0e1950c1c5dc4893d72670e` | QQ幻想后台输入核心技术基线 | 保留修改补丁、EasyHook 版本、编译脚本、实验报告、许可证 |

参考：2026-11-18 还存在 MaaFramework **v6.0.0-beta.3**、MFAAvalonia **v2.16.6-beta.1** 等预发布版；**不作为首期主线**，可在单独兼容分支研究。6.0 升级涉及 Agent 协议变化，主程序/Agent/运行时必须共同验证。

### 2.2 本地路径与 GitHub 仓库

```text
F:\各类脚本\2025.12.08-qq幻想脚本开发\22.MaaQQFO
```

GitHub 新主仓库拟命名：**`MaaQQFO`**。

- 对可能已经存在的旧 `MaaQQFO` / `MaaQQFantasy` 仓库，实施前先检查实际名称、所有者、访问权限与现有内容。
- 用户倾向**旧项目删除后重新建设**；但执行删除前必须备份旧仓库中有价值的资料并对目标仓库作最终确认。当前阶段**没有进行任何删除操作**。
- 新主线以 MFAAvalonia 稳定版 Fork 为起点，保留 `upstream` 关系；所有 QQ幻想功能以可辨认的独立目录、扩展层和提交记录承载，尽量减少修改上游核心逻辑。
- 上游稳定版本的同步在独立分支完成兼容性检查，再合入 MaaQQFO 主线；不得直接在运行中的主线覆盖新版本 DLL。
- GitHub Actions 负责可自动化的静态检查、构建、单元测试与打包；游戏实机、后台输入和窗口生命周期测试保留 Windows 本地验收流程。

### 2.3 历史成果迁移原则

| 既有成果 | 处理方式 |
|---|---|
| 旧 Maa Pipeline、图片模板、资源和配置 | 清点后迁移，按新版接口兼容性调整，不原样盲拷 |
| P1/P3 / D1 中已研究的 Controller 架构、x64/x86 边界、IPC、DPI 与 FramePool | **优先继承经证据验证的设计与代码**，避免重复开发 |
| 旧 DM 输入适配 | 保留历史代码与实验记录作参考；新 Controller 默认改用已验证的 Proto Input 链路 |
| P8.2/P8.3 Proto Input 实机实验 | 固定为后台定点点击的验收基线，纳入项目版本和回归测试 |
| 人物/地图/场景对象研究以及 CE 原始快照 | 整理为内存读取适配层和字段证据表，逐项转成可复用读取 API |
| 旧测试程序、调试页面、日志格式 | 提取有效功能，统一纳入新测试平台，避免以后每个 Action 单独造测试工具 |

旧工程的具体实现、哪些部分曾完整验收，仍须以实存源码和测试证据核对；总体方案不把未检查的历史实现直接认定为已经可用。

---

## 三、总体技术架构

```text
MaaQQFO（Fork MFAAvalonia 最新稳定客户端）
│
├── 用户配置与角色选择
│   ├── 按角色身份展示窗口 / Controller 绑定
│   ├── 截图分辨率、DPI、运行配置
│   └── 多实例任务、日志、开发测试入口
│
├── MaaFramework（按兼容版本锁定）
│   ├── Maa Tasker / Resource / Controller 实例
│   ├── Pipeline：识别、动作、分支、超时、停止
│   └── Maa 原生 TemplateMatch、OCR、Click 等功能
│
├── QQFOController（Custom Controller）
│   ├── 窗口/角色会话绑定、连接与断开
│   ├── 截图（沿用有效的 FramePool 路线）
│   ├── Maa 统一坐标转换
│   └── Proto Input 后台输入桥接（x64 主端 / x86 目标端）
│
└── QQFOAgent（唯一 C++ Agent 程序入口）
    ├── Recognition：人物、地图、NPC、怪物、物品、状态判定
    ├── Actions：寻路、NPC 交互、拾取、战斗、Buff、钓鱼……
    ├── GameStateReader：当前进程的只读数据访问
    ├── SnapshotCache：当前进程的状态快照缓存
    └── Session/Target Context：目标引用、有效性和取消状态
         │
         └── 仅绑定当前 qqfo.exe 进程及对应游戏会话
```

图为**逻辑模块关系**，不要求所有框图对应独立 OS 进程。是否独立启动 Agent、Agent 的客户端连接方式，遵循当期 MaaFramework/MFAAvalonia 的实现；不人为另建多实例调度器。

### 3.1 核心职责界限

- **Maa Pipeline：** 决定何时执行、选择何种策略、执行顺序和分支、错误处理及任务结束。
- **QQFOController：** 执行标准输入/截图命令，维护窗口与连接生命周期；不直接负责打怪或寻路策略。
- **QQFOAgent：** 集中注册业务识别器和动作，提供可组合能力；允许 Action 内部为完成单个原子业务而进行有限状态循环，但不能取代 Pipeline 作为总体调度中心。
- **GameStateReader：** 只读观察并标准化游戏状态，不负责直接点击或控制人物。

---

## 四、游戏角色选择与窗口绑定

### 4.1 由 HWND 选择升级为按角色选择

用户侧展示：**角色名称 / 人物 ID（如已确认） / 地图 / 连接状态**；系统内部仍保留 PID、HWND、线程、DPI、客户区等技术信息。

建议绑定流程：

1. 枚举所有目标 `qqfo.exe` 游戏进程与窗口，校验窗口确属对应进程。
2. 尝试从每个进程读取已验证的人物身份字段、名称、地图与位置。
3. 在 MaaQQFO 中显示角色候选列表，用户按角色选择。
4. 创建或绑定该角色对应的 Maa Controller / Tasker 会话。
5. 游戏退出、角色重新登录或人物身份发生变化时，立即重新验证绑定，清除旧缓存和旧目标引用。

### 4.2 需要保留的条件与边界

- 历史实验已经定位过**本地角色对象和坐标**，但人物**永久稳定 ID 的内存读取与跨重启稳定性尚不能直接视为已验证**。此项作为后续专项核对。
- 场景对象的 **query_key / 查询键不是永久人物 ID**；不能拿它替代角色唯一身份。
- 人物尚未登录或身份读取失败时，仍允许显示“未识别/待登录”的窗口候选，并保留 PID/HWND 技术信息作为备选。
- 角色同名、角色切换、断线重连、窗口重建、同一角色在不同游戏进程出现等情况，按会话进行区分和重新绑定。
- **角色 ID 用于用户识别；Session ID 用于本次连接生命周期**。游戏进程重启后，即使角色 ID 相同，也必须创建新 Session。

---

## 五、QQFOController：统一操作与截图坐标体系

### 5.1 Controller 接口范围

由 MaaFramework Custom Controller 对外提供原生操作回调，首期重点为：

- `connect` / `connected` / `inactive`：连接检查、释放、异常退出处理。
- `screencap`：FramePool 等适合目标窗口的截图实现。
- `click`：基于 P8.3 的 Proto Input 后台客户区定点点击。
- 键盘相关回调：逐项迁移、分别验证；不可将“鼠标点击 PASS”等同于所有键盘及鼠标 API 均已通过。
- 后续 `swipe`、滚轮、其他输入按实际需求扩展并验收。

底层优先保持原有 x64 主端、x86 游戏端的职责边界。Proto Input 的 Hook 与游戏内 Windows 消息状态处理属于游戏目标侧或其专属辅助进程的输入实现细节；Maa Controller 应保持统一、稳定的上层接口。

### 5.2 Proto Input 验证结论及保留要求

P8.3 已在 QQ幻想客户端验证：

- 不同客户区虚拟坐标 A `(1000,450)`、B `(600,450)`、C `(1000,450)` 分别使人物产生预期的右、左、右方向移动；
- 游戏内部的输入坐标缓存随虚拟坐标更新；
- 通过 `WindowActivateFilter` 触发游戏内部有效输入状态，未通过直接写游戏字段制造成功；
- 测试采样期间物理鼠标和 Windows 前台未发生观测到的变化；
- 正常卸钩路径已验证。

**保留的工程风险：** DLL 异常清理、进程退出恢复、长时间运行、多窗口实例隔离、完整键盘支持等仍需专门验收。P8.3 的 PASS 是**后台定点点击能力 PASS**，并不是整个 Controller 全功能 PASS。

### 5.3 分辨率与坐标统一规范（重点保留历史成果）

明确区分四类坐标：

1. **游戏世界坐标** `WorldPosition`：内存读取的世界位置，服务于寻路、NPC/怪物距离等业务。
2. **游戏客户区坐标** `ClientPosition`：Proto Input 最终点击目标的客户区原始像素。
3. **Maa 截图坐标** `MaaImagePosition`：Pipeline 识别和动作看到的截图尺寸坐标。
4. **Windows 屏幕坐标** `ScreenPosition`：用于 OS 窗口位置、DPI 与 Hook 内部必要换算，不作为业务坐标混用。

统一流程：

```text
原始游戏客户区（例 1600×900）
       ↓ FramePool / 截图
       ↓ Maa 截图尺寸策略（例 1280×720）
Maa 识别 / ROI / Pipeline Click（1280×720 坐标）
       ↓ MaaFramework 坐标缩放（由框架负责）
QQFOController 接收原始客户区目标（1600×900 坐标）
       ↓ Proto Input 游戏线程 ClientToScreen
游戏内部 GetCursorPos / 点击处理
```

原则：**不重复缩放**。若使用 `display_raw`，截图坐标与原始客户区尺寸原则上一致；若使用缩放/自定义尺寸，必须沿 Maa 框架真实转换规则进行测试。以上示例仅说明坐标空间，不代表任何未经验证的硬编码换算在所有 DPI、窗口尺寸和黑边情况下均成立。

### 5.4 保留的用户可配置项

- `display_raw`
- `display_long_side`
- `display_short_side`
- `display_expand`
- 目标 HWND / 人物身份绑定、截图模式（例如 FramePool）、Controller 连接与输入设置
- DPI 缩放与窗口变化后的重新取样

这些是新版 MFAAvalonia 可识别的相关配置字段，但最终哪些配置在 QQFOController 适配层生效、如何传给编辑器/测试页，还需要以 Fork 版本实际代码及测试为准。

---

## 六、QQFOAgent：单入口、内部模块化

### 6.1 核心决定

**一个 Agent 程序 / 一个统一模块注册入口 / 内部按业务功能组织**。

不得将“每个功能一个 Agent EXE”作为默认架构；也不强制“每个角色一个 Agent OS 进程”。

Agent 的模块注册工作可按如下抽象组织：

```cpp
void RegisterAllModules(ModuleRegistry& registry)
{
    RegisterPlayerRecognitions(registry);
    RegisterMapRecognitions(registry);
    RegisterNPCRecognitions(registry);
    RegisterMonsterRecognitions(registry);
    RegisterItemRecognitions(registry);

    RegisterNavigationActions(registry);
    RegisterInteractionActions(registry);
    RegisterPickupActions(registry);
    RegisterCombatActions(registry);
    RegisterBuffActions(registry);
    RegisterFishingActions(registry);
}
```

`ModuleRegistry` 和以上函数是**项目拟定的组织方式**，不是 MaaFramework 已有的同名公开 API；内部应落到对应版本的官方 Custom Recognition / Custom Action 注册接口。

### 6.2 模块层次

| 层次 | 典型模块 | 对外表现 |
|---|---|---|
| 基础数据服务 | `GameStateReader`、`MapReader`、`EntityReader`、`StateCache` | 仅供内部使用，不逐个注册为 Maa 节点 |
| Custom Recognition | `QQFO.PlayerPosition`、`QQFO.IsInMap`、`QQFO.NearbyNPC`、`QQFO.NearbyMonster`、`QQFO.NearbyItem`、`QQFO.AtPosition` | 作为 Pipeline 的判断条件 / 结构化识别结果 |
| Custom Action | `QQFO.NavigateRoute`、`QQFO.InteractNPC`、`QQFO.PickupItem`、`QQFO.Combat`、`QQFO.BuffRoutine`、`QQFO.Fishing` | 执行一个有明确终点的业务动作，完成后交还 Maa |
| Maa 原生节点 | TemplateMatch、OCR、Click、DoNothing、分支等 | 优先直接使用，不用 C++ 重写原生已有功能 |

名称是建议的稳定对外接口；没有注册的模块属于**拟开发接口**，不得在项目文档中标注为已实现。

### 6.3 Custom Action 的边界

- 接收结构化参数：路线名、目标类别、距离、超时、容差等。
- 使用 GameStateReader 获取必要状态；通过当前 Maa Controller 执行操作。
- 允许执行动作内部的有限步骤（如逐路点寻路），但不负责跨任务的总体任务队列。
- 结束时返回成功/失败；对于超时、取消、目标丢失等细节，通过统一业务状态/日志表达，并使用 Maa 原生 `next` / `on_error` 等机制处理分支。
- 长时动作必须支持 Maa 停止请求、超时和连接失效；禁止无限等待。

---

## 七、GameStateReader：同一进程共享、不同进程隔离

### 7.1 共享范围的最终确认

**“统一读取一次，多个模块共享”仅适用于同一个 `qqfo.exe` 进程 / 有效游戏 Session 的一次采样批次。**

```text
Maa 实例 A → Controller A → qqfo.exe PID A
                            └─ GameStateReader A
                               └─ Snapshot A
                                  ├─ PlayerPosition
                                  ├─ NearbyNPC
                                  ├─ NearbyMonster
                                  ├─ NearbyItem
                                  └─ NavigateRoute

Maa 实例 B → Controller B → qqfo.exe PID B
                            └─ GameStateReader B
                               └─ Snapshot B（与 A 隔离）
```

不是程序启动时读一次后永远不更新，也不是全体角色共用一份内存状态。数据可按需刷新、由同一采样批次内多个模块复用；人物位置、怪物、掉落物等可使用不同刷新策略。

### 7.2 统一状态快照（建议接口）

```cpp
struct GameStateSnapshot
{
    GameSessionId session_id;
    uint64_t sequence;
    Timestamp sampled_at;

    MapState map;
    PlayerState player;
    std::vector<GameEntity> entities;

    ReadStatus status;
};
```

上面是逻辑接口草案，不是最终可编译头文件。`GameEntity` 面向 NPC、怪物、地面物品等共同的可确认字段进行标准化；类别特有字段放入各自可选属性，不允许将个别样本推断强制扩展到所有类型。

### 7.3 既有内存研究的复用范围

已经有样本证据支持的能力：

- 场景管理器及本地角色对象链；地图资源与人物世界坐标。
- 场景查询管理器 Q，以及其中本地角色、NPC 实例的记录。
- 芳草林中“粉粉兔”个体的名称、查询键、候选世界坐标及动态成员变化。
- 芳草林地面“兔毛”实例，已核对资源编号 4101、类别、名称、位置、拾取前后成员变化。

应保留的重要技术约束：

- 对象地址可能复用；容器槽位不代表持久身份。
- 场景实体查询键并非永久人物 ID，也未验证跨所有生命周期绝对唯一。
- 先核对当前查询容器成员身份、地图与对象字段，再读取对象数据；离场后不沿旧指针解引用。
- 实测过某一种怪物/物品，不代表所有怪物/掉落物类别、属性或拾取规则都已验证。
- 当前报告中已经验证的字段与尚未确认的候选字段分开标注，附上来源与客户端版本信息。

### 7.4 StateSnapshot 的一致性与有效期

读数应包含：进程及会话标识、地图、采样时间、采样序号、字段有效性与失败原因。需对进程退出、地图变化、对象出入查询容器、部分读取失败等情况作缓存失效处理。

建议采用**按需刷新 + 短时缓存 + 对更新快的字段单独刷新**，而非每个模块不停进行全量扫描。具体刷新间隔及性能阈值以 Windows 实机数据确定，不在总体设计阶段强制写死。

---

## 八、识别结果与动作之间的数据传递

优先使用 **Maa 原生识别结果、Pipeline 节点引用、Custom Recognition 的结构化详情和 Custom Action 参数** 完成协作；不重复开发另一套通用工作流引擎。

### 8.1 短期实体引用（TargetRef）

当 `NearbyMonster` 选出一个怪物，后续 `Combat` 需要准确使用该目标。为避免传递失效的内存地址，内部可采用当前会话有效的短期引用：

```json
{
  "session_id": "example-session-a",
  "map_id": 49,
  "entity_type": "monster",
  "query_key": "0x15259236",
  "snapshot_seq": 128,
  "name": "粉粉兔"
}
```

以上为示意结构，查询键采用历史验证样本，不代表当前实时游戏目标。

- `TargetRef` 不等于永久对象 ID，不能跨地图或游戏重启永久有效。
- Action 实际执行前按当前 Session、地图、成员身份、类别和查询键**重新核对**目标。
- 在 Maa 原生接口可直接提供所需目标信息时，不额外创建全局目标仓库；仅对无法由原生节点参数清晰传递的数据使用最小化会话状态。
- 不同 `qqfo.exe` 进程的目标引用和缓存严格隔离。

### 8.2 后续重点核对

Custom Recognition 的 `out_detail` 虽能返回结构化结果，但具体某版本的 Pipeline 能否将其中任意字段直接作为下一个 Action 的参数，仍须以官方 API 和实际运行测试为准。不得将“识别结果有 JSON”误解为“Pipeline 自动支持通用 JSON 变量传递”。

---

## 九、Maa Pipeline 的最终编排方式

示例策略（逻辑示意，非完整可运行 JSON）：

```text
Maa 任务：芳草林采集与战斗
    ↓
识别当前地图（Custom Recognition）
    ↓
走指定路线（QQFO.NavigateRoute Custom Action）
    ↓
查找附近怪物（QQFO.NearbyMonster）
    ├─ 存在 → Combat → 检查掉落 → PickupItem
    └─ 不存在 → 检查指定路点 / 继续路线
    ↓
检查 Buff（已有视觉或状态数据）
    ├─ 需要 → BuffRoutine
    └─ 不需要 → 下一节点
    ↓
返回 Maa，继续其他 Pipeline 或结束
```

每个模块只提供可组合能力：

- `NavigateRoute` 不决定下一步是否战斗。
- `NearbyMonster` 不自行执行战斗。
- `Combat` 不自带全项目巡逻计划。
- `PickupItem` 不自行安排后续钓鱼。
- 路线、目标筛选、超时、容差等尽量以 Pipeline 参数配置，而非硬编码到 C++。

**目标是不同任务策略能够复用同一批底层识别器与动作模块。**

---

## 十、编辑器、测试页面与用户配置

### 10.1 编辑器

以稳定版 MaaPipelineEditor 作为 Pipeline 和图像识别开发工具。保留原有能力并逐步对接：

- Pipeline 节点图、Custom Recognition / Action 参数编辑。
- 模板匹配、OCR、ROI、截图及识别详情调试。
- 使用 QQFOController 对应的实际游戏窗口及截图尺寸。
- 测试模块时与正式任务遵守相同坐标空间和配置来源。

编辑器先独立维护并集成工作流，不强制把整个编辑器源码塞入 MFAAvalonia 主仓库；确需新增 QQFO 特有控制器/测试功能时再决定是否单独 Fork。

### 10.2 测试页面（新项目长期保留）

建议包含：

1. **连接与身份页：** PID/HWND、角色名称/ID、地图、角色绑定状态与异常信息。
2. **截图页：** FramePool 截图、原始尺寸、Maa 尺寸、DPI、客户端区域、缩放/黑边情况。
3. **输入页：** 指定客户点点击、Maa 截图点测试、真实鼠标/前台变化观测、Proto Input 初始化及卸载检查。
4. **识别页：** TemplateMatch/OCR/ROI、识别结果与实际点击区域叠加。
5. **内存状态页：** 人物、地图、NPC、怪物、物品列表及各字段证据/采样状态。
6. **模块页：** 单个 Custom Recognition / Action 的参数、执行、停止、返回状态与日志。
7. **Pipeline 调试页：** 节点执行轨迹、错误与超时、最近一次截图、关联 Action 日志。

这些应尽可能复用正式 Maa Controller 与 Agent 服务，不产生第二套独立的游戏操作实现。

### 10.3 配置保存与版本升级

- 用户配置、Controller 配置、角色绑定信息与资源默认值区分存储；升级时不覆盖个人设置。
- 日志标记 MaaFW 版本、MFAAvalonia 版本、Agent 协议、QQFOController 版本、游戏客户端版本、PID/Session。
- GUI 配置变化后，截图库和输入映射使用同一份有效配置，不出现“正式运行一套、编辑器一套、测试页一套”。

---

## 十一、多实例、异常与生命周期

### 11.1 多实例规则

- 依托 Maa 自身的实例 / Tasker / Controller 隔离机制，不自建平行调度器。
- 同一 Maa 实例内不同 Pipeline 如果使用同一 Controller，其状态与操作资源也属于同一会话范围；不能误以为“不同 Pipeline 天然互不影响”。
- 一个 Agent 程序中无论是多 OS 进程模式还是共享 Agent 进程模式，状态索引始终以**明确的游戏 Session / PID 绑定**为依据；绝不使用未限定实例的全局可变角色状态。
- 多角色同时操作需要验证 Proto Input 输入状态、消息过滤与 Hook 作用域不会串到其他游戏窗口。

### 11.2 生命周期与中断

必须设计以下场景的结果与清理：

- 角色登录、角色切换、进程重启、游戏退出。
- Controller 连接失败、重绑定、截图超时。
- 游戏地图切换导致对象指针失效。
- Maa 主动停止或 Tasker 停止请求；长时间 Action 及时结束等待并停止后续输入。
- Proto Input Hook 正常卸载、异常退出/清理；避免目标进程中残存无效输入状态。

保留既有成功验证的恢复链，后续对异常态进行专门测试，不把“正常卸钩 PASS”扩展为“任意崩溃情形都可恢复”。

---

## 十二、建议的项目目录（待核对 Fork 实际目录后微调）

```text
22.MaaQQFO/
├── [MFAAvalonia 上游目录结构]          # 保留，尽量少修改核心文件
├── QQFO/
│   ├── Controller/                  # Custom Controller、截图、IPC、Proto Input 适配
│   ├── Agent/
│   │   ├── main.cpp                 # 唯一 Agent 程序入口
│   │   ├── Registry/                # Recognition / Action 注册
│   │   ├── Recognition/
│   │   │   ├── Player/
│   │   │   ├── Map/
│   │   │   └── Entities/
│   │   ├── Actions/
│   │   │   ├── Navigation/
│   │   │   ├── NPC/
│   │   │   ├── Pickup/
│   │   │   ├── Combat/
│   │   │   ├── Buff/
│   │   │   └── Fishing/
│   │   └── Common/
│   ├── GameState/                 # 内存读取、快照、缓存、状态有效性
│   ├── Session/                   # 角色/进程绑定、短期目标引用
│   └── Tests/                     # Controller、截图、Agent、状态测试
├── resource/                      # 遵循实际 MFA / Project Interface 路径约定
│   ├── pipeline/
│   ├── image/
│   ├── routes/
│   └── interface.json
├── docs/
│   ├── architecture/
│   ├── memory-evidence/
│   ├── migration/
│   └── validation/
└── .github/workflows/
```

**说明：** 路径是功能分层草案，不表示已经创建；如果上游需要 `resource/base/...` 等实际布局，必须按已选版本规则调整，不能为了贴合本图破坏资源加载。

---

## 十三、渐进式建设路线（设计顺序，非当前开发任务书）

| 阶段 | 主要建设内容 | 阶段结束应具备的能力 |
|---|---|---|
| 基础阶段 | 核实旧仓库并备份、Fork 稳定版客户端、选定兼容 MaaFW/Agent/MPE、建立 GitHub 与配置基线 | 一个可维护、可构建、能跟踪上游的 MaaQQFO 主项目 |
| 客户端阶段 | 角色身份显示/绑定、Controller 设置、截图分辨率/DPI、编辑器和测试入口 | 用户在统一客户端选择角色与配置运行方式 |
| 输入阶段 | 复用现有 Controller 架构，对接 P8.3 Proto Input，保留 FramePool、IPC 与回归测试 | Maa 原生 Click 节点触发 QQ幻想后台独立定点点击 |
| 状态阶段 | 整合人物、地图、NPC、怪物、物品读取，实现按进程隔离的 GameStateReader | 多个模块可靠读取同一游戏会话的状态快照 |
| 识别阶段 | 注册 Custom Recognition，验证识别结果及 Maa 原生分支 | Pipeline 能按人物/地图/实体状态选择后续任务 |
| 行为阶段 | 实现 NavigateRoute、NPC 交互、拾取等基础 Custom Action | 业务动作执行后返回 Maa，继续后续 Pipeline |
| 策略阶段 | 实现战斗、Buff、钓鱼及跨模块配置复用 | 多套策略共用同一底层组件而不重复开发 |
| 长期维护 | 多窗口回归、异常恢复、上游版本同步、持续测试与发布 | 以 GitHub 为中心长期维护和扩展 |

各阶段具体优先级、验收脚本及可并行事项，进入实施前再共同审定。本方案不授权任何实际删除、Fork 或代码修改操作。

---

## 十四、已确认事项与待核对事项

### 14.1 本次讨论已确认

- 项目名称：**MaaQQFO**。
- 本地根目录：**`F:\各类脚本\2025.12.08-qq幻想脚本开发\22.MaaQQFO`**。
- GitHub：倾向以全新仓库重新建设，保留上游 Fork / 同步关系；旧仓库执行前核实备份。
- 版本路线：**稳定版优先**，不在主线直接使用新 beta。
- 框架核心：**MaaFramework**；最终策略由 **Maa Pipeline** 决定。
- 客户端：**基于 MFAAvalonia 稳定版**建设 QQ幻想专属客户端。
- 编辑器：**使用 MaaPipelineEditor**，保留并整合原有编辑/测试能力。
- 操作底层：**Proto Input** 作为 QQFOController 的后台点击技术基线。
- 角色交互：**优先按读取的人物身份选择游戏实例**，PID/HWND 为内部绑定信息和备选。
- Agent：**单一 C++ Agent 程序入口，内部按功能划分模块**。
- 多实例：**利用 Maa 的实例机制**，不强制一个角色一个独立 Agent OS 进程。
- 状态：**GameStateReader 面向单个 qqfo.exe 进程；同一进程模块共享，跨进程隔离**。
- 模块：**Custom Recognition + Custom Action** 不断扩展，使用 Maa Pipeline 进行策略组合。
- 坐标：保留分辨率、DPI、FramePool 等历史成果，统一编辑器、测试页和实际点击的坐标规范。

### 14.2 后续实施前须核实，不视为已完成

1. 旧 GitHub 仓库的真实位置、所有者、删除范围和备份状态。
2. MFAAvalonia 稳定标签与指定 MaaFramework 稳定版本的实际依赖、ABI/协议兼容性。
3. 旧 P1/P3/D1 控制器源代码、可复用组件以及原有分辨率/测试页面具体实现。
4. 游戏人物稳定身份 ID 的读取方式，跨地图、跨登录和重启后的稳定性。
5. 最新 Maa 客户端中 Custom Controller 的实际创建/选择扩展点。
6. QQFOController 与 MPE 调试控制器的共享接入方式。
7. `WorldPosition` 与客户区目标点的业务换算规则；不能直接把历史显示坐标比例当成屏幕投影。
8. 各实体类别和字段的普适性、有效范围与失效机制。
9. Custom Recognition 结构化结果到 Action 的数据传递能力和必要的最小 TargetRef 机制。
10. Proto Input 在多窗口、长时运行、程序崩溃、异常卸载、键盘输入等情形的回归表现。

---

## 十五、总体结论

**MaaQQFO 的建设目标，是以最新稳定版 Maa 生态为统一底座，在一个可持续升级的 GitHub 项目中，逐步沉淀 QQ幻想专属 Controller、游戏状态读取能力、Custom Recognition 和 Custom Action。**

Maa 负责流程编排；Controller 负责游戏操作；GameStateReader 按单个游戏进程提供可信状态；Agent 统一注册和执行业务模块。角色选择围绕内存读取的人物身份展开，编辑器与测试页面共享实际运行的 Controller 与坐标规范。新功能通过模块化开发接入 Pipeline，旧技术研究成果则经过核对后逐项转化为正式工程能力。

**当前为总体设计留档版本。未经后续明确授权，不进行 GitHub 仓库删除、新建/Fork、项目重构或实机控制。**

---

**文档版本：** V1.0（2026-11-18）  
**建议归档位置：** `22.MaaQQFO/docs/architecture/MaaQQFO_总体架构设计方案_V1.0_20261118.md`（项目建立后再落地）
