# MaaQQFO — Codex 总开发任务书（P9，长期自主实施版）

> **版本：** 1.0｜**日期：** 2026-11-18  
> **性质：** 交付给 Codex 的主实施任务书；不是已经完成开发的证明  
> **项目根目录：** `F:\各类脚本\2025.12.08-qq幻想脚本开发\22.MaaQQFO`  
> **项目名 / GitHub 目标仓库名：** `MaaQQFO`  
> **权威设计基线：** `MaaQQFO_总体架构设计方案_V1.0_20261118.md`（本启动包附带）

---

## 0. Codex 的总目标与工作模式

你（Codex）现在承担 **MaaQQFO 项目的主要软件工程实施责任**。目标是基于稳定版 Maa 生态，构建一个可安装/解压运行、支持 QQ幻想角色选择、后台独立点击、统一截图和识别、内存状态读取、可扩展 C++ Agent，以及 Maa Pipeline 策略编排的完整 Windows 项目，并将成果维护在用户授权的 GitHub 仓库中。

**用户希望尽量少参与**。你应在安全与权限范围内自主阅读源码、设计、实现、构建、调试、修复、编写测试、运行自动验收、提交分支、建立 PR、维护进度和发布文档。只有确实需要用户介入的实机操作、授权、账户权限或高风险不可逆动作，才集中提出明确、最少量的问题。

**重要：本任务书是多阶段开发合同，不是“只研究并报告”的任务。** 每一阶段的默认行动是完成实际代码、运行测试并提交可检验的成果；不要完成一份方案就停止，也不要把尚未实测的功能宣称通过。

### 最终验收原则

- **MaaFramework 是唯一任务流程调度中心**，不编写与 Pipeline 平行的第二套业务任务引擎。
- **一个统一的 C++ Agent 程序入口**，内部注册 Custom Recognition / Custom Action；不要求一个角色一个独立 OS Agent 进程，跟随 Maa 客户端的正常实例管理机制。
- **GameStateReader 按 `qqfo.exe` 进程及有效会话隔离**；同一进程内模块共享短期数据快照，不同进程绝不混用。
- **QQFOController 负责所有底层游戏操作**，原生 Maa `Click` 经由 P8.3 验证成功的 Proto Input；不恢复 DM 与 Proto Input 并行的默认点击路线。
- **角色 ID 优先显示**，PID/HWND 保留为内部技术信息和读取失败时的备用选择；永久人物 ID 的稳定性须验证，场景 query_key 不得冒充永久人物 ID。
- **分辨率/DPI/截图/编辑器/测试页面统一**，保留 `display_raw`、`display_short_side`、`display_long_side`、`display_expand` 配置能力，避免二次缩放。
- 必须复用经实测确认的旧成果：P1/P3/D1 Controller/IPC/FramePool、P8.3 Proto Input、C1/C2 场景对象研究、既有图片/管线/路线和工具；先核实源码和证据，不凭名称假定可用。
- **最终产物**应包括可运行 Windows 发布包、源码、构建脚本、配置和示例 Pipeline、模块清单、测试日志、使用手册、GitHub Release。不能以“报告完成”替代“工程完成”。

## 1. 先读的材料与版本纪律

首先完整阅读：

1. `docs/architecture/MaaQQFO_总体架构设计方案_V1.0_20261118.md`；若首次启动仅存在下载目录中的同名文件，复制到对应 `docs/architecture/`，且不得覆盖较新的已批准设计文档。
2. 新旧 MaaQQFantasy / MFAAvalonia 本地历史源码及其测试日志；以**实际文件**识别有效模块。
3. 2026-11-18 的 P8.3 Proto Input PASS 报告及编译日志；既有 CE C1/C2 人物、NPC、怪物、兔毛证据和原始快照。
4. MaaFramework、MFAAvalonia、MaaPipelineEditor 对应版本的官方 API、Project Interface、C++ Agent、截图与自定义 Controller 文档。

**基线目标（确认依赖兼容后冻结到锁文件）：**

