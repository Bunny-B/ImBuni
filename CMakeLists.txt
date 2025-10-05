cmake_minimum_required(VERSION 3.16)
project(ImBuni LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/MP) # multi-processor build
endif()

# --- Source files ---
set(SOURCES
    src/render/render.cpp
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
)

# --- Header files (for IDE) ---
set(HEADERS
    src/render/render.h
    vendor/imgui/imconfig.h
    vendor/imgui/imgui.h
    vendor/imgui/imgui_internal.h
    vendor/imgui/imstb_rectpack.h
    vendor/imgui/imstb_textedit.h
    vendor/imgui/imstb_truetype.h
)

add_subdirectory(vendor/glfw)

# --- Library target ---
add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}  # optional, IDE only
)

# --- Include directories ---
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb

)

add_library(stb_image INTERFACE)
target_link_libraries(${PROJECT_NAME} PRIVATE stb_image)


# --- System dependencies ---
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw opengl32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
        dl
        X11
        Xrandr
        Xi
        Xxf86vm
        Xinerama
        Xcursor
    )
endif()

# --- Filters for IDE ---
source_group("Root" FILES RenderLib.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"
    FILES
        src/render/render.cpp
        src/render/render.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui PREFIX "ImGui"
    FILES
        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_tables.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/imconfig.h
        vendor/imgui/imgui.h
        vendor/imgui/imgui_internal.h
        vendor/imgui/imstb_rectpack.h
        vendor/imgui/imstb_textedit.h
        vendor/imgui/imstb_truetype.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb PREFIX "stb"
    FILES 
        vendor/stb/stb_image.h

)
