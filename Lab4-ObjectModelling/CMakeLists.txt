cmake_minimum_required(VERSION 2.8)

project(lab4)

add_executable(Walls.out I_Walls/Walls.cpp)
add_executable(Vase.out II_Vase/Vase.cpp)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( Walls.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
target_link_libraries( Vase.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

