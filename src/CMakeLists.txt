set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/modules/")

find_package(Magnum REQUIRED
    Shaders
    MeshTools
    Primitives
    TgaImporter
    Sdl2Application)

find_package(Corrade REQUIRED Utility)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_resource(TexturedTriangle_RESOURCES ${PROJECT_SOURCE_DIR}/res/resources.conf)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

add_executable(MyApplication
    MyApplication.cpp
    TexturedTriangleShader.cpp
    TexturedTriangleShader.h
    ${TexturedTriangle_RESOURCES})

target_include_directories(MyApplication PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(MyApplication
    Magnum::Application
    Magnum::Shaders
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::TgaImporter
    Magnum::Magnum)
