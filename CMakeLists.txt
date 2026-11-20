
cmake_minimum_required(VERSION 3.20)

project(OptionLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(OptionLab
    src/main.cpp

    src/Amer/Binomial_Amer.cpp
    src/Amer/Default_Amer.cpp
    src/Amer/FDM_Amer.cpp

    src/Asian/Default_Asian.cpp
    src/Asian/MonteCarlo_Asian.cpp

    src/Euro/Binomial_Eu.cpp
    src/Euro/ClosedForm_Eu.cpp
    src/Euro/Default_Eu.cpp
    src/Euro/FDM_Eu.cpp
    src/Euro/MonteCarlo_Eu.cpp

    src/Tests/Tests_Amer.cpp
    src/Tests/Tests_Asian.cpp
    src/Tests/Tests_Eu.cpp
    src/Tests/TestStructure.cpp
)

target_include_directories(OptionLab PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(OptionLab PRIVATE
    "OPTIONLAB_PROJECT_DIR=\"${PROJECT_SOURCE_DIR}\""
)
