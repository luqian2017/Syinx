cmake_minimum_required(VERSION 3.02)

message (STATUS "Syinx Version:0.5.0 ")
message (STATUS "Syinx Project Compiles , You can add this project file to compile together ")


include_directories(
./module 
./Syinx 
./Sylog 
./Player
)

file(GLOB srcs 
./main.cpp 
./module/*.cpp 
./Syinx/*.cpp 
./Sylog/*.cpp 
./Sylog/*.cc 
./Player/*.cpp
)

LINK_DIRECTORIES(/usr/local/lib/)

add_executable(gameserver ${srcs})


TARGET_LINK_LIBRARIES (
gameserver 
-lpthread 
-levent
#-lprotobuf	需要利用protobuf添加lib库
-lmysqlclient
)