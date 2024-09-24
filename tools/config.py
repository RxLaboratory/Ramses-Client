import os
import multiprocessing
import platform
from pathlib import Path
from rxbuilder.utils import normpath

# System

SYSTEM = platform.system()
NUM_THREADS = multiprocessing.cpu_count() - 1
if NUM_THREADS == 0:
    NUM_THREADS = 1

# General

PROJECT_NAME = "Ramses"
TOOLS_PATH = normpath(Path(__file__).parent.resolve())
REPO_PATH = normpath(Path(TOOLS_PATH).parent.resolve())
BUILD_PATH = os.path.join( REPO_PATH, 'build', SYSTEM)

# Sources and other files

PRO_PATH = os.path.join( REPO_PATH, 'src', 'Ramses.pro')
RESOURCES_PATH = os.path.join( REPO_PATH, 'src', 'resources')
DESKTOP_PATH = os.path.join( RESOURCES_PATH, 'Ramses.desktop')
ICON_SET = os.path.join( REPO_PATH, 'src', 'resources', 'icons', 'ramses.iconset')
VERSION_H = os.path.join(REPO_PATH, 'src', 'dumodules', 'duapp', 'app-version.h')
BIN_PATH = os.path.join( REPO_PATH, 'bin', SYSTEM)

# Windows
if SYSTEM == 'Windows':
    # Build tools
    QMAKE_PATH = "C:/Qt/5.15.2/mingw81_64/bin/qmake.exe"
    MAKE_PATH = "C:/Qt/Tools/mingw810_64/bin/mingw32-make.exe"
    DEPLOYQT_PATH = "C:/Qt/5.15.2/mingw81_64/bin/windeployqt.exe"
    # General
    EXE_NAME = "Ramses.exe"
# Linux
elif SYSTEM == 'Linux':
    # Build tools
    QMAKE_PATH = "~/Qt/5.15.2/gcc_64/bin/qmake"
    MAKE_PATH = "make"
    DEPLOYQT_PATH = "~/Deploy/linuxdeployqt.AppImage"
    # General
    EXE_NAME = "ramses"
