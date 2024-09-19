import os
import subprocess
import platform

def get_version(h_file):
    """!
    @brief Gets the version of an app as a string from a C++ header file

    Parameters : 
        @param h_file => the header file
    """

    content = ()
    with open(h_file, 'r', encoding='utf8') as f:
        content = f.readlines()

    M = '0'
    m = '0'
    b = '0'
    s = ''
    for line in content:
        if line.startswith('#define VERSION_MAJOR '):
            M = line.replace('#define VERSION_MAJOR ', '').strip()
            continue
        if line.startswith('#define VERSION_MINOR '):
            m = line.replace('#define VERSION_MINOR ', '').strip()
            continue
        if line.startswith('#define VERSION_BUILD '):
            b = line.replace('#define VERSION_BUILD ', '').strip()
            continue
        if line.startswith('#define VERSION_SUFFIX '):
            s = line.replace('#define VERSION_SUFFIX ', '').strip()
            continue

    return M+'.'+m+'.'+b+s.replace('"', '')

def build(build_path, pro_path, qmake_path, make_path, num_threads, mode='release'):
    """!
    @brief Builds DuME

    Parameters : 
        @param mode = 'release' => either 'release' or 'debug'
    """

    build_path = os.path.join(build_path, mode)
    print("> Building "+os.path.basename(pro_path)+" in "+build_path+" ...")

    if not os.path.isdir(build_path):
        os.makedirs(build_path)

    # On windows, we need g++ in PATH
    env = os.environ.copy()
    if platform.system() == "Windows":
        env["PATH"] = os.path.dirname(make_path) + os.pathsep + env["PATH"]

    bin_args = [
        os.path.expanduser(qmake_path),
        pro_path
    ]
    if mode == 'debug':
        bin_args.append("CONFIG+=debug")
        bin_args.append("CONFIG+=qml_debug")

    bin_process = subprocess.Popen( bin_args, cwd=build_path, env=env )
    bin_process.communicate()

    print("> Running make...")

    bin_args = [
        os.path.expanduser(make_path),
        "-j" + str(num_threads)
    ]

    bin_process = subprocess.Popen( bin_args, cwd=build_path, env=env )
    bin_process.communicate()

def clean(build_path, make_path, mode='release'):
    """!
    @brief Cleans the build folder

    Parameters : 
        @param mode = 'release' => either 'release' or 'debug'

    """
    build_path = os.path.join(build_path, mode)
    print("> Cleaning "+build_path+" ...")
    bin_args = [
        os.path.expanduser(make_path),
        "clean"
    ]
    bin_process = subprocess.Popen( bin_args, cwd=build_path )
    bin_process.communicate()
