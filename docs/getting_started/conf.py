# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'Getting Started with Robot Raconteur'
copyright = '2023, Wason Technology, LLC'
author = 'John Wason'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx_rtd_theme',
    'sphinx_tabs.tabs'
]

try:
    import sphinx_toolbox.collapse
    extensions.append('sphinx_toolbox.collapse')
except ImportError:
    from docutils.parsers.rst import directives
    from docutils.parsers.rst.directives.body import Container

    def setup(app):
        # Maps 'collapse' to the standard 'container' directive
        app.add_directive('collapse', Container)

templates_path = ['templates']
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
