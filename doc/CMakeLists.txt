find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR})
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR})
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/_build/html/index.html)

configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating doc/xml")
add_custom_target(Doxygen DEPENDS ${DOXYGEN_INDEX_FILE})
add_custom_target(docs
                  DEPENDS ${DOXYGEN_INDEX_FILE}
                  COMMAND ${SPHINX_EXECUTABLE} -b html
                  -Dbreathe_projects.rune-engine=${DOXYGEN_OUTPUT_DIR}/xml
                  ${SPHINX_SOURCE} ${SPHINX_BUILD}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation")