| 组件 | 初始目标 | 要求 |
|---|---|---|
| MFAAvalonia | `v2.16.4` 稳定版 | 作为有 upstream 关系的 Fork 主体；不要用 `master` HEAD 假装稳定版 |
| MaaFramework | `v5.16.1` 稳定版 | 以客户端实际依赖可兼容版本为准；Agent 协议必须匹配 |
| MaaPipelineEditor | `v2.1.3` 稳定版 | 优先独立工具接入；确需修改时再单独 Fork |
| Proto Input | `dd29bf8d22349605d0e1950c1c5dc4893d72670e` | 固定源码提交、补丁、EasyHook x86 版本、许可证及验收资料 |

不擅自升级 MaaFramework 6.0 beta。若所选 MFAAvalonia 稳定标签实际依赖的 MaaFW 不等于目标 5.16.1，优先完成依赖矩阵和兼容性验证，锁定一套**确实可运行**的稳定组合；不得为了凑版本号造成混装。

## 2. 首次执行流程（不可跳过但应自动进行）

### 2.1 检查目录和 GitHub

- 检查 `F:\各类脚本\2025.12.08-qq幻想脚本开发\22.MaaQQFO` 是否已有文件；**不覆盖未知用户数据**。建立清单并安全规划布局。
- 搜索用户已授权 GitHub 下的 `MaaQQFO`、`MaaQQFantasy` 等旧仓库，输出仓库 full name、owner、可访问状态、Fork 关系、是否含独有成果。
- **禁止未经用户针对具体仓库的最终确认而删除远端仓库、改仓库可见性或强制改写历史**。用户原则上希望旧项目删除重建，但尚未确认精确远端对象；先备份，必要时用临时名称或新工作分支推进不受影响的任务。
- 对 `MaaXYZ/MFAAvalonia` 建立可追踪的官方 upstream；新主线目标名称 `MaaQQFO`。Fork/重命名/推送如果当前登录没有权限，要提出一次具体授权请求，不用自行伪造账号或凭证。
- 不修改旧本地项目；只读取、复制已核实有价值的代码或报告，并注明来源、许可证。

### 2.2 建立基线

- 获取官方稳定源码，并按 GitHub Fork 原关系组织；避免通过无 upstream 的纯复制破坏维护能力。
- 完成 Windows x64 构建基线，记录 .NET/C++ 工具链、实际框架 DLL、Agent 协议、架构、入口、依赖锁定。
- 建立 GitHub Actions：格式/静态检查、可自动运行的构建与单元测试、打包；游戏实机测试只能在用户授权的 Windows 测试环境执行，CI 不得伪造游戏实机 PASS。
- 建立 `AGENTS.md`、`docs/engineering/ROADMAP.md`、`docs/engineering/PROGRESS.md`、`docs/engineering/HANDOFF.md`、`docs/engineering/BLOCKERS.md`、`docs/validation/EVIDENCE_MATRIX.md`。

## 3. 长期任务分章实施（按依赖推进；独立事项允许并行）

### A0 — 工程资产与风险盘点

**工作：** 全量扫描可访问的新旧工程、GitHub 状态、历史源码、资源、日志、需求和依赖；形成实际存在的文件清单与技术成果证据矩阵。将过去验证通过的功能和仅提出方案的功能明确区分。

**验收：** `MIGRATION_INVENTORY.md` 至少覆盖旧 Controller、IPC、截图、DPI、编辑器、测试页、Proto Input、人物/地图/NPC/怪物/掉落物、路线、既有 Pipeline 与图片；给每项标注 `VERIFIED / PARTIAL / PLAN / NOT_FOUND` 以及来源文件。不能用想象中的旧代码开工。

### A1 — 新 GitHub 主线和稳定版基座

**工作：** 在用户许可范围内建立 MFAAvalonia 稳定版 Fork/工作分支及 QQFO 专属扩展目录，完成源码构建、依赖锁定、基础 CI、文档和 GitHub Issue/PR 规范。

**验收：** 新源码可干净克隆、干净构建，能够启动未改动或最小改动的 Maa 客户端；构建过程可重复；MaaFramework 与 Agent 协议匹配；Fork 的 upstream 关系可追溯。

### A2 — Controller 契约、分辨率和坐标基线

