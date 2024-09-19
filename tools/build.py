"""! @brief Builds the DuME App on Linux
 Tested on Ubuntu
 @file build_app.py
 @section authors Author(s)
  - Created by Nicolas Dufresne on 6/24/2024 .
"""

import os
import subprocess

from rxbuilder.cpp import(
    build,
    clean
)

from rxbuilder.utils import(
    wipe,
)

from config import (
    BUILD_PATH,
    PRO_PATH,
    NUM_THREADS,
    SYSTEM,
    QMAKE_PATH,
    MAKE_PATH,
    EXE_NAME,
)

def run(mode='release'):
    """!
    @brief Runs DuME

    Parameters : 
        @param mode = 'release' => either 'release' or 'debug'

    """
    exe_path = get_exe(mode)
    if exe_path == "":
        print("> The project hasn't been built yet.")
        return
    print("> Running...")
    bin_process = subprocess.Popen( (exe_path) )
    bin_process.communicate()

def get_exe(mode='release'):
    """!
    @brief Checks if DuME has been built

    Parameters : 
        @param mode = 'release' => either 'release' or 'debug'

    """
    if SYSTEM == 'Windows':
        exe_path = os.path.join(BUILD_PATH, mode, mode, EXE_NAME)
    else:
        exe_path = os.path.join(BUILD_PATH, mode, EXE_NAME)

    if os.path.isfile(exe_path):
        return exe_path
    return ""

# Run
if __name__ == '__main__':
    wipe(BUILD_PATH)

    build(
        BUILD_PATH,
        PRO_PATH,
        QMAKE_PATH,
        MAKE_PATH,
        NUM_THREADS
        )

    clean(
        BUILD_PATH,
        MAKE_PATH
    )

    run()
