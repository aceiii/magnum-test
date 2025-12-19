set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
#        ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_resource(TexturedTriangle_RESOURCES ${PROJECT_SOURCE_DIR}/res/resources.conf)

add_executable(MyApplication
        MyApplication.cpp
        TexturedTriangleShader.cpp
        TexturedTriangleShader.h
        ${TexturedTriangle_RESOURCES}
)

add_dependencies(MyApplication
        Magnum::Sdl2Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Trade
        Magnum::TgaImporter
)

target_link_libraries(MyApplication PRIVATE
        Magnum::Sdl2Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Trade
)

target_link_libraries(MyApplication INTERFACE
    Magnum::TgaImporter
)