**工作：** 核实官方 `MaaCustomControllerCreate` 的真实创建点，以及 MFAAvalonia Controller 工厂/枚举/选择入口；迁移 P1/P3/D1 有效的 C++ x64/游戏 x86 桥接设计、IPC、FramePool、DPI 工具。先实现一个不依赖真实游戏动作的可测 Controller 骨架。

**必须区分的坐标：** `WorldPosition`、`ClientPosition`、`MaaImagePosition`、`ScreenPosition`。记录原始游戏客户区尺寸、原始截图尺寸、缩放后 Maa 图像尺寸、DPI，测试 `1600x900 -> 1280x720` 等典型路径和窗口变更。**MaaFramework 已处理的缩放不得在 Controller 重复执行。**

**验收：** `connect/connected/screencap` 及尺寸选项可测；配置 `display_raw`、`display_short_side`、`display_long_side`、`display_expand` 在新版 UI、截图和返回尺寸上符合实际语义；测试窗口的四角、中心及边界坐标映射正确。

### A3 — QQFOController + Proto Input 完整接入

**工作：** 将 P8.3 已验证的 Proto Input 虚拟鼠标及 `WindowActivateFilter` 有效性机制变成 QQFOController 的正式 `click` 后端；尽量复用现有控制器/Named Pipe；按 x64 主端与 x86 游戏端划分职责，保留窗口绑定、取消、卸载、日志和恢复链。不要另造 Maa Host 或第二套点击器。

**验收：** Maa Pipeline 的**原生 Click 节点**在后台真实 QQ幻想内按 A/B/C 不同坐标产生相应动作；不抢系统前台、不移动用户真实鼠标（在可观察范围内）；Hook 正常卸载；游戏仍响应。键盘、右键、滚轮等须**逐功能测试**，不能从 Click PASS 推定其它输入已通过。

### A4 — 角色选择界面 + Controller 设置 + 统一测试页面

**工作：** 让用户通过人物名称/经验证的稳定 ID 选游戏实例，显示地图/连接状态；PID/HWND 作为内部绑定数据和识别失败时的候选。保存 Controller、截图、DPI、输入、任务和账号（不含密码）配置。恢复并升级旧项目的截图页、点击页、识图/OCR/ROI 页、Agent/Action 页和日志页。

**验收：** 多窗口角色列表可刷新；未登录状态能够正常显示窗口候选；切换角色或重启游戏后不使用旧句柄；配置保存与版本升级不丢失用户设置。测试页面与正式 Pipeline 使用同一 QQFOController 和截图/坐标规则。

### A5 — MaaPipelineEditor 接入

**工作：** 优先采用 MPE v2.1.3 作为独立编辑器，复用同一 Controller/截图配置，提供项目内可直接打开编辑器和资源的路径/入口，测试 `TemplateMatch`、OCR、ROI、Custom Action/Recognition 调试。只有发现具体缺口并提供复现后，才评估单独 Fork MPE；不要将整个 MPE 粗暴复制入 MFAAvalonia 主源码。

**验收：** 同一模板/ROI 在编辑器和 Maa 正式任务的坐标一致；编辑器测试与正式 Click 使用同一后端（不偷用普通 `SendMessage`）；有可复现的截图和日志证据。

### A6 — GameStateReader / 内存数据层

**工作：** 将已实测的场景管理器、人物对象、地图、查询容器 Q、NPC、粉粉兔、兔毛等旧研究转换为只读且可验证的 C++ 数据接口；按 PID/Session 隔离，按需采样、短时缓存、对象失效校验、地图切换清理、健康状态；**不得绕过证据假定所有实体通用字段一致**。

**验收：** 同一游戏进程的多个模块可共享一个可信 `GameStateSnapshot`；两个游戏进程绝不串数据；游戏退出、重新登录、地图变化、对象出入容器均可正确处理；输出含采样时间、会话、地图、原始类别/查询键与数据有效性。人物永久 ID 若未定位必须标记为未支持并提供候选回退路径，不得伪造。

### A7 — Custom Recognition 注册与 Maa 分支

**工作：** 在唯一 C++ Agent 程序入口统一注册 `QQFO.PlayerPosition`、`QQFO.IsInMap`、`QQFO.NearbyNPC`、`QQFO.NearbyMonster`、`QQFO.NearbyItem`、`QQFO.AtPosition`；以当前版本的真实 API 开发，先研究原生节点引用/out_detail 的能力，再增加必要的最小 TargetRef。

