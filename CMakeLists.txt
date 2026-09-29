cmake_minimum_required(VERSION 3.28)
project(CognitaOS VERSION 2.0.0 LANGUAGES C CXX)

# ========================================================================
# Options
# ========================================================================

option(COGNITA_BUILD_TESTS "Build tests" ON)
option(COGNITA_BUILD_EXAMPLES "Build examples" ON)
option(COGNITA_BUILD_BENCHMARKS "Build benchmarks" ON)
option(COGNITA_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(COGNITA_ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(COGNITA_ENABLE_LTO "Enable Link-Time Optimization" ON)

# ========================================================================
# C++ Standard
# ========================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ========================================================================
# Compiler Flags
# ========================================================================

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror=return-type
    -Werror=uninitialized
    -Werror=unused-result
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
    if(COGNITA_ENABLE_ASAN)
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
    if(COGNITA_ENABLE_TSAN)
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
    endif()
else()
    add_compile_options(-O3 -DNDEBUG)
    if(COGNITA_ENABLE_LTO)
        add_compile_options(-flto)
        add_link_options(-flto)
    endif()
endif()

# ========================================================================
# Include Directories
# ========================================================================

include_directories(${CMAKE_SOURCE_DIR}/include)

# ========================================================================
# Source Files
# ========================================================================

set(COGNITA_KERNEL_SOURCES
    src/kernel/attention_scheduler.c
    src/kernel/semantic_memory.c
    src/kernel/context_switch.c
    src/kernel/kernel_evolution.c
)

set(COGNITA_COMPILER_SOURCES
    src/compiler/intent_compiler.c
)

set(COGNITA_RUNTIME_SOURCES
    src/runtime/agent.c
    src/runtime/capability.c
)

set(COGNITA_HAL_SOURCES
    src/hal/crystal.c
    src/hal/photonic.c
    src/hal/zerocopy.c
)

set(COGNITA_ALL_SOURCES
    ${COGNITA_KERNEL_SOURCES}
    ${COGNITA_COMPILER_SOURCES}
    ${COGNITA_RUNTIME_SOURCES}
    ${COGNITA_HAL_SOURCES}
)

# ========================================================================
# Library
# ========================================================================

add_library(cognita SHARED ${COGNITA_ALL_SOURCES})
target_include_directories(cognita PUBLIC ${CMAKE_SOURCE_DIR}/include)
set_target_properties(cognita PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "include/cognita/os.h;include/cognita/kernel.h;include/cognita/compiler.h;include/cognita/runtime.h;include/cognita/hal.h"
)

# ========================================================================
# Tests
# ========================================================================

if(COGNITA_BUILD_TESTS)
    enable_testing()

    # Unit tests
    add_executable(test_attention_scheduler tests/unit/test_attention_scheduler.c)
    target_link_libraries(test_attention_scheduler cognita)
    add_test(NAME attention_scheduler COMMAND test_attention_scheduler)

    add_executable(test_semantic_memory tests/unit/test_semantic_memory.c)
    target_link_libraries(test_semantic_memory cognita)
    add_test(NAME semantic_memory COMMAND test_semantic_memory)

    add_executable(test_context_switch tests/unit/test_context_switch.c)
    target_link_libraries(test_context_switch cognita)
    add_test(NAME context_switch COMMAND test_context_switch)

    add_executable(test_intent_compiler tests/unit/test_intent_compiler.c)
    target_link_libraries(test_intent_compiler cognita)
    add_test(NAME intent_compiler COMMAND test_intent_compiler)

    # Integration tests
    add_executable(test_agent_lifecycle tests/integration/test_agent_lifecycle.c)
    target_link_libraries(test_agent_lifecycle cognita)
    add_test(NAME agent_lifecycle COMMAND test_agent_lifecycle)

    add_executable(test_capability_chain tests/integration/test_capability_chain.c)
    target_link_libraries(test_capability_chain cognita)
    add_test(NAME capability_chain COMMAND test_capability_chain)

    add_executable(test_semantic_store tests/integration/test_semantic_store.c)
    target_link_libraries(test_semantic_store cognita)
    add_test(NAME semantic_store COMMAND test_semantic_store)

    # Benchmarks
    if(COGNITA_BUILD_BENCHMARKS)
        add_executable(bench_context_switch tests/benchmarks/bench_context_switch.c)
        target_link_libraries(bench_context_switch cognita)

        add_executable(bench_semantic_query tests/benchmarks/bench_semantic_query.c)
        target_link_libraries(bench_semantic_query cognita)

        add_executable(bench_intent_compile tests/benchmarks/bench_intent_compile.c)
        target_link_libraries(bench_intent_compile cognita)
    endif()
endif()

# ========================================================================
# Examples
# ========================================================================

if(COGNITA_BUILD_EXAMPLES)
    add_executable(hello_intent examples/hello_intent.c)
    target_link_libraries(hello_intent cognita)

    add_executable(agent_orchestration examples/agent_orchestration.c)
    target_link_libraries(agent_orchestration cognita)

    add_executable(semantic_storage examples/semantic_storage.c)
    target_link_libraries(semantic_storage cognita)

    add_executable(capability_chain examples/capability_chain.c)
    target_link_libraries(capability_chain cognita)
endif()

# ========================================================================
# Install
# ========================================================================

install(TARGETS cognita
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/cognita
)

install(DIRECTORY include/cognita DESTINATION include)

# ========================================================================
# Print Configuration
# ========================================================================

message(STATUS "")
message(STATUS "CognitaOS Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Tests: ${COGNITA_BUILD_TESTS}")
message(STATUS "  Examples: ${COGNITA_BUILD_EXAMPLES}")
message(STATUS "  Benchmarks: ${COGNITA_BUILD_BENCHMARKS}")
message(STATUS "  ASAN: ${COGNITA_ENABLE_ASAN}")
message(STATUS "  TSAN: ${COGNITA_ENABLE_TSAN}")
message(STATUS "  LTO: ${COGNITA_ENABLE_LTO}")
message(STATUS "")
