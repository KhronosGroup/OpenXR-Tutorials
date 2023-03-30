set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD "" CACHE PATH "Set the location of the HTML build." )

file(GLOB_RECURSE RST_FILES
    "*.rst"
)

add_custom_target(Sphinx ALL
					COMMAND ${SPHINX_EXECUTABLE} -a -c ${CMAKE_CURRENT_SOURCE_DIR} -b html
					${SPHINX_SOURCE} ${SPHINX_BUILD}
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
					DEPENDS
					# Other docs files you want to track should go here (or in some variable)
					${SIMUL_PLATFORM_DIR}/Docs/index.rst
					${DOXYGEN_INDEX_FILE} conf.py tutorial/tutorial_sphinx_theme_1/static/css/theme.css
					MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
					COMMENT "Generating documentation with Sphinx"
					SOURCES ${RST_FILES} conf.py) 
set_target_properties( Sphinx PROPERTIES FOLDER Docs)
