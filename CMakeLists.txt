# 标题
cmake_minimum_required(VERSION 3.10.0)
project(Pet VERSION 0.1.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置编译选项
if (MSVC)
    add_compile_options(/W4 /utf-8)
endif()

# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

# 查找并载入Cmake预设
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(glm REQUIRED)


# 显示添加头文件包含路径
# include_directories(${CMAKE_SOURCE_DIR}/src/tools)

# 添加可执行文件
add_executable(${TARGET}
                src/main.cpp
                src/core/game.cpp
                src/core/animation.cpp
                src/core/desktoppet.cpp
                src/pet/catpet.cpp
                src/tools/manifest_loader.cpp
                src/tools/minijson.cpp
                src/tools/hittest.cpp
                src/tools/tools.cpp)

# 添加头文件搜索路径
# 使得 core/desktoppet.h 等路径可被正确解析
target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src)

# 链接库
target_link_libraries(${TARGET}
                        ${SDL3_LIBRARIES}
                        SDL3_image::SDL3_image
                        SDL3_mixer::SDL3_mixer
                        SDL3_ttf::SDL3_ttf
                        glm::glm
                        )

