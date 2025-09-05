cmake_minimum_required(VERSION 3.28)

project(cmake-test)

function(COMPLEX)
    cmake_parse_arguments(
        COMPLEX_PREFIX
        "ANOTHER"
        "ONE_VALUE"
        "ALSO_ONE_VALUE"
        "MULTI_VALUE"
        ${ARGN}
    )

    message(STATUS "SINGLE: ${COMPLEX_PREFIX_SINGLE}")
    message(STATUS "ANOTHER: ${COMPLEX_PREFIX_ANOTHER}")
    message(STATUS "ONE_VALUE: ${COMPLEX_PREFIX_ONE_VALUE}")
    message(STATUS "ALSO_ONE_VALUE: ${COMPLEX_PREFIX_ALSO_ONE_VALUE}")
    message(STATUS "MULTI_VALUE: ${COMPLEX_PREFIX_MULTI_VALUE}")
endfunction()

complex(
    ONE_VALUE one
    MULTI_VALUE three four five
)
