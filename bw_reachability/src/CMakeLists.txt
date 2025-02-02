set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../out)


add_library(tofn impl/TableOfNodes.cpp)
add_library(trans impl/Transducer.cpp)
add_library(bw impl/BackwardsReachability.cpp)
add_library(bw_test impl/benchmarking/BackwardsReachabilityTesting.cpp)
add_library(parser impl/MISTparser/parser.cpp)

add_executable(testTable impl/tests/testTable.cpp)
target_link_libraries(testTable tofn)
target_link_libraries(testTable trans)

add_executable(testTrans impl/tests/testTransducer.cpp)
target_link_libraries(testTrans trans)
# # target_link_libraries(testTrans tofn)

# add_executable(testMist impl/MISTparser/parser.cpp)

add_executable(testPre impl/tests/testPre.cpp)
target_link_libraries(testPre tofn)
target_link_libraries(testPre trans)

add_executable(testPetriPre impl/tests/testPetriPre.cpp)
target_link_libraries(testPetriPre tofn)
target_link_libraries(testPetriPre trans)

add_executable(testBW impl/tests/testBW.cpp)
target_link_libraries(testBW tofn)
target_link_libraries(testBW trans)
target_link_libraries(testBW bw)

add_executable(benchmarks impl/benchmarking/simpleBenchmarks.cpp)
target_link_libraries(benchmarks tofn)
target_link_libraries(benchmarks trans)
target_link_libraries(benchmarks bw)
target_link_libraries(benchmarks parser)

add_executable(sanity impl/correctness/SanityChecker.cpp)
target_link_libraries(sanity tofn)
target_link_libraries(sanity trans)
target_link_libraries(sanity bw)
target_link_libraries(sanity parser)

add_executable(petrimat impl/petrimat.cpp)
target_link_libraries(petrimat tofn)
target_link_libraries(petrimat trans)
target_link_libraries(petrimat bw)
target_link_libraries(petrimat parser)
set_property(TARGET petrimat PROPERTY COMPILE_FLAGS "-Ofast -march=native")

add_executable(benchmarkDataIter impl/benchmarking/autoBenchmarksWithData.cpp)
target_link_libraries(benchmarkDataIter tofn)
target_link_libraries(benchmarkDataIter trans)
target_link_libraries(benchmarkDataIter bw_test)
target_link_libraries(benchmarkDataIter parser)

add_executable(testParser impl/MISTparser/testParser.cpp)
target_link_libraries(testParser parser)