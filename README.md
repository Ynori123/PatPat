# PatPat
- 一只叫PatPat的桌宠小猫，你可以称呼她帕帕或者啪啪，甚至直接叫papa也不会惹她生气
- 目前项目还在处于开发阶段
- 啪啪只是一只小猫，学什么都学得很慢，请不要迁怒于她

## 项目简介
- 个人独立开发的陪伴型桌宠
- 目前仅适用于Windows平台（win10 & win11均可，更早的版本没有进行测试）
- 纯属个人娱乐项目
- 使用SDL3与Cmake进行构建

## ToDo List
基本按照预期实现的时间先后排序
- papa的随机行为

## 功能特性
- 透明、置顶、无边框窗口（分层窗口，支持 Alpha 合成）
- 基于 JSON manifest 的精灵动画加载（支持行/列/网格与自定义 rects）
- 状态机驱动动画（Idle/Walk/Click 等）
- 动态点击穿透（鼠标在宠物外时透传到底层窗口，宠物区域内可交互）
- 每秒一次的 FPS 与平均帧时日志（节流打印，避免刷屏）
- SDL3 子系统初始化：Image/Mixer/TTF
- 菜单栏与番茄钟（计时功能）
- 敲击键盘次数记录
- 和papa进行对话


## 目录结构（节选）
```
.
├─ CMakeLists.txt
├─ resources/
│  └─ sprites/CatPet/
│     ├─ manifest.json
│     ├─ idle_anim.png
│     ├─ walk_anim.png
│     └─ click_anim.png
└─ src/
	 ├─ main.cpp
	 ├─ core/
	 │  ├─ game.{h,cpp}
	 │  ├─ animation.{h,cpp}
	 │  ├─ desktoppet.{h,cpp}
	 │  └─ …
	 ├─ pet/
	 │  └─ catpet.{h,cpp}
	 └─ tools/
			├─ manifest_loader.{h,cpp}
			├─ minijson.{h,cpp}
			├─ t_UI.h（Win32 UI 工具函数实现）
			└─ tools.{h,cpp}
```

## 构建与运行（Windows）
项目使用 CMake，依赖：
- MSVC（Visual Studio 2022 / Build Tools）
- CMake ≥ 3.10
- SDL3、SDL3_image、SDL3_mixer、SDL3_ttf、glm（建议用 vcpkg 安装）

示例（PowerShell + vcpkg，可按需调整路径）：

```powershell
# 可选：安装依赖（示例）
# vcpkg install sdl3 sdl3-image sdl3-mixer sdl3-ttf glm --triplet x64-windows

# 配置并生成（如使用 vcpkg：指定 toolchain）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug `
	-DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" `
	-DVCPKG_TARGET_TRIPLET=x64-windows

# 编译
cmake --build build --config Debug --target ALL_BUILD -j 12

# 运行（可直接从项目根目录，输出目录已配置到根目录）
./Pet-Windows.exe
```

你也可以：
- 使用 Visual Studio 打开文件夹并直接“生成/启动”；
- 在 VS Code 中使用 CMake Tools 插件（选择 MSVC Kit，配置并构建）。


## 许可证
- MIT
- 该项目为个人独立开发的项目
- 图片素材来源于网络无版权素材，在此感谢开源
- 后续考虑新增更多美术资源，对于需要授权使用的素材会加以说明

## Version
### 0.0.1
- 基本的播放动画
- 处理鼠标点击事件
- 屏幕穿透