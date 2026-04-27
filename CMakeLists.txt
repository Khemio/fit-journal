cmake_minimum_required(VERSION 3.10)
project(journal CXX)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(vendor/Bcrypt.cpp)

add_executable(${PROJECT_NAME} src/main.cc)


# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# add_subdirectory(drogon) 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} 
                        PRIVATE Drogon::Drogon
                        bcrypt-hash)

# ##############################################################################


aux_source_directory(src/controllers CTL_SRC)
aux_source_directory(src/filters FILTER_SRC)
aux_source_directory(src/plugins PLUGIN_SRC)
aux_source_directory(src/models MODEL_SRC)
aux_source_directory(src/database DB_MNG_SRC)
aux_source_directory(${CMAKE_CURRENT_BINARY_DIR}/views_gen VIEWS_GEN_SRC)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/static
            ${CMAKE_CURRENT_BINARY_DIR}/static
)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/views
                    ${CMAKE_CURRENT_BINARY_DIR}/views_gen)
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(${PROJECT_NAME}
                            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/models)
target_sources(${PROJECT_NAME}
                PRIVATE
                ${SRC_DIR}
                ${VIEWS_GEN_SRC}
                ${CTL_SRC}
                ${DB_MNG_SRC}
                ${FILTER_SRC}
                ${MODEL_SRC}
                ${PLUGIN_SRC})
# ##############################################################################
# uncomment the following line for dynamically loading views 
set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

add_subdirectory(test)
