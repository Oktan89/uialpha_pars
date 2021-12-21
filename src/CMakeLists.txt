cmake_minimum_required(VERSION 3.0.0)

set(MAIN_SOURCES main.cpp getdate.cpp logreader.cpp logparser.cpp database.cpp mainwin.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


#----------------Qt-------------------------
if(WIN32)
  message("Version Qt for Windows")
  set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/mingw81_64/lib/cmake")
elseif(UNIX)
  message("Version Qt for Linux")
  set(CMAKE_PREFIX_PATH "/home/oktan/Qt/5.15.2/gcc_64/lib/cmake")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

message("Looking for Qt...")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

if(${Qt5_FOUND})
    message("Found Qt [${Qt5_VERSION}]")
else()
    message("Not Found Qt :(")
endif()

#qt5_generate_moc(main.cpp main.moc TARGET QtWeb)
qt5_wrap_cpp (MAIN_SOURCES ${MY_INCLUDE_DIR}/mainwin.h)
#qt5_wrap_cpp (MAIN_SOURCES ${MY_INCLUDE_DIR}/database.h)
#------------------------------------------

#---------------BOOST----------------------
if(WIN32)
  message("Version BOOST for Windows")
  set(Boost_INCLUDE_DIR "C:/boost/boost_1_78_0")
  set(Boost_LYBRARY_DIR "C:/boost_1_78_0/stage/lib")
elseif((UNIX))
  message("Version BOOST for Linux")
  set(Boost_INCLUDE_DIR "/usr/local/boost_1_77_0")
  set(Boost_LYBRARY_DIR "/usr/local/boost_1_77_0/stage/lib")
endif()

if(POLICY CMP0074)
  message("Set POLICY CMP0074 NEW FOR BOOST FIND PACKAGE")
  cmake_policy(SET CMP0074 NEW)
endif()

find_package(Boost COMPONENTS REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LYBRARY_DIR})
endif()
#-------------------------------------------

#---------------Thread STL------------------
#Подключаем потоки
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
#-------------------------------------------

add_executable(Alpha_pars ${MAIN_SOURCES})

target_link_libraries(Alpha_pars Qt5::Core Qt5::Widgets)
target_link_libraries(Alpha_pars ${Boost_LYBRARIES})
target_link_libraries(Alpha_pars Threads::Threads)

if(MINGW)
    message("SET LIBRARIES BOOST FOR MINGW WS2_32")
    target_link_libraries(Alpha_pars ws2_32)
endif()

target_include_directories(Alpha_pars PUBLIC ${MY_INCLUDE_DIR})

