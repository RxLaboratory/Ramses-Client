import os
import shutil
from rxbuilder import (
    cpp,
    utils,
)
from config import (
    BUILD_PATH,
    PRO_PATH,
    NUM_THREADS,
    SYSTEM,
    VERSION_H,
    ICON_SET,
    DESKTOP_PATH,
    RESOURCES_PATH,
    PROJECT_NAME,
    QMAKE_PATH,
    MAKE_PATH,
    DEPLOYQT_PATH,
    BIN_PATH
)
from build import get_exe

if SYSTEM == "Linux":
    from rxbuilder import deploy_linux as dep
elif SYSTEM == "Windows":
    from rxbuilder import deploy_win as dep

VERSION = cpp.get_version(VERSION_H)

def get_app_dir():
    """!
    @brief Returns the path to the main app dir
    """
    return os.path.join(BUILD_PATH, 'deploy', PROJECT_NAME)

def deploy():
    """!
    @brief Deploys DuME App
    """

    exe_path = get_exe()

    # Build if it's not already built in release mode
    if exe_path == "":
        utils.wipe(BUILD_PATH)
        cpp.build(
            BUILD_PATH,
            PRO_PATH,
            QMAKE_PATH,
            MAKE_PATH,
            NUM_THREADS
            )
        cpp.clean(
            BUILD_PATH,
            MAKE_PATH
        )

    exe_path = get_exe()
    # Build failed
    if exe_path == "":
        print("> Can't deploy: build failed")
        return

    # Wipe previous
    app_dir = get_app_dir()
    utils.wipe(os.path.dirname(app_dir))

    # Deploy
    if SYSTEM == "Linux":
        dep.deploy_qt(
            app_dir,
            ICON_SET,
            DESKTOP_PATH,
            exe_path
        )
        dep.deploy_appImage(
            app_dir,
            DESKTOP_PATH,
            DEPLOYQT_PATH,
            QMAKE_PATH,
            VERSION
        )
        dep.deploy_deb(
            app_dir,
            RESOURCES_PATH,
            VERSION,
            os.path.basename(exe_path)
        )
        # TODO .tar.xz
        # TODO installer
    elif SYSTEM == "Windows":
        dep.deploy_qt(
            app_dir,
            exe_path,
            MAKE_PATH,
            DEPLOYQT_PATH
        )
        # We need to copy the openssl libs
        for file in os.listdir(BIN_PATH):
            shutil.copy(
                os.path.join(BIN_PATH, file),
                os.path.join(app_dir, file)
            )
        dep.deploy_zip(
            app_dir,
            VERSION
        )

# Run
if __name__ == '__main__':
    deploy()
    utils.write_version(BUILD_PATH, VERSION)