**验收：** Maa Pipeline 依靠实时游戏状态进行至少三种可复现条件分支；不存在的实体返回未命中而非假成功；不同实例 TargetRef 严格隔离；没有永久保存内存地址作为唯一 ID。

### A8 — 导航 Custom Action

**工作：** 实现 `QQFO.NavigateRoute`，读取资源中的路线、使用游戏世界坐标与地图状态判断，操作通过当前 Maa Controller 完成；明确区分世界、显示、客户区和截图坐标；重用旧寻路状态机设计中的有证据部分。支持超时、无位移、地图切换、目标失效、任务停止和结果记录。

**验收：** Maa Pipeline 调用路线动作，人物逐路点行动，到达后返回 Maa 自动继续下一节点；错误路径能够停止并产生可审计原因；不得用历史回放或模拟点击冒充本次实机验收。

### A9 — 基础交互动作与策略复用

**工作：** 逐项实现 `QQFO.InteractNPC`、`QQFO.PickupItem` 以及所需的识别/等待/动作后核实；围绕已确认的 NPC/怪物/兔毛样本先形成可复现的简单策略。实现“导航 → 实体识别 → 交互/拾取 → 继续 Pipeline”完整闭环。

**验收：** 有至少两套不同 Pipeline 复用相同的 Recognition/Action，而非复制内存扫描/点击代码；失败情形有可重试/取消语义，不能仅凭点击 API 返回成功判定拾取成功。

### A10 — Buff、战斗、钓鱼与扩展机制

**工作：** 按用户既有策略需求，将 `QQFO.BuffRoutine`、`QQFO.Combat`、`QQFO.Fishing` 分拆为可测试子模块，继续复用 Maa 原生图像识别与 Controller；优先实现已明确且可实测的子功能，其他字段/事件通过专项样本继续验证。模块必须支持任务停止与持续状态采集。

**验收：** 每个功能提交能力清单、测试案例、实机记录及 Pipeline 样例；不将待研究项目标为完成。加入新模块不需要修改既有 Controller 或重新创建 Agent 程序。

### A11 — 多实例、异常恢复和稳定性

**工作：** 验证两个以上真实游戏进程，Tasker/Controller/Agent 绑定，Proto Input 作用域、IPC、缓存、配置、重绑定及任务中断；测试正常退出、异常断开、游戏窗口重建、长时间运行。特别审查 P8.3 中 DLL 驻留与 Hook 异常恢复的风险。

**验收：** 两实例能执行不同 Pipeline 且互不干扰；一个实例退出不破坏另一个；停止指令不继续发下一动作；重启后不会读取旧对象；所有风险/缺口有明确等级和复现证据。

### A12 — 发布、文档、验收与维护

**工作：** 编写一键构建、首次安装、QQ幻想角色选择、Controller 设置、分辨率、MPE、测试页、任务配置、常见故障和卸载指南；生成 Windows Release 压缩包及校验和；GitHub 发布流程；CI 和可重复的开发测试步骤；保留对上游稳定版本更新的兼容分支机制。

**验收：** 在干净 Windows 环境（按列明的前置条件）解压/安装、启动、连接、截图、点击并执行样例 Pipeline；发布包不依赖开发者机器的旧绝对路径；版本号、源码 commit、构建记录和证据矩阵一致。

> **原则：** 阶段 A0–A12 是完整项目工作流。允许在不影响依赖的情况下并行小任务，但不能因一个阶段暂时需要实机验证，就谎称整个项目已完成。可先继续与其无依赖的工作，并在 BLOCKERS 中记录真实状态。

---

## 4. 每个阶段的强制执行循环（尽可能无人参与）

对每一个阶段：

