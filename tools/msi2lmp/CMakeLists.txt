cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

PROJECT(msi2lmp
    LANGUAGES CXX
)

# set(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_STANDARD 17)

aux_source_directory(
    ${CMAKE_CURRENT_LIST_DIR}/src
    DIR_SRCS
)

add_executable(msi2lmp
    ${DIR_SRCS}
)
