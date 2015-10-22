from setuptools import setup

"""
# These 'backends' are dynamically imported so we need to tell py2app about
# them.
# https://github.com/tiagopereira/python_tips/wiki/Using-py2app-with-Enthought-GUI-tools-and-Qt
# http://stackoverflow.com/questions/15561722/error-in-py2exe-python-app-using-chaco-in-pyside
"""
# add if using enthought python
"""
OPTIONS = dict(
               includes = [
                           # The backends are dynamically imported and thus we need to
                           # tell py2app about them.
                           'kiva.*',
                           'enable.*',
                           'enable.qt4.*',
                           'pyface.*',
                           'pyface.ui.qt4.*',
                           'pyface.ui.qt4.action.*',
                           'pyface.ui.qt4.timer.*',
                           'pyface.ui.qt4.wizard.*',
                           'pyface.ui.qt4.workbench.*',
                           'traitsui.qt4.*',
                           'traitsui.qt4.extra.*',
                           'PyQt4.pyqtconfig'],
               argv_emulation = True)
"""

setup(
      app = ["LMTools.py"],
      #packages = [],
      #options = {'py2app': OPTIONS},
      setup_requires = ["py2app"]
)
