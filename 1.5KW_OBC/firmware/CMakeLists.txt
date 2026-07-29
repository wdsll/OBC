cmake_minimum_required(VERSION 3.16)
project(obc_1p5kw_initial_fw C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(fw_core STATIC
    src/app.c
    src/can_service.c
    src/safety_manager.c
)
target_include_directories(fw_core PUBLIC include)
target_compile_options(fw_core PRIVATE -Wall -Wextra -Werror)

enable_testing()
add_executable(test_safety_manager
    tests/test_safety_manager.c
    tests/mock_board.c
)
target_include_directories(test_safety_manager PRIVATE include tests)
target_link_libraries(test_safety_manager PRIVATE fw_core)
target_compile_options(test_safety_manager PRIVATE -Wall -Wextra -Werror)
add_test(NAME safety_manager COMMAND test_safety_manager)
