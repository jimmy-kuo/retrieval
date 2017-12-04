set(EXEC_TEST_READ testRead.cpp)
add_executable(testRead ${EXEC_TEST_READ})

set(EXEC_TEST testRetrieval.cpp)
add_executable(testRetrieval ${EXEC_TEST})

set(EXEC_TEST_MEM testMem.cpp)
add_executable(testMem ${EXEC_TEST_MEM})

set(EXEC_TEST_ATTR_WITH_FEATURE FeatureWithAttrTest.cpp)
add_executable(FeatureWithAttrTest ${EXEC_TEST_MEM})

target_link_libraries(
        FeatureWithAttrTest
        libretrieval.so
        libfaiss.a
        libopenblas.so.0
        libfeatureSql.so
        libfeature.so
        libmysqlclient.so
        libmysqlpp.so
        libopencv_core.so
        libopencv_imgcodecs.so
        libopencv_imgproc.so
        libboost_system.so
        libboost_filesystem.so
        ${PROJECT_SOURCE_DIR}/caffe/build/lib/libcaffe.so
)

target_link_libraries(
        testRead
        libretrieval.so
        libfaiss.a
        libopenblas.so.0
)

target_link_libraries(
        testRetrieval
        libretrieval.a
        libfaiss.a
        libopenblas.so.0
)

target_link_libraries(
        testMem
        libretrieval.a
        libfaiss.a
        libopenblas.so.0
)