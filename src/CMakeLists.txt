cmake_minimum_required(VERSION 3.20.0)
project(ImageMagickFileReader C)

add_library(${PROJECT_NAME} SHARED)

set(IMAGEMAGICK_INCLUDE_DIR CACHE PATH "ImageMagick include directory")
set(IMAGEMAGICK_LIBRARY_DIR CACHE PATH "ImageMagick library directory")

target_sources(${PROJECT_NAME} PRIVATE main.c main.def)
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".aui")
target_include_directories(${PROJECT_NAME} PRIVATE ${IMAGEMAGICK_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE ${IMAGEMAGICK_LIBRARY_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE CORE_RL_MagickCore_ CORE_RL_MagickWand_)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/LARGEADDRESSAWARE")
    target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX /source-charset:utf-8 /execution-charset:shift_jis)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    target_link_options(${PROJECT_NAME} PRIVATE -static-libstdc++ -static)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")