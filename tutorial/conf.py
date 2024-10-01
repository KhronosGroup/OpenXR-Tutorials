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

def createSiteMap(root):
    variant_pages = ['1-introduction','2-setup','3-graphics','4-actions','5-extensions','6-next-steps']
    static_pages=['index','search'];
    platforms = ['linux','windows','android']
    apis=[['vulkan','opengl'],['vulkan','opengl','d3d11','d3d12'],['vulkan','opengles']]
    with open('sitemap.txt', 'w') as f:
        for pg in static_pages:
            f.write(root+'/'+pg+'/')
            f.write('\n')
        for pg in variant_pages:
            for i in range(len(platforms)):
                platform=platforms[i]
                for api in apis[i]:
                    f.write(root+'/'+pg+'/'+platform+'/'+api+'/')
                    f.write('\n')
def configureDoxyfile(input_dir, output_dir):
    with open('Doxyfile.in', 'r') as file :
        filedata = file.read()

    filedata = filedata.replace('@DOXYGEN_INPUT_DIR@', input_dir)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)

    with open('Doxyfile', 'w') as file:
        file.write(filedata)

OPENXR_PLATFORM_API='Android'
if(tags.has('windows')):
    OPENXR_PLATFORM_API = "Windows"
if(tags.has('linux')):
    OPENXR_PLATFORM_API = "Linux"
if(tags.has('android')):
    OPENXR_PLATFORM_API = "Android"
OPENXR_PLATFORM_API+="/"
if(tags.has('vulkan')):
    OPENXR_PLATFORM_API += "Vulkan"
elif(tags.has('opengles')):
    OPENXR_PLATFORM_API += "OpenGL ES"
elif(tags.has('opengl')):
    OPENXR_PLATFORM_API += "OpenGL"
elif(tags.has('d3d11')):
    OPENXR_PLATFORM_API += "D3D11"
elif(tags.has('d3d12')):
    OPENXR_PLATFORM_API += "D3D12"
else:
    OPENXR_PLATFORM_API += "Vulkan"
if(tags.has('OPENXR_MAINSITE')):
    OPENXR_MAINSITE="true";
else:
    OPENXR_MAINSITE="false";
OPENXR_PLATFORM_API_PATH=OPENXR_PLATFORM_API.replace(' ','').lower();
print('OPENXR_PLATFORM_API_PATH '+OPENXR_PLATFORM_API_PATH)
print('OPENXR_MAINSITE '+OPENXR_MAINSITE)
html_context = {'platform_api': OPENXR_PLATFORM_API, 'platform_api_path':OPENXR_PLATFORM_API_PATH,'tutorial_root_site':OPENXR_MAINSITE}


# -- Project information -----------------------------------------------------

project = 'OpenXR Tutorial'
copyright = ''
author = 'Simul Software Ltd'

createSiteMap('https://openxr-tutorial.com')
# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ["myst_parser","sphinx.ext.autosectionlabel","sphinxcontrib.mermaid","sphinxcontrib.jquery","sphinx_copybutton","sphinx.ext.extlinks"]
mermaid_output_format='png'
source_suffix = {'.rst': 'restructuredtext'}
# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']
sitemap_locales = []
# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["**/README.md","**/Readme.md","ReadMe.md","**/*.md","External/**/*.*"]
html_baseurl = 'https://openxr-tutorial.com/'
html_static_path = ['_static']
html_extra_path = []

html_logo = "images/OpenXR_170px_Feb17.png"
html_theme_options = {
    'logo_only': False,
    'display_version': False,
    'show_next_previous_button': OPENXR_MAINSITE=="false"
}

html_favicon = 'favicon.ico'
# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'tutorial_sphinx_theme_1'
html_theme_path = ["."]

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
    'openxr_ref' : ('https://registry.khronos.org/OpenXR/specs/1.1/man/html/%s.html', '%s'), # :openxr_ref:
    'git_release' : ('https://github.com/KhronosGroup/OpenXR-Tutorials/releases/download/' + openxr_tutorials_git_tag_py + '/%s', '%s') # :git_release:

}

# -- Substitutions for sphinx.ext.extlinks -----------------------------------
rst_epilog = 'Version: %s' % openxr_tutorials_git_tag_py # Appears at the end of the page
rst_prolog = '.. |openxr_tutorials_git_tag| replace:: %s' % openxr_tutorials_git_tag_py


def setup(app):
    print(str(app.config.html_extra_path));
