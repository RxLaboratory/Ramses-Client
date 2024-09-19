import os
import shutil
import subprocess
import zipfile
from .utils import (
    zip_dir,
)

def deploy_qt(app_dir, exe_path, make_path, windeployqt):
    """!
    @brief Deploys to a folder

    Parameters : 
        @param app_dir => The destination
        @param exe_path => The app executable
        @param windeployqt => The path to windeployqt
    """

    print("> Deploying main dir in "+app_dir+" ...")

    if not os.path.isdir(app_dir):
        os.makedirs(app_dir)

    # Copy  bin
    exe = os.path.join(app_dir, os.path.basename(exe_path))
    shutil.copy(
        exe_path,
        exe
    )

    # Windeployqt
    bin_args = [
        windeployqt,
        '--compiler-runtime',
        exe
    ]

    # we need g++ in PATH
    env = os.environ.copy()
    env["PATH"] = os.path.dirname(make_path) + os.pathsep + env["PATH"]

    bin_process = subprocess.Popen( bin_args, cwd=os.path.dirname(app_dir), env=env )
    bin_process.communicate()

def deploy_zip(app_dir, version):
    """!
    @brief Exports to a zip

    Parameters : 
        @param app_dir => The app dir path
        @param version => The app version

    """
    zip_file = os.path.join(
        os.path.dirname(app_dir),
        os.path.basename(app_dir) + '-' + version + '.zip'
        )
    with zipfile.ZipFile(zip_file, 'w', zipfile.ZIP_DEFLATED) as z:
        zip_dir(app_dir, z)
