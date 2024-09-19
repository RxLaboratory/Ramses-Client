
import os
import shutil
import subprocess
import tempfile
from .utils import (
    get_dir_size,
    replace_in_file
)

def deploy_qt(app_dir, icon_set, desktop_path, exe_path):
    """!
    @brief Deploys DuME main (common) directory
    """
    print("> Deploying main dir in "+app_dir+" ...")

    bin_dir = os.path.join(app_dir, 'bin')
    if not os.path.isdir(bin_dir):
        os.makedirs(bin_dir)

    share_dir = os.path.join(app_dir, 'share')
    if not os.path.isdir(share_dir):
        os.makedirs(share_dir)

    icons_dir = os.path.join(share_dir, 'icons', 'hicolor')
    if not os.path.isdir(icons_dir):
        os.makedirs(icons_dir)

    applications_dir = os.path.join(share_dir, 'applications')
    if not os.path.isdir(applications_dir):
        os.makedirs(applications_dir)

    # Copy icons
    for i in os.listdir(icon_set):
        dest_dir = ""
        if "24x24" in i:
            dest_dir = os.path.join(icons_dir, "24x24", "apps")
        elif "32x32" in i:
            dest_dir = os.path.join(icons_dir, "32x32", "apps")
        elif "48x48" in i:
            dest_dir = os.path.join(icons_dir, "48x48", "apps")
        elif "128x128" in i:
            dest_dir = os.path.join(icons_dir, "128x128", "apps")
        elif "256x256" in i:
            dest_dir = os.path.join(icons_dir, "256x256", "apps")
        else:
            continue
        i_file = os.path.join(icon_set, i)
        if not os.path.isdir(dest_dir):
            os.makedirs(dest_dir)
        shutil.copy(
            i_file,
            os.path.join(dest_dir, os.path.basename(exe_path) + ".png")
        )

    # Copy Desktop file
    if os.path.isfile(desktop_path):
        shutil.copy(
            desktop_path,
            os.path.join(applications_dir, os.path.basename(desktop_path))
        )

    # Copy bin
    shutil.copy(
        exe_path,
        os.path.join(bin_dir, os.path.basename(exe_path))
    )

def deploy_appImage(usr_dir, desktop_path, linuxdeployqt, qmake_path, version):
    """!
    @brief Deploys the AppImage
    The main folder must already be deployed
    """
    print("> Deploying AppImage ...")
    app_dir = usr_dir + ".AppDir"
    shutil.copytree(
        usr_dir,
        os.path.join(app_dir, 'usr')
    )

    desktop_file = os.path.join(app_dir, 'usr', 'share', 'applications', os.path.basename(desktop_path))

    bin_args = [
        os.path.expanduser(linuxdeployqt),
        desktop_file,
        '-unsupported-allow-new-glibc',
        '-always-overwrite',
        '-no-translations',
        '-qmake=' + os.path.expanduser(qmake_path),
        '-extra-plugins=iconengines,platformthemes/libqgtk3.so',
        '-appimage',
        '-no-strip'
    ]

    env = os.environ.copy()
    env['VERSION'] = version.replace('-', '_')
    bin_process = subprocess.Popen( bin_args, cwd=os.path.dirname(app_dir), env=env )
    bin_process.communicate()

def deploy_deb(usr_dir, resources_path, version, exe_name):
    """!
    @brief Deploys the deb package
    """

    print("> Deploying Deb package...")

    with tempfile.TemporaryDirectory() as tmpdata_folder:

        # copy data to the tmpfolder
        deb_folder = os.path.join(tmpdata_folder, 'deb')
        shutil.copytree(
            usr_dir,
            os.path.join(deb_folder, 'usr')
            )

        # Create DEBIAN folder
        debian_folder = os.path.join(deb_folder, "DEBIAN")

        os.mkdir( debian_folder )
        shutil.copy(
            os.path.join(resources_path, 'copyright'),
            os.path.join(debian_folder, 'copyright')
            )
        control_file = os.path.join(debian_folder, 'control')
        shutil.copy(
            os.path.join(resources_path, 'control'),
            control_file
            )
        # Update control file
        size = get_dir_size( usr_dir )
        size = round( size / 1024 )
        replace_in_file( {
                "#version#": version,
                "#size#": str(size)
                },
                control_file
            )

        cmd_str = "find . -type f ! -regex '.*.hg.*' ! -regex '.*?debian-binary.*' ! -regex '.*?DEBIAN.*' -printf '%P ' | xargs md5sum > DEBIAN/md5sums"
        subprocess.run(cmd_str, shell=True, cwd=deb_folder, check=False)
        subprocess.run("chmod 755 DEBIAN", shell=True, cwd=deb_folder, check=False)
        subprocess.run("dpkg -b deb dume.deb", shell=True, cwd=tmpdata_folder, check=False)

        # Get the result
        shutil.copy(
            os.path.join(tmpdata_folder, exe_name + '.deb'),
            os.path.join(os.path.dirname(usr_dir), exe_name + '-' + version.replace('-', '_') + '-amd64.deb' )
            )
