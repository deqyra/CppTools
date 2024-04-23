find_program(
    CLANG_FORMAT_EXE
    clang-format
    PATH_SUFFIXES .exe
)

if (NOT CLANG_FORMAT_EXE)
    message(FATAL_ERROR "Could not find clang-format")
endif()

file(GLOB_RECURSE
    CPPTOOLS_FILE_LIST
    LIST_DIRECTORIES false
    ${CMAKE_CURRENT_SOURCE_DIR}/../cpptools/*.?pp
)

set(FILE_LIST_FILE ${CMAKE_CURRENT_BINARY_DIR}/cpptools_file_list.txt)

list(JOIN CPPTOOLS_FILE_LIST \n CPPTOOLS_FILES)

file(WRITE
    ${FILE_LIST_FILE}
    ${CPPTOOLS_FILES}
)

execute_process(COMMAND ${CLANG_FORMAT_EXE} -i --files=${FILE_LIST_FILE})