1. **Read：** 读总体设计、本文件、AGENTS.md、ROADMAP、最新 HANDOFF、相关源码与上阶段实证。
2. **Plan：** 列出明确的文件修改、测试、验收条件；范围限制在当前阶段和必要依赖，不进行无关重构。
3. **Implement：** 真实修改代码/资源/测试；能自动复现的错误自行修复，不把第一次构建失败立即抛给用户。
4. **Verify：** 执行可运行的静态检查、构建、单元/集成/模拟测试；实机测试必须有实际登录游戏的证据。
5. **Review：** 自检 diff；运行 GitHub PR 的自动检查，能启用经授权的 Codex Code Review/独立模型复审时进行第二层审核，修复审核意见。
6. **Commit：** 使用 `phase/Ax-<feature>` 等工作分支、原子提交、必要 PR；只在授权及分支保护允许时自动合并低风险通过项。禁止向未确认仓库强推。
7. **Record：** 更新进度、证据矩阵、未通过项、下一步和恢复命令。**下一个执行回合从 HEAD + HANDOFF 恢复**，而不是重新询问用户已经提供的信息。
8. **Advance：** 若本阶段的全部可自动验证条件满足，继续下一项；如果仅缺需要用户实机验证的部分，建立阻塞条目及测试界面，优先推进无依赖任务。

所有阶段状态仅使用：`NOT_STARTED` / `IN_PROGRESS` / `BLOCKED_USER` / `BLOCKED_TECH` / `VERIFY_PENDING` / `PASS`。`PASS` 需要可检查的证据；不以纯文字自报代替构建/运行记录。

### 阶段交付最小证据

- `commit SHA` 与 PR/Issue 链接（已有 GitHub 许可时）；
- 改动文件及关键设计取舍；
- 构建命令及结果、测试数量与结果；
- 实机测试的客户区坐标、人物变化、前台/光标采样、目标 PID/HWND/Session（适用时）；
- 未覆盖功能、风险、已知问题及复现办法；
- 下一回合应做的**唯一首要任务**和允许并行的独立任务。

## 5. 额度限制、自动续工与恢复协议（必须实施，但不得声称突破限额）

### 5.1 客观限制

Codex 的 5 小时使用窗口和可能存在的周限额由账号和服务端控制。到达限制时不能靠不断重试、切换身份、隐含消费或其他方式绕过。官方 Goals 可以持续工作到预算/其他停止条件；自动化可以按计划恢复任务，但**不保证额度重置的具体时刻或重置后必然无人值守成功继续**。

### 5.2 优先方案：Codex Goal + Codex 原生 Automations

1. 检查本机 Codex 版本及是否支持 `/goal`；若支持，使用**单阶段、可验收**的 Goal，而不是“永远做完所有功能”的无边界目标。
2. 检查 Codex 桌面版是否具备可用的 Automation，以及能否沿用当前工程线程/上下文；如果支持，按用户许可创建**定期续工检查**，目标为“读取仓库最新 ROADMAP/HANDOFF，若有额度且无人工阻塞就继续下一未完成阶段；否则记录状态并退出”。
3. Automation 执行频率由工具提供的实际可用调度能力决定，建议低频，如每 1–2 小时检查一次；不得在触及额度后高频重试。**不要把“5 小时后”写成固定的服务端恢复时间。**
4. 如 Codex Desktop 对 Windows 机器上的本地自动化不可用，允许用户另行授权后开发**Windows 任务计划程序 + Codex CLI/SDK** 的保守续工启动器；必须先验证 `codex` 版本、实际帮助信息、登录状态、会话恢复功能和用户当前权限，不能假定 `resume` 命令的参数跨版本恒定。
5. 自动续工仅在已授权范围、机密不外泄、机器可用、相关额度和 API/账户权限满足时运行。不弹 UAC 就尝试绕过、不自动购买额度、不修改付款设置。

### 5.3 断点状态文件（每次提交前、额度接近时、任务退出前更新）

`docs/engineering/HANDOFF.md` 必须包含：

```markdown
# MaaQQFO 开发交接
- 最后更新时间：ISO8601
- 当前阶段：A?
- 当前状态：IN_PROGRESS / VERIFY_PENDING / BLOCKED_...
- 工作目录：F:\各类脚本\2025.12.08-qq幻想脚本开发\22.MaaQQFO
- Git 分支 / HEAD：...
- 已完成：...
- 已执行的验证与结果：...
- 当前未提交文件及原因：...
- 下次恢复的第一条动作：...
- 下一阶段：...
- 人工阻塞：无 / 具体所需操作
- 当前风险与回滚方式：...
```

