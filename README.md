# Sample Python Plugin
A Sample/Template project to demonstrate the implementation of Xfce4 Panel plugin in python

# Requirements
- cmake 3.1 and above (might also work for the lower version too.)
- gcc
- pkgconfig
- libgtk3-dev
- libxfce4panel-2.0-dev
- python-gi-dev

**requirements are as per xubuntu 21.10 release**


# How To Use
To use this repository for contructing a full fledged plugin you need to
- Change the PLUGIN_ID with Your Unique Plugin Name
- Make sure the python plugin exists as src/**<PLUGIN_ID>**.py
- Code your plugin in src/**<PLUIGN_ID>**.py
- Make sure the add any other python file used in **<PLUGIN_ID>**.py to cmake install(FILES) at DESTINATION ${PYTHON_PLUGIN_PATH_SUFFIX}


# Building
```shell
cmake -B build -CMAKE_INSTALL_PREFIX=/usr
cmake --build build
cmake --install build --prefix /usr
```