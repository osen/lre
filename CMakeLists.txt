cmake_minimum_required(VERSION 2.6)
project(LRE C)

if(UNIX)
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -Wall -Werror -pedantic")
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -ffast-math")
else()
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/src
)

if(WIN32)
  include_directories(
    ${PROJECT_SOURCE_DIR}/contrib/msvc/include
  )
  link_directories(
    ${PROJECT_SOURCE_DIR}/contrib/msvc/lib
  )
else()
  include_directories(
    /usr/local/include
    /usr/X11R6/include
  )
  link_directories(
    /usr/local/lib
    /usr/X11R6/lib
  )
endif()

add_library(plank
  src/plank/palloc.c
  src/plank/sstream.c
  src/plank/vector.c
)

add_executable(horde
  src/horde/main.c
)

target_link_libraries(horde plank)

add_library(gluten
  src/gluten/gluten.c
)

target_link_libraries(gluten GLEW GLU glut GL plank)

add_library(platform
  src/platform/platform.c
  src/platform/Actor.c
  src/platform/Environment.c
  src/platform/Cache.c
  src/platform/Mesh.c
  src/platform/math.c
  src/platform/Gui.c
  src/platform/Camera.c
  src/platform/util.c
  src/platform/Debug.c
  src/platform/Color.c
  src/platform/Input.c

  src/platform/Image.c
  src/platform/Animation.c
  src/platform/Font.c
  src/platform/Model.c

  src/platform/ModelRenderer.c
  src/platform/Transform.c
  src/platform/BoxCollider.c
  src/platform/ModelCollider.c
  src/platform/Gravity.c

  src/platform/contrib/lodepng.c
  src/platform/contrib/parson.c
  src/platform/contrib/raytri.c
  src/platform/contrib/tritri.c
  src/platform/contrib/tribox.c

  src/platform/h.out.c
)

add_executable(lre
  src/lre/main.c
  src/lre/RoomScreen.c
  src/lre/Player.c
  src/lre/Room.c
  src/lre/CameraManager.c
  src/lre/ScriptManager.c
  src/lre/Door.c
  src/lre/TransitionScreen.c
)

#target_link_libraries(platform plank glew32 freeglut)
target_link_libraries(platform plank m gluten)
target_link_libraries(lre platform)

