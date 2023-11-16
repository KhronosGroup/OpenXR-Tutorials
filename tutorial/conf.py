# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))
import subprocess, os

def configureDoxyfile(input_dir, output_dir):
    with open('Doxyfile.in', 'r') as file :
        filedata = file.read()

    filedata = filedata.replace('@DOXYGEN_INPUT_DIR@', input_dir)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)

    with open('Doxyfile', 'w') as file:
        file.write(filedata)


# -- Project information -----------------------------------------------------

project = 'OpenXR Tutorial'
copyright = ''
author = 'Simul Software Ltd'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ["myst_parser","sphinx.ext.autosectionlabel","sphinxcontrib.mermaid","sphinxcontrib.jquery","sphinx_copybutton","sphinx.ext.extlinks"]
mermaid_output_format='png'
source_suffix = {'.rst': 'restructuredtext'}
# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["**/README.md","**/Readme.md","ReadMe.md","**/*.md","External/**/*.*"]

html_static_path = ['_static']
html_logo = "images/OpenXR_170px_Feb17.png"
html_theme_options = {
    'logo_only': False,
    'display_version': False
}

html_favicon = 'favicon.ico'
# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'tutorial_sphinx_theme_1'
html_theme_path = ["."]
html_theme_options = {
  "show_nav_level": 4
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

master_doc = 'index'

html_sidebars = {
   '**': ['globaltoc.html', 'sourcelink.html', 'searchbox.html']
}
mermaid_cmd='mmdc.cmd'

# -- Get and parse the OPENXR_TUTORIALS_GIT_TAG environment variable ---------
openxr_tutorials_git_tag_py = os.getenv("OPENXR_TUTORIALS_GIT_TAG")
if openxr_tutorials_git_tag_py != None:
    openxr_tutorials_git_tag_py = openxr_tutorials_git_tag_py.strip('\"')
else:
    openxr_tutorials_git_tag_py = 'v0.0.0'

print('openxr_tutorials_git_tag_py is ' + openxr_tutorials_git_tag_py)

# -- Definitions for sphinx.ext.extlinks -------------------------------------
extlinks = {
    'openxr_ref' : ('https://registry.khronos.org/OpenXR/specs/1.0/man/html/%s.html', '%s'), # :openxr_ref:
    'git_release' : ('https://github.com/KhronosGroup/OpenXR-Tutorials/releases/tag/' + openxr_tutorials_git_tag_py + '/%s', '%s') # :git_release:
}

# -- Substitutions for sphinx.ext.extlinks -----------------------------------
rst_epilog = 'Version: %s' % openxr_tutorials_git_tag_py # Appears at the end of the page
rst_prolog = '.. |openxr_tutorials_git_tag| replace:: %s' % openxr_tutorials_git_tag_py