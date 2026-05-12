# ================================================================================= #
# ------------------------------- Demo Executables -------------------------------- #
# ================================================================================= #
#
# Demo programs live in src/main_*.cc and are built automatically as
# test_<Structure> executables. Auto-discovery keeps CMake aligned with the
# repository as new demos are added or renamed.
#
# Prerequisites:
#   - ads_lib target must be defined (from LibraryDefinition.cmake)
#   - ads_add_executable() function must be available (from CompilerSetup.cmake)
#
# Usage:
#   include(cmake/MainExecutables.cmake)
#
# ================================================================================= #

file(GLOB ADS_MAIN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main_*.cc"
)

list(SORT ADS_MAIN_SOURCES)
set(ADS_MAIN_EXECUTABLE_TARGETS "")

foreach(main_source IN LISTS ADS_MAIN_SOURCES)
    get_filename_component(main_name "${main_source}" NAME_WE)
    string(REGEX REPLACE "^main_" "" structure_name "${main_name}")
    set(target_name "test_${structure_name}")

    ads_add_executable("${target_name}" "${main_source}")
    list(APPEND ADS_MAIN_EXECUTABLE_TARGETS "${target_name}")
endforeach()

if(TARGET test_Binary_Search_Tree)
    set(ADS_DEFAULT_RUN_TARGET test_Binary_Search_Tree)
elseif(ADS_MAIN_EXECUTABLE_TARGETS)
    list(GET ADS_MAIN_EXECUTABLE_TARGETS 0 ADS_DEFAULT_RUN_TARGET)
else()
    unset(ADS_DEFAULT_RUN_TARGET)
endif()

list(LENGTH ADS_MAIN_EXECUTABLE_TARGETS ADS_MAIN_EXECUTABLE_COUNT)
message(STATUS "${ANSI_GREEN}Configured ${ADS_MAIN_EXECUTABLE_COUNT} demo executables${ANSI_RESET}")
