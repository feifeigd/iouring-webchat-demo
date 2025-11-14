cmake_minimum_required(VERSION 3.20)

project(iouring-webchat 
    VERSION 25.11.12.0 
    LANGUAGES CXX)

# 禁止直接在源码目录编译, 要放在 project指令之后
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(FATAL_ERROR "You cannot build in a source directory (or any directory with a  CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 更新子模块
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	message(STATUS "git path ${GIT_EXECUTABLE}")
	execute_process(COMMAND ${GIT_EXECUTABLE}
		submodule update --init --recursive		 
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"	# 工作目录
		RESULT_VARIABLE GIT_SUBMOD_RESULT
	)
	if(NOT GIT_SUBMOD_RESULT EQUAL "0")
		message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
	endif()
endif()

# 编译器特定标志
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.1)
        message(FATAL_ERROR "GCC version >= 10.1 required for jthread support")
    endif()
    add_compile_options(-Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
        message(FATAL_ERROR "Clang version >= 10.0 required for jthread support")
    endif()
    add_compile_options(-Wall -Wextra -pedantic -std=c++23)
endif()
# 根据构建类型定义宏
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build configured")
    add_compile_definitions(DEBUG=1)
    add_compile_definitions(_DEBUG=1)  # 某些平台需要
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
else()
    message(STATUS "Release build configured")
    add_compile_definitions(NDEBUG=1)
endif()

# 测试
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	#include(CTest)	# BUILD_TESTING=ON
	enable_testing()
	include(GoogleTest)
	add_subdirectory(tests)
endif()

add_subdirectory(src)