长任务尽量按可验证的小里程碑完成后就 Commit，不依赖超长上下文。恢复时先运行 `git status`、读取 HEAD/HANDOFF/ROADMAP/EVIDENCE，再继续当前阶段。禁止因超时重新初始化仓库或覆盖已完成代码。

### 5.4 额度不足时

- 不虚报已完成；将阶段置为 `IN_PROGRESS` 或 `BLOCKED_TECH`，记录已提交证据。
- 若本次 Codex 可继续工作则正常完成当前有限回合；否则停止发起模型请求。
- 由真实可用的 Codex Automation / 经用户授权的本地续工器在之后**尝试**恢复。恢复后先校验额度/环境可用性；不可用则退出等待下次调度。
- 若到达每周限额、无法登录、用户关闭电脑或自动化不支持此环境，需如实标记阻塞；**不得承诺五小时后一定继续。**

## 6. 多层自动验收，而非要求用户逐阶段审稿

### 第一层：程序和测试的机器验收

CI 必须覆盖构建、格式/静态检查、DLL 架构/版本一致性、内存快照离线回放、坐标变换测试、基本 Controller/Agent API 冒烟、资源加载和打包。每次 PR 运行，失败自动修复；禁止禁用失败测试来换绿色 CI。

### 第二层：代码审查

- 用 GitHub PR 留存变更，能使用 GitHub/Codex Code Review 时进行复核；必要时在授权与费用允许下通过 GitHub Actions 调用另一个模型做独立代码审查。
- 对图像缩放、会话隔离、越界读取、DLL 卸载、长时 Action 取消、升级/打包等高风险变更要求审查意见已解决；**同一个 Codex 的自检不等于独立验收。**
- 自动审查可能漏掉问题；最终软件质量仍由可复现的实机和软件测试决定。

### 第三层：ChatGPT 复核接口

本项目用户希望由当前 ChatGPT 对方案和里程碑做第二层验收。**不要假装 Codex 可以直接调用这段 ChatGPT 会话**。正确方法是将每个里程碑的 `docs/validation/STAGE_Ax_REVIEW.md`、PR、日志、测试矩阵、可下载构建包保存在已授权的 GitHub 仓库；用户随后可以让 ChatGPT 读取 GitHub 资料进行复核。若用户未来连接了允许定期触发的 ChatGPT Work/自动化和 GitHub 工具，也可在用户显式启用后建立自动检查流程。

如用户授权使用 OpenAI API 为 GitHub PR 构建 AI Reviewer，那是**另一个可审计的 AI 审查任务**，不是与当前 ChatGPT 会话自动对话；需独立密钥、费用、权限、审计记录和安全限制。

### 第四层：真实游戏验收

GitHub CI 无法替代真实 Windows 游戏客户端。后台点击、Hook 还原、角色登录、地图移动、物品拾取、战斗、钓鱼、多窗口，都须在用户授权环境中测试。先构建一键测试页面和短脚本，并在可安全重复时自动执行；**需要用户登录、UAC 提权确认、游戏内点位选择或人工判断的步骤，集中一次提出，不要每天让用户填重复信息**。

明确标记三种验收：`OFFLINE_PASS`、`WINDOWS_LIVE_PASS`、`FULL_ACCEPTED`；离线测试通过不能升级为游戏实机通过。

## 7. 权限与必须请用户介入的边界

**可在当前工具与已有授权允许的工作区内自主做：** 代码编辑、文档、编译、离线测试、生成构建物、非破坏性的 Git 分支、经授权的 PR/推送、查阅开源文档、复核旧文件、复用资源。

**必须明确授权/确认：** 精确指定的旧远端 GitHub 仓库删除或重命名、强制推送、不可逆批量删除、购买额度和付费 API、创建/扩展凭据权限、首次向真实游戏注入/安装管理员组件、涉及真实账号操作/高风险实机动作、对用户机器的计划任务持久驻留启用。即使总目标是尽量无人值守，也不能跳过这些权限边界。

**用户的实机配合应最小化：** 优先一次性提供测试窗口和游戏登录状态，在安全范围内让 Codex 自动执行批量校验；需要用户肉眼判断时使用单个测试面板，收集明确 PASS/FAIL/实际效果，不要求用户手动重复几十次复杂步骤。不得在日志、PR、截图中泄露游戏密码、Token、敏感文件。

## 8. 交付完成定义（Definition of Done）

只有以下均符合证据，才可宣称“完整 MaaQQFO 发布候选版完成”：

- [ ] GitHub 新主线可维护、与上游关系可追踪、干净环境构建 PASS。
- [ ] MaaQQFO Windows 发布包可下载、校验、启动，提供完整安装/运行说明。
- [ ] 按角色选择绑定实例（身份读取失败有真实可用的降级 UI），连接/重连正常。
- [ ] QQFOController 的截图及分辨率在正式运行、编辑器、测试页语义一致；原生 Click 后台实机 PASS。
- [ ] C++ Agent 单入口、模块可注册；GameStateReader 单进程共享/多进程隔离 PASS。
- [ ] 人物/地图/NPC/怪物/物品基础 Recognition 有实际快照或实机证据，条件分支成功。
- [ ] NavigateRoute 真实运行到达并返回 Maa 继续 Pipeline。
- [ ] NPC 交互、基础拾取、Buff/战斗/钓鱼在**明确列出的支持范围内**可实测运行；未支持的功能不伪装已完成。
- [ ] 多实例基础隔离、游戏进程退出、Maa 停止、Proto Input 正常清理有测试证据；未解决的高风险缺陷不得无说明发布。
- [ ] Github Release、变更说明、功能矩阵、测试报告、最终源码 commit 与可复现构建方式齐全。

如果某些需求客观上缺游戏样本、权限或尚未验证的逆向链路，可以交付**明确受限的版本**，但必须以 `PARTIAL` 标注限制，不能当作全功能正式版。

## 9. 第一次交给 Codex 时的执行指令（复制本段即可）

> 请完整阅读当前任务书和随附 V1.0 总体架构文档，以 `F:\各类脚本\2025.12.08-qq幻想脚本开发\22.MaaQQFO` 为唯一新工程根目录。你负责此项目的持续开发：优先完成 A0→A1，随后按 A2–A12 的依赖逐步编译、实现、验证、修复、提交、归档。每阶段必须有代码/可检查交付和实际测试，不得只交一份规划。采用 AGENTS.md、ROADMAP、PROGRESS、HANDOFF、BLOCKERS 和证据矩阵记录断点；尽量不要求用户逐步确认。旧 GitHub 仓库删除、账号权限、实机管理员注入、付费行为等需独立明确授权。核实 Codex Goals 和 Automations 当前可用能力，若具备，则在权限许可内设计和启用低频自动续工；遇到 5 小时或周额度不能绕过，记录进度并在允许时尝试恢复。各里程碑建立 GitHub PR 和可供 ChatGPT 审查的报告。现在立即开始实际检查旧工程和 GitHub、创建迁移清单，先完成 A0 的实证盘点，并继续执行不受人工阻塞的 A1 工作；不要再次向用户询问已经写明的工程路径和架构原则。

---

## 10. 官方资料（用于核实实时能力，而不是假定可用）

- MaaFramework: https://github.com/MaaXYZ/MaaFramework
- MFAAvalonia: https://github.com/MaaXYZ/MFAAvalonia
- MaaPipelineEditor: https://github.com/kqcoxn/MaaPipelineEditor
- Codex 使用与额度：https://help.openai.com/en/articles/11369540-using-codex-with-your-chatgpt-plan
- Codex Goals：https://developers.openai.com/cookbook/examples/codex/using_goals_in_codex
- Codex 长期计划：https://developers.openai.com/cookbook/articles/codex_exec_plans
- Codex 自动续工能力：https://openai.com/index/codex-for-almost-everything/
- Codex SDK / app-server：https://developers.openai.com/blog/codex-as-a-platform

**本任务书不会自动使 Codex 在用户电脑上运行，也不会自动获得 GitHub、管理员或付费权限；请由用户在其 Codex 工作环境中启动，并按实际功能完成自动化配置。**
