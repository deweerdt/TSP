# - this module looks for Doxygen and the path to Graphiz's dot
# With the OS X GUI version, it likes to be installed to /Applications and
# it contains the doxygen executable in the bundle. In the versions I've 
# seen, it is located in Resources, but in general, more often binaries are 
# located in MacOS.
MESSAGE(STATUS "Looking for doxygen...")
FIND_PROGRAM(DOXYGEN
  NAMES doxygen
  PATHS "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\doxygen_is1;Inno Setup: App Path]/bin"
  /Applications/Doxygen.app/Contents/Resources
  /Applications/Doxygen.app/Contents/MacOS
  /usr/bin
  /usr/local/bin
  DOC "Doxygen documentation generation tool (http://www.doxygen.org)"
)
IF (DOXYGEN)
  MESSAGE(STATUS "Looking for doxygen... - found ${DOXYGEN}")
ENDIF (DOXYGEN)

# In the older versions of OS X Doxygen, dot was included with the 
# Doxygen bundle. But the new versions place make you download Graphviz.app
# which contains dot in its bundle.
MESSAGE(STATUS "Looking for dot tool...")
FIND_PROGRAM(DOT
  NAMES dot
  PATHS "$ENV{ProgramFiles}/ATT/Graphviz/bin"
  "C:/Program Files/ATT/Graphviz/bin"
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\ATT\\Graphviz;InstallPath]/bin
  /Applications/Graphviz.app/Contents/MacOS
  /Applications/Doxygen.app/Contents/Resources
  /Applications/Doxygen.app/Contents/MacOS
  /usr/bin
  /us/local/bin
  DOC "Graphiz Dot tool for using Doxygen"
)
IF (DOT)
  MESSAGE(STATUS "Looking for dot tool... - found ${DOT}")
ENDIF (DOT)

# The Doxyfile wants the path to Dot, not the entire path and executable
# so for convenience, I'll add another search for DOT_PATH.
FIND_PATH(DOT_PATH
  dot
  "C:/Program Files/ATT/Graphviz/bin"
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\ATT\\Graphviz;InstallPath]/bin
  /Applications/Graphviz.app/Contents/MacOS
  /Applications/Doxygen.app/Contents/Resources
  /Applications/Doxygen.app/Contents/MacOS
  /usr/bin
  /usr/local/bin
  DOC "Path to the Graphviz Dot tool"
)