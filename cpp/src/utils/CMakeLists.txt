# utils/CMakeLists.txt

# Regular utilities
add_library(utils STATIC
    utils.hpp utils.cpp
    cli.hpp cli.cpp
)

# Utilities for interacting with ROOT files
add_library(root-utils STATIC root.cpp root.hpp)
target_link_libraries(root-utils utils ROOT::Core ROOT::RIO ROOT::Tree)
