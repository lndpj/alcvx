cmake_minimum_required( VERSION 3.10 )

project( alcvx VERSION 0.1 LANGUAGES C)
set(PROJECT_DESCRIPTION "Aligned C Vector Extension single-header math library")

include(GNUInstallDirs)
include(FindPkgConfig)

set( CMAKE_C_STANDARD 23 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_EXTENSIONS ON )

if( NOT CMAKE_BUILD_TYPE )
        set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

string(TOUPPER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_PROCESSOR_UPPER)
message(STATUS "Upper-case processor: ${SYSTEM_PROCESSOR_UPPER}")

cmake_policy(SET CMP0072 NEW)   #OpenGL_GL_PREFERENCE to GLVND

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
	add_compile_options(-Wno-gnu-alignof-expression)
endif()

if (CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "GNU" )
	if( NOT CMAKE_BUILD_OPTLEVEL )
		set( CMAKE_BUILD_OPTLEVEL 3 CACHE STRING "Choose the optimization level." FORCE )
	endif()
	add_compile_options(-O${CMAKE_BUILD_OPTLEVEL})
	if (NOT CMAKE_CROSSCOMPILING)
		add_compile_options(-march=native)

		if (${SYSTEM_PROCESSOR_UPPER} MATCHES "X64|X86_64|AMD64")
			add_compile_options(-mfpmath=sse)
		endif()
	endif()
	add_compile_options(-ftree-vectorize)
	add_compile_options(-fopenmp-simd)
	add_compile_options(-fdata-sections)
	add_compile_options(-fpermissive)
	add_compile_options(-ffunction-sections)
	add_compile_options(-fno-asynchronous-unwind-tables)
	if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
		add_link_options(-Wl,--gc-sections)
		add_link_options(-Wl,--print-gc-sections)
		add_link_options(-Wl,-s)
	endif()
else (CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
	if( NOT CMAKE_BUILD_OPTLEVEL )
		set( CMAKE_BUILD_OPTLEVEL 2 CACHE STRING "Choose the optimization level." FORCE )
	endif()
	add_compile_options(/O${CMAKE_BUILD_OPTLEVEL})
	if (NOT CMAKE_CROSSCOMPILING)
		if (${SYSTEM_PROCESSOR_UPPER} MATCHES "X64|X86_64|AMD64")
			add_compile_options(/Gv)
		endif()
	endif()
	add_compile_options(/openmp)
	if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
		add_link_options(/DEBUG:NONE)
	endif()
endif()

add_library( alcvx INTERFACE )
target_include_directories( alcvx INTERFACE
        PUBLIC_HEADER $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(FILES ${PROJECT_SOURCE_DIR}/include/alcvx.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT headers)

add_executable( example examples/example.c )
target_link_libraries( example alcvx )
target_include_directories( example INTERFACE
        PUBLIC_HEADER $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


  #add_library(alcvx-objlib OBJECT alcvx.c)
  #add_library(alcvx-shared SHARED)
  #set_target_properties(alcvx-shared PROPERTIES OUTPUT_NAME alcvx)
  #target_link_libraries(alcvx-shared PUBLIC alcvx-objlib)

  #add_library(alcvx-static STATIC)
  #set_target_properties(alcvx-static PROPERTIES OUTPUT_NAME alcvx)
  #target_link_libraries(alcvx-static PUBLIC alcvx-objlib)

