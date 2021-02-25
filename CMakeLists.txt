cmake_minimum_required(VERSION 3.2)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
   set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
endif()

# Name of the project
project(ase-cgitech-JoshhhBailey-Build)
# This is the name of the Exe change this and it will change everywhere
set(TargetName ase-cgitech-JoshhhBailey)
# Instruct CMake to run moc automatically when needed (Qt projects only)
set(CMAKE_AUTOMOC ON)
# Run the uic tools and search in the ui folder
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${PROJECT_SOURCE_DIR}/ui)
# find Qt libs
find_package(Qt5Widgets)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(OpenImageIO CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
# Windows specific packages
find_package(IlmBase CONFIG REQUIRED)
find_package(OpenEXR CONFIG REQUIRED)

# use C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
# Set the name of the executable we want to build
add_executable(${TargetName})

# Add NGL include path
include_directories(include $ENV{HOME}/NGL/include)
target_sources(${TargetName} PRIVATE
      # .cpp
      ${PROJECT_SOURCE_DIR}/src/main.cpp
			${PROJECT_SOURCE_DIR}/src/NGLScene.cpp  
			${PROJECT_SOURCE_DIR}/src/NGLSceneMouseControls.cpp  
      ${PROJECT_SOURCE_DIR}/src/ImageStack.cpp
      ${PROJECT_SOURCE_DIR}/src/Mesh.cpp
      ${PROJECT_SOURCE_DIR}/src/Table.cpp
      ${PROJECT_SOURCE_DIR}/src/Camera.cpp
      ${PROJECT_SOURCE_DIR}/src/Timer.cpp
      ${PROJECT_SOURCE_DIR}/src/MainWindow.cpp
      # .h
      ${PROJECT_SOURCE_DIR}/include/WindowParams.h
      ${PROJECT_SOURCE_DIR}/include/NGLScene.h
      ${PROJECT_SOURCE_DIR}/include/ImageStack.h
      ${PROJECT_SOURCE_DIR}/include/Mesh.h
      ${PROJECT_SOURCE_DIR}/include/Table.h
      ${PROJECT_SOURCE_DIR}/include/Camera.h
      ${PROJECT_SOURCE_DIR}/include/Timer.h
      ${PROJECT_SOURCE_DIR}/include/MainWindow.h
      #.glsl
      ${PROJECT_SOURCE_DIR}/shaders/PBRFragment.glsl
      ${PROJECT_SOURCE_DIR}/shaders/PBRVertex.glsl
)
add_compile_definitions(ADDLARGEMODELS)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# see what platform we are on and set platform defines
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message("Mac build")
  find_library(MACGL OpenGL)
  find_package(IlmBase CONFIG REQUIRED)
	find_package(OpenEXR CONFIG REQUIRED)
	set(EXTRALIBS  ${MACGL})
	target_link_libraries(${TargetName} PRIVATE  IlmBase::Iex IlmBase::Half IlmBase::Imath IlmBase::IexMath)
	target_link_libraries(${TargetName} PRIVATE OpenEXR::IlmImf OpenEXR::IlmImfUtil OpenEXR::IlmImfConfig)
	add_definitions(-DGL_SCILENCE_DEPRICATION)
	set ( PROJECT_LINK_LIBS ${MACGL} NGL)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set ( PROJECT_LINK_LIBS -lGL NGL)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	include_directories(include $ENV{HOMEDRIVE}/$ENV{HOMEPATH}/NGL/include)
	link_directories($ENV{HOMEDRIVE}/$ENV{HOMEPATH}/NGL/lib)	
	target_link_libraries(${TargetName} PRIVATE $ENV{HOMEDRIVE}/$ENV{HOMEPATH}/NGL/lib/NGL.lib)
	add_compile_definitions(_USE_MATH_DEFINES)
	add_compile_definitions(NOMINMAX)
	target_compile_definitions(${TargetName} PRIVATE /wd4251)
	# Need to define this when building shared library or suffer dllimport errors
	add_compile_definitions(BUILDING_DLL)
endif()

# now add NGL specific values
target_link_directories(${TargetName} PRIVATE $ENV{HOME}/NGL/lib )

# add exe and link libs that must be after the other defines
target_link_libraries(${TargetName} PRIVATE OpenImageIO::OpenImageIO OpenImageIO::OpenImageIO_Util)
target_link_libraries(${TargetName} PRIVATE ${PROJECT_LINK_LIBS}  Qt5::Widgets fmt::fmt-header-only freetype)

# Copy folders to .exe directory
add_custom_target(CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${TargetName}>/shaders
)

#################################################################################
# Testing code
#################################################################################

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
enable_testing()
add_executable(Tests)
target_link_directories( Tests PRIVATE $ENV{HOME}/NGL/lib )
target_sources(Tests PRIVATE tests/Tests.cpp src/Table.cpp src/Camera.cpp src/ImageStack.cpp src/Mesh.cpp )
target_link_libraries(Tests PRIVATE GTest::gtest GTest::gtest_main NGL Qt5::Widgets)
gtest_discover_tests(Tests)
