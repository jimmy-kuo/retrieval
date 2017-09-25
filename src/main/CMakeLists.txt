set(EXEC_TEST_READ testRead.cpp)
add_executable(testRead ${EXEC_TEST_READ})

set(EXEC_TEST main.cpp)
add_executable(main ${EXEC_TEST})

set(EXEC_TEST_MEM testMem.cpp)
add_executable(testMem ${EXEC_TEST_MEM})

target_link_libraries(
        testRead
        libretrieval.so
        libfaiss.a
        libopenblas.so.0
)

target_link_libraries(
        main
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