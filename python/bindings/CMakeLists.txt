# PFC Python Bindings

# Collect all binding source files
set(BINDING_SOURCES
    module.cpp
    core_bindings.cpp
    codec_bindings.cpp
    error_handling.cpp
)

# Create the pybind11 module
pybind11_add_module(_pfc ${BINDING_SOURCES})

# Include PFC headers
target_include_directories(_pfc PRIVATE
    ${PFC_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler flags for optimization
target_compile_options(_pfc PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/O2 /W4>
)

# Install the module
install(TARGETS _pfc
        LIBRARY DESTINATION ${Python3_SITEARCH}/pfc_python)
