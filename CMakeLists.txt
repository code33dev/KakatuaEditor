cmake_minimum_required(VERSION 3.16)
project(KakatuaEditor)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable automatic handling of Qt MOC, UI, and resource files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Gather source files
set(SOURCES
    main.cpp
    TextEditor.cpp
    SyntaxHighlighter.cpp
    FindWindow.cpp
    ReplaceWindow.cpp
    FindInFilesWindow.cpp
    ReplaceInFilesWindow.cpp
)

# Gather header files
set(HEADERS
    TextEditor.hpp
    SyntaxHighlighter.hpp
    FindWindow.hpp
    ReplaceWindow.hpp
    FindInFilesWindow.hpp
    ReplaceInFilesWindow.hpp
)

# Add the executable
add_executable(KakatuaEditor ${SOURCES} ${HEADERS})

# Link the necessary Qt6 libraries
target_link_libraries(KakatuaEditor Qt6::Widgets Qt6::Core Qt6::Gui)
