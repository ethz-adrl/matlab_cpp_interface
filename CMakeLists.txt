cmake_minimum_required(VERSION 2.8.3)
project(matlab_cpp_interface)

find_package(catkin REQUIRED COMPONENTS roscpp message_generation message_runtime cmake_modules)

set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Matlab REQUIRED QUIET)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)

if(${MATLAB_FOUND})

catkin_package(
   INCLUDE_DIRS include ${MATLAB_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR} ${Boost_INCLUDE_DIRS}
   LIBRARIES mxArrayWrapper matlabMatFile matlabEngine ${MATLAB_LIBRARIES}
)

include_directories(
  include
  test
  ${EIGEN3_INCLUDE_DIR}
  ${catkin_INCLUDE_DIRS}
  ${MATLAB_INCLUDE_DIR}
  )

add_library(mxArrayWrapper STATIC
    src/internal/MxArrayWrapper.cpp
    src/internal/MxArrayNDimWrapper.cpp
)

add_library(matlabMatFile STATIC
  src/MatFile.cpp
)
add_library(matlabEngine STATIC
  src/Engine.cpp
)

add_executable(matlabTest test/test_main.cpp)
add_executable(matlabROSTest test/ros_test_main.cpp)

target_link_libraries(matlabMatFile
    mxArrayWrapper
    ${MATLAB_LIBRARIES}
)

target_link_libraries(matlabEngine
    mxArrayWrapper
    ${MATLAB_LIBRARIES}
)

target_link_libraries(matlabTest
  matlabMatFile
  matlabEngine
  mxArrayWrapper
  ${MATLAB_LIBRARIES}
)

target_link_libraries(matlabROSTest
  matlabMatFile
  matlabEngine
  mxArrayWrapper
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES}
  ${MATLAB_LIBRARIES}
)

else(${MATLAB_FOUND})
    message(WARNING "MATLAB NOT FOUND, WILL NOT COMPILE THIS PACKAGE")
    catkin_package()
endif(${MATLAB_FOUND})
