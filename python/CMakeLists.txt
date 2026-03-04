cmake_minimum_required(VERSION 3.15)
project(pfc_python VERSION 1.0.0 LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type defaults to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find Python and pybind11
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)

# Try to get pybind11 cmake path from pip installation
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE PYBIND11_RESULT
)
if(PYBIND11_RESULT EQUAL 0)
    list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_CMAKE_DIR}")
endif()

find_package(pybind11 CONFIG REQUIRED)

# Path to PFC headers (parent directory — bindings live inside the pfc repo)
set(PFC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../include")

# Verify PFC headers exist
if(NOT EXISTS "${PFC_INCLUDE_DIR}/pfc/pfc.hpp")
    message(FATAL_ERROR "PFC headers not found at ${PFC_INCLUDE_DIR}. "
                        "Please ensure you are building from within the pfc/python/ directory.")
endif()

# Add bindings subdirectory
add_subdirectory(bindings)

# Install Python package
install(DIRECTORY src/pfc_python/
        DESTINATION ${Python3_SITEARCH}/pfc_python)
