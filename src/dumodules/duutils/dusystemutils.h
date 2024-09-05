#ifndef DUSYSTEMUTILS_H
#define DUSYSTEMUTILS_H

#include "qglobal.h"
#include <QString>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winbase.h>
#include <shellapi.h>
#include <processthreadsapi.h>
#include <Lmcons.h>
#include <lmaccess.h>
#include <QString>

/**
 * Useful methods and wrappers for the Windows API
 */
namespace WinUtils
{
/**
     * @brief logon Shows a dialog to ask for the user name and password
     * @return true on success
     */
bool logon(HANDLE *h, QString reason = "");

/**
     * @brief logonUser Gets the user token (the handle)
     * @param username
     * @param password
     * @param h
     * @return
     */
bool logonUser(QString username, QString password, HANDLE *h);

/**
     * @brief execElevated runs a command and its params as elevated (admin) user. The windows UAC may be shown. This methods waits for the command to finish.
     * @param cmd
     * @param params
     * @return The command exit code
     */
int execElevated(QString cmd, QStringList params, bool detached = false);

/**
     * @brief execElevated runs a command as a given user using the token. This methods waits for the command to finish.
     * @param userToken
     * @param cmd
     * @param params
     * @return
     */
int execAs(HANDLE userToken, QString cmd, QStringList params);

/**
     * @brief error Gets the formatted last error of the current thread
     * @return
     */
QString error();

/**
     * @brief sanitizeParams prepares a list of parameters to be passed to exec or createProcess
     * @param params
     * @return
     */
QString sanitizeParams(QStringList params);

/**
     * @brief userName Returns the current user name running the process
     * @return
     */
QString userName();

/**
     * @brief isAdmin Checks if the user running the process is admin
     * @return
     */
bool isAdmin();
}
#endif // WINDOWS

#ifdef Q_OS_LINUX
#include <unistd.h>
namespace LinuxUtils
{
/**
     * @brief userName Returns the current user name running the process
     * @return
     */
QString userName();

/**
     * @brief isAdmin Checks if the user running the process is admin (sudoer)
     * @return
     */
bool isAdmin();
};
#endif //  LINUX

namespace SystemUtils {
bool isAdmin();
QString userName();
}

#endif // DUSYSTEMUTILS_H
