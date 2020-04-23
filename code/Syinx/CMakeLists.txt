cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

message (STATUS "Syinx Version:0.7.0 ")
message (STATUS "Syinx Project Compiles , You can add this project file to compile together ")


 
#set project name
set(TARGET_NAME gameserver)

project(${TARGET_NAME})

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#项目生成的flags Debug = 0; Release = 1
set(PROJECT_FLAGS 0)
set(CMAKE_BUILD_TYPE_STRING )
if (("${CMAKE_BUILD_TYPE}" STREQUAL "debug") OR
	("${CMAKE_BUILD_TYPE}" STREQUAL "Debug") OR
	("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG") 
	)
	set(PROJECT_FLAGS 0)
	set(CMAKE_BUILD_TYPE_STRING Debug)
	else()
	set(PROJECT_FLAGS 1)
	set(CMAKE_BUILD_TYPE_STRING Release)
endif()

#设置库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

#set project cpp path
set(PROJECT_FILE
	./pkg/easylog/easylogging++.cc

	./pkg/inih/ini.c

	./pkg/inih/cpp/INIReader.cpp

	./Syinx/SConnect.cpp

	./Syinx/Syinx.cpp

	./Syinx/SyPthreadPool.cpp

	./Syinx/SyResAdapter.cpp

	./Syinx/SyTaskAdapter.cpp

	./Player/CPlayer.cpp

	main.cpp
)

function(GNUCFunction)
	set(PROJECT_INCLUDE
		./pkg/easylog/easylogging++.h

		./pkg/inih/ini.h

		./pkg/inih/cpp/INIReader.h

		./Syinx/include/GameObject.h

		./Syinx/include/SConnect.h

		./Syinx/include/Tools.hpp

		./Syinx/include/SyInc.h

		./Syinx/include/Syinx.h

		./Syinx/include/SyPthreadPool.h

		./Syinx/include/SyResAdapter.h

		./Syinx/include/SyTaskAdapter.h

		./Player/CPlayer.h
	)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_BUILD_TYPE_STRING})
	message(STATUS "OUTPUT: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

	link_libraries(-lpthread)
	set(PROJECT_OTHER_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Shard/llib)
	message("INCLUDE : ${PROJECT_INCLUDE}\n")

	message("CPP FILE :${PROJECT_FILE}\n")

	message(STATUS "Shard Path: ${PROJECT_OTHER_LIB_PATH}")
	add_executable(${TARGET_NAME} ${PROJECT_FILE})

	target_link_libraries(${TARGET_NAME} ${PROJECT_OTHER_LIB_PATH}/libevent.a)

	install(TARGETS ${TARGET_NAME} DESTINATION)

	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Config/Config.ini 
	DESTINATION 
			  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_BUILD_TYPE_STRING}/)

	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Config/Syinx-Server.logger.conf 
	DESTINATION 
			  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_BUILD_TYPE_STRING}/)
endfunction()

function(CLANGFunction)

endfunction()

function(MSVCFunction)
	set(PROJECT_INCLUDE
		./pkg/easylog/easylogging++.h

		./pkg/inih/ini.h

		./pkg/inih/cpp/INIReader.h

		./Syinx/include/GameObject.h

		./Syinx/include/SConnect.h

		./Syinx/include/Tools.hpp

		./Syinx/include/SyInc.h

		./Syinx/include/Syinx.h

		./Syinx/include/SyPthreadPool.h

		./Syinx/include/SyResAdapter.h

		./Syinx/include/SyTaskAdapter.h

		./Player/CPlayer.h
	)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/)
	message(STATUS "OUTPUT: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	add_definitions(-D WIN32)
	add_definitions(-D HAVE_STRUCT_TIMESPEC)
	add_definitions(-D _CRT_SECURE_NO_WARNINGS)
	add_definitions(-D _WINSOCK_DEPRECATED_NO_WARNINGS)
	add_definitions(-W3)
	add_definitions(-MTd)
	
	include_directories(
	${PROJECT_SOURCE_DIR}/../../Shard/include/pthread
	${PROJECT_SOURCE_DIR}/../../Shard/include/libevent-master
	${PROJECT_SOURCE_DIR}/../../Shard/include/libevent-master/include
	${PROJECT_SOURCE_DIR}/../../Shard/include/libevent-master/build/include
	)
	message(STATUS "${PROJECT_SOURCE_DIR}")
	add_executable(${TARGET_NAME} ${PROJECT_FILE} ${PROJECT_INCLUDE})
	message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/../../")
	target_link_libraries(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../Shard/wlib/pthread/pthreadVC2.lib)

	target_link_libraries(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../Shard/wlib/libevent/${CMAKE_BUILD_TYPE_STRING}/event.lib)
	target_link_libraries(${TARGET_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../Shard/wlib/libevent/${CMAKE_BUILD_TYPE_STRING}/event_core.lib)
	target_link_libraries(${TARGET_NAME} Ws2_32.lib)


	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Config/Config.ini 
	DESTINATION 
			  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_BUILD_TYPE_STRING}/)

	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Config/Syinx-Server.logger.conf 
	DESTINATION 
			  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_BUILD_TYPE_STRING}/)

	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Shard/wlib/libevent/${CMAKE_BUILD_TYPE_STRING}/event.dll
	DESTINATION 
			  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_BUILD_TYPE_STRING}/)

	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Shard/wlib/libevent/${CMAKE_BUILD_TYPE_STRING}/event_core.dll
	DESTINATION 
			  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_BUILD_TYPE_STRING}/)
	
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../../Shard/wlib/pthread/pthreadVC2.dll
	DESTINATION 
			  ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_BUILD_TYPE_STRING}/)

	
endfunction()

set(GNUC 0)
set(CLANG 0)
set(MSVC 0)
if (("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang") OR
    ("${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang"))
    set(CLANG 1)
endif()
if (("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU") OR (${CLANG}))
    set(GNUC 1)
endif()
if (("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC") OR ("${CMAKE_C_SIMULATE_ID}" STREQUAL "MSVC"))
    set(MSVC 1)
endif()

if(${GNUC})
	message(STATUS "GNUC")
	GNUCFunction()
endif()
if(${CLANG})
	message(STATUS "CLANG")
	CLANGFunction()
endif()
if(${MSVC})
	message(STATUS "MSVC")
	MSVCFunction()
endif()