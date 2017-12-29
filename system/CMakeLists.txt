find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package( Boost COMPONENTS thread filesystem system REQUIRED )

find_package(CUDA REQUIRED)
# SET(OpenCV_DIR /usr/local/share/OpenCV/OpenCVConfig.cmake)
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND) # if not OpenCV 3.x, then imgcodecs are not found
    message("opencv found ${OpenCV_INCLUDE_DIRS} ")
endif()


set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
        /usr/local/include
        /usr/include
        /share/local/included
        /usr/local/cuda/include
        ${PROJECT_SOURCE_DIR}/caffe/include
        ${PROJECT_SOURCE_DIR}/caffe/build/include
        ${PROJECT_SOURCE_DIR}/faiss/
        ${PROJECT_SOURCE_DIR}/faiss/gpu/utils
        /usr/local/cuda/include
)
link_directories(
        ${PROJECT_SOURCE_DIR}/lib
        ${PROJECT_SOURCE_DIR}/faiss
        ${PROJECT_SOURCE_DIR}/faiss/gpu/
        /usr/lib
        /usr/lib/mysql
        /usr/lib/x86_64-linux-gnu/
        /share/local/lib
        /usr/local/lib
        ${PROJECT_SOURCE_DIR}/caffe/build/lib
)

add_executable(AddData AddData.cpp)
target_link_libraries(
        AddData
        gpufaiss
        libfaiss.a
        libfeature.so
        pthread
        libboost_system.so
        libboost_filesystem.so
        libboost_thread.so
        libopencv_core.so
        libopencv_imgcodecs.so
        libopencv_imgproc.so
        libopenblas.so.0
        ${PROJECT_SOURCE_DIR}/caffe/build/lib/libcaffe.so
        libglog.so
        libcudart.so
        libcublas.so
)

#set(SOURCE_SYSTEM IdmSystem.cpp)
#add_executable(IdmSystem ${SOURCE_SYSTEM})
#
#set(SOURCE_MAP_SYSTEM mapSystem.cpp)
#add_executable(mapSystem ${SOURCE_MAP_SYSTEM})
#
#target_link_libraries(
#        IdmSystem
#        gpufaiss
#        libfaiss.a
#        libfeature.so
#        pthread
#        libboost_system.so
#        libboost_filesystem.so
#        libboost_thread.so
#        libopencv_core.so
#        libopencv_imgcodecs.so
#        libopencv_imgproc.so
#        libopenblas.so.0
#        ${PROJECT_SOURCE_DIR}/caffe/build/lib/libcaffe.so
#        libglog.so
#        libcudart.so
#        libcublas.so
#)
#
#target_link_libraries(
#        mapSystem
#        gpufaiss
#        libfaiss.a
#        libbinary.so
#        libfeature.so
#        pthread
#        libboost_system.so
#        libboost_filesystem.so
#        libboost_thread.so
#        libopencv_core.so
#        libopencv_imgcodecs.so
#        libopencv_imgproc.so
#        libopenblas.so.0
#        ${PROJECT_SOURCE_DIR}/caffe/build/lib/libcaffe.so
#        libglog.so
#        libcudart.so
#        libcublas.so
#)
#
#
#
#
#
#
#
#
#




