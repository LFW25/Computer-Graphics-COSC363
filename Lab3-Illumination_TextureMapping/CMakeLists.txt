cmake_minimum_required(VERSION 2.8)

project(lab3)

add_executable(Spaceship.out I_Spaceship/Spaceship.cpp)
add_executable(Earth.out II_Earth/Earth.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( Spaceship.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
target_link_libraries( Earth.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
