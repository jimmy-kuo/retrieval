
set(EXEC_TEST_READ testRead.cpp)
add_executable(testRead ${EXEC_TEST_READ})

set(LIB_RETRIEVAL Retrieval.cpp)
add_library(retrieval SHARED ${LIB_RETRIEVAL})

set(EXEC_TEST main.cpp)
add_executable(main ${EXEC_TEST})

target_link_libraries(
        testRead
        libretrieval.so
        libfaiss.a
        libopenblas.so.0
)

target_link_libraries(
        main
        libretrieval.so
        libfaiss.a
        libopenblas.so.0
)