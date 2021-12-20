cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 17)

project(Alpha_pars VERSION 0.2.0)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
if (CMAKE_COMPILER_IS_GNUCXX)
message ("Compiler set options...")
	add_compile_options(
        #-finput-charset=CP1251 #кодировка исходников
        #-fexec-charset=CP866 #кодировка исполняемого фала
        -Werror
        -pedantic-errors
        -Wall
        -Wextra
        -Wpedantic        
        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wctor-dtor-privacy
        -Wduplicated-branches
        -Wduplicated-cond
        -Wextra-semi
        -Wfloat-equal
        -Wlogical-op
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wsign-conversion
        -Wsign-promo
        )
else ()
	message ("Unknown compiler")
endif ()

set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)