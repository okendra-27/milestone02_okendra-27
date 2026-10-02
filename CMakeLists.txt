cmake_minimum_required(VERSION 3.16)
project(ai_workspace_m2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(aiws_m2
    src/document.cpp
    src/workspace.cpp
    src/text_processor.cpp
    src/chunker.cpp
    src/corpus_index.cpp
    src/retrieval_engine.cpp
    src/context_builder.cpp
    src/processing_core.cpp
)
target_include_directories(aiws_m2 PUBLIC include)
target_compile_options(aiws_m2 PRIVATE -Wall -Wextra -Wpedantic)

add_executable(public_tests tests/public_tests.cpp)
target_link_libraries(public_tests PRIVATE aiws_m2)
target_compile_options(public_tests PRIVATE -Wall -Wextra -Wpedantic)

enable_testing()
add_test(NAME public_tests COMMAND $<TARGET_FILE:public_tests>)

# Student-written tests. Add student-authored tests to tests/student_tests.cpp.
# Do not modify the course-provided tests/public_tests.cpp.
add_executable(student_tests tests/student_tests.cpp)
target_link_libraries(student_tests PRIVATE aiws_m2)
target_compile_options(student_tests PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME student_tests COMMAND $<TARGET_FILE:student_tests>)
