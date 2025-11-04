CMAKE_MINIMUM_REQUIRED(VERSION 3.28)

file(GLOB_RECURSE SRC_LIST
    "./*.c"
)

if (APPLE)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/mac)
endif()

if (WIN32)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/win)
endif()

if (UNIX)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/linux)
endif()

ADD_EXECUTABLE(snake-c ${SRC_LIST})

if (APPLE)
    include_directories(/usr/local/include)

    # 查找 SDL3 库（关键步骤）
    find_package(SDL3 REQUIRED)
    target_link_libraries(snake-c PRIVATE SDL3::SDL3)

    # 查找 SDL3_image 库（关键步骤）
    find_package(SDL3_image REQUIRED)
    target_link_libraries(snake-c PRIVATE SDL3_image::SDL3_image)
endif()

if (WIN32)
    # 设置SDL3库路径
    set(SDL3_DIR "D:/local/SDL3")
        
    # 包含SDL3的头文件目录
    include_directories("${SDL3_DIR}/include")

    # 添加SDL3的库文件目录 - 确保这个路径包含实际的库文件
    link_directories("${SDL3_DIR}/lib")

    # 查找SDL3库文件并链接 - 处理MinGW的命名 convention
    find_library(SDL3_LIBRARY
        NAMES SDL3 libSDL3 SDL3.dll.a libSDL3.dll.a
        PATHS "${SDL3_DIR}/lib"
        NO_DEFAULT_PATH
    )
    target_link_libraries(snake-c PRIVATE ${SDL3_LIBRARY})

    # 复制SDL3.dll到输出目录
    add_custom_command(
        TARGET snake-c POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_DIR}/bin/SDL3.dll"
        $<TARGET_FILE_DIR:snake-c>
        COMMENT "Copying SDL3.dll to output directory"
    )

    # 添加glad头文件目录
    include_directories("D:/local/glad/include")

    # 添加cglm头文件目录
    include_directories("D:/local/cglm/include")

    # 设置SDL3_image库路径
    set(SDL3_IMAGE_DIR "D:/local/SDL3_image")

    # 包含SDL3_image的头文件目录
    include_directories("${SDL3_IMAGE_DIR}/include")
    
    # 添加SDL3_image的库文件目录
    link_directories("${SDL3_IMAGE_DIR}/lib")
    
    # 查找SDL3_image库文件并链接
    find_library(SDL3_IMAGE_LIBRARY
        NAMES SDL3_image libSDL3_image SDL3_image.dll.a libSDL3_image.dll.a
        PATHS "${SDL3_IMAGE_DIR}/lib"
        NO_DEFAULT_PATH
    )
    target_link_libraries(snake-c PRIVATE ${SDL3_IMAGE_LIBRARY})
    
    # 复制SDL3_image.dll到输出目录（如果有）
    if(EXISTS "${SDL3_IMAGE_DIR}/bin/SDL3_image.dll")
        add_custom_command(
            TARGET snake-c POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL3_IMAGE_DIR}/bin/SDL3_image.dll"
            $<TARGET_FILE_DIR:snake-c>
            COMMENT "Copying SDL3_image.dll to output directory"
        )
    endif()
endif()



