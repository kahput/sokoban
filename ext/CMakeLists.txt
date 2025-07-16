add_subdirectory("glfw")
add_subdirectory("glad/cmake")
add_subdirectory("cglm")
add_subdirectory("miniaudio")
add_subdirectory("stb")

add_library("ext" INTERFACE)
target_link_libraries("ext" INTERFACE
        glfw
        glad_gl_core_45
        cglm
        miniaudio
        stb
)
target_include_directories("ext" INTERFACE "./")

# Specify glad settings
glad_add_library(glad_gl_core_45 REPRODUCIBLE API gl:core=4.5)
