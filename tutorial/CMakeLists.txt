set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(TUTORIAL_SPHINX_BUILD "" CACHE PATH "Set the location of the HTML build." )

file(GLOB_RECURSE RST_FILES  "*.rst")
file(GLOB HTM_FILES  "*.html")

file(GLOB_RECURSE THEME_FILES  "tutorial_sphinx_theme_1/*.html")

file(GLOB_RECURSE CSS_FILES "tutorial_sphinx_theme_1/*.css")

source_group("Theme" FILES ${THEME_FILES} )

add_custom_target(Sphinx ALL
					COMMAND ${SPHINX_EXECUTABLE} -a -c ${CMAKE_CURRENT_SOURCE_DIR} -b html
					${SPHINX_SOURCE} ${TUTORIAL_SPHINX_BUILD}
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
					DEPENDS
					# Other docs files you want to track should go here (or in some variable)
					${SIMUL_PLATFORM_DIR}/Docs/index.rst
					${DOXYGEN_INDEX_FILE} conf.py tutorial/tutorial_sphinx_theme_1/static/css/theme.css
					MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
					COMMENT "Generating documentation with Sphinx"
					SOURCES ${RST_FILES} ${THEME_FILES} ${CSS_FILES} ${HTM_FILES} conf.py) 
set_target_properties( Sphinx PROPERTIES FOLDER Docs)


# see https://validator.github.io/validator/
set(TUTORIAL_VALIDATOR_EXECUTABLE "C:/Program Files/vnu-runtime-image/bin/vnu.bat" CACHE FILEPATH "path to vnu.bat")
set(ALL_HTML_FILES ${TUTORIAL_SPHINX_BUILD})
add_custom_target(Validator ALL
					COMMAND
					${TUTORIAL_VALIDATOR_EXECUTABLE} --format gnu --asciiquotes --skip-non-html ${ALL_HTML_FILES}
					WORKING_DIRECTORY ${TUTORIAL_SPHINX_BUILD}
					COMMENT "Validating documentation nu validator"
					SOURCES ${ALL_HTML_FILES})
	#add_dependencies(Validator Sphinx)
set_target_properties( Validator PROPERTIES FOLDER Docs)