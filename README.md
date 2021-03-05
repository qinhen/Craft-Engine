# Craft-Engine

## History
2019/6/?? - 2020/6/30 v0.00






2020/6/30 v0.01
[gui]add Slider Template Widget

2020/7/2 v0.02
[gui]add Vulkan backend for gui renderer

2020/7/10 v0.03
[net]add network module
supporting http,ftp protocol 
as well as fundamental socket c++ encapsulation

2020/7/15 v0.04
[gui]add Tab Widget

2020/8/12 v0.05
[gui]add Rich-Text Textbox(bugs)

2020/9/13 v0.06
[gui]add Model-View-based ItemWidget
similar to Qt Model-View pattern

2020/9/27 v0.08
[graphics]add 3D Graphics module
add related class such as AbstractScene, Vertex, AbstractRenderer
, Model, ModelInstance, Material, Camera

2020/10/14 v0.10
[gui-guiext]add ColorTextEditWidget
base on: https://github.com/BalazsJako/ImGuiColorTextEdit
support tokens colorize for language like glsl, c/c++, hlsl...

2020/10/30 v0.11
[graphics]add a appropriative-3d-model format and method to transform from other 3d-format using assimp

2020/11/10 v0.12
[gui]add NodeEdit Widget, base on modelview

2020/11/24 v0.13
[graphics]implement PBR with graphics module

2020/12/4 v0.14
[gui-guiext]add MaterialEditWidget

2020/12/13 v0.15
[graphics]implement Pre-Calcular Sky Model with graphics module

2020/12/23 v0.16
[soft3d]add soft3d module
a Vulkan-like cpu graphics api.
support concept such as Memory, Buffer, Image, Sampler, Framebuffer, Device, ImageView, Shader, SwapChain, Pipeline...
support a lot of image operations, such as nearest/linear/mipmap sample
support a lot of image format, such as R8G8B8A8_Unorm, R32G32B32A32_float...

2020/12/26 v0.17
[soft3d]add rasterization graphics pipeline
support drawing triangle\line\point by rasterization graphics pipeline
support multi-threads rasterization base on the quantity of logical-cpu-core on machine 

2021/1/15 v0.18
[soft3d]add raytracing graphics pipeline
a Vulkan-RayTracing-extension-like cpu raytracing api.
support concept such as TopLevelAccelerationStructure, BottomLevelAccelerationStructure
, RayHitShader, RayMissShader, AnyHitShader, ShaderPayload...

2021/1/29 v0.21
[gui]add FontSelector Widget

2020/2/8 v0.22
[graphics]add skeleton animation support
modify graphic::ModelFile to support skeleton animation
add graphic::AnimationSlover



## Dependency
|base|Version|
|:---|:--:|
|glew/vulkan-loader||
|stb-img||

|optinal|Version|
|:---|:--:|
|ffmpeg||
|libyuv||
|rapidjson||
|sqlite3||
|assimp||
|openAL||
|Lua||


## Description
Craft-Engine是一个使用c++，包含Gui（最主要的）、3D图形渲染、多媒体播放、3D线性代数、简单网络通信等多个方面相关代码的集合，是本人在学习过程中编写的一部分可重用的的代码。

## Example
#### 01.Quick Start
``` C++
#include "CraftEngine/gui/Gui.h"
using namespace CraftEngine;
int main()
{
	gui::Application app;
	auto main_window = new gui::MainWindow(L"Craft-Engine Example-01: MainWindow");
	main_window->exec();
}
```
#### 12.Material Editor
![12](https://github.com/qinhen/Craft-Engine/blob/main/Images/Example12.PNG?raw=true)

#### 15.Pre-Calcular Sky Model
![15](https://github.com/qinhen/Craft-Engine/blob/main/Images/Example15.PNG?raw=true)

#### 17.Cpu-Gui-Rendering&Cpu-Rasterization
![17](https://github.com/qinhen/Craft-Engine/blob/main/Images/Example17.PNG?raw=true)

## Sub Projects

#### MediaPlayer

#### SkeletonAnimation

#### Backend for DearImgui

#### Cpu-Path-Tracing

