#include "dusystemutils.h"


#ifdef Q_OS_WIN
#include <QDir>
#include <QtDebug>
#include "duwidgets/dulogindialog.h"

bool WinUtils::logonUser(QString username, QString password, HANDLE *h)
{
    QString domain;
    if (username.indexOf("@")) {
        QStringList user = username.split("@");
        username = user.first();
        domain = user.last();
    }
    else  {
        domain = ".";
    }
    return LogonUser(
        username.toStdWString().c_str(),
        domain.toStdWString().c_str(),
        password.toStdWString().c_str(),
        LOGON32_LOGON_INTERACTIVE,
        LOGON32_PROVIDER_DEFAULT,
        h
        );
}

int WinUtils::execElevated(QString cmd, QStringList params, bool detached)
{
    // Sanity checks
    cmd  = QDir::toNativeSeparators(cmd);
    QString param = sanitizeParams(params);

    SHELLEXECUTEINFO ShExecInfo = {};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = L"runas";
    ShExecInfo.lpFile = cmd.toStdWString().c_str();
    ShExecInfo.lpParameters = param.toStdWString().c_str();
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = NULL;
    if (!ShellExecuteEx(&ShExecInfo)) return 1;
    if (detached) return 0;
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
    ulong result;
    GetExitCodeProcess(ShExecInfo.hProcess, &result);
    CloseHandle(ShExecInfo.hProcess);
    return result;
}

bool WinUtils::logon(HANDLE *h, QString reason)
{
    // First, ask for username and password
    DuLoginDialog *loginDialog = new DuLoginDialog();
    if (reason !="") loginDialog->setInfo(reason);

    while (loginDialog->exec() == QDialog::Accepted) {
        if (WinUtils::logonUser(
                loginDialog->userName(),
                loginDialog->password(),
                h
                )) return true;
        // Get the error
        loginDialog->setInfo(WinUtils::error());
        qDebug() <<  QStringLiteral("Windows Logon failed.");
    }

    qDebug() <<  QStringLiteral("Windows Logon canceled.");
    return false;
}

QString WinUtils::error()
{
    DWORD err = GetLastError();
    qDebug() << "Windows Error Code: " << err;
    if (err == ERROR_LOGON_FAILURE)
        return "Invalid username or password";
    if (err == ERROR_LOGON_TYPE_NOT_GRANTED)
        return "Insufficient rights";
    if (err == ERROR_INVALID_HANDLE) {
        return "Invalid Handle";
    }
    return QString("Unknown error (%1)").arg(err);
}

int WinUtils::execAs(HANDLE userToken, QString cmd, QStringList params)
{
    // Sanity checks
    cmd  = "\"" + QDir::toNativeSeparators(cmd) + "\"";
    QString param = cmd + " " + sanitizeParams(params);

    // Convert
    std::wstring cmdStr = param.toStdWString();
    wchar_t *cmdC = new wchar_t[ cmdStr.size() ];
    wcscpy(cmdC, cmdStr.c_str() );

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    if ( !CreateProcessAsUser(
            userToken,
            0,
            cmdC,
            0,
            0,
            false,
            CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS,
            0,
            0,
            &si,
            &pi
            )) {
        qDebug() << WinUtils::error();
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    ulong result;
    GetExitCodeProcess(pi.hProcess, &result);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
}

QString WinUtils::sanitizeParams(QStringList params)
{
    // Enclose params in double quotes, and build param string
    for(int i = 0; i < params.count(); i++) {
        params[i] = "\"" + params[i] + "\"";
    }
    return params.join(" ");
}

bool WinUtils::isAdmin()
{
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if ( OpenProcessToken( GetCurrentProcess( ),TOKEN_QUERY,&hToken ) ) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof( TOKEN_ELEVATION );
        if( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof( Elevation ), &cbSize ) ) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if ( hToken ) {
        CloseHandle( hToken );
    }
    return fRet;
}

QString WinUtils::userName()
{
    QString username;
    wchar_t *acUsername = new wchar_t[UNLEN + 1];
    DWORD nUsername = sizeof(acUsername);
    if (GetUserName(acUsername, &nUsername)) {
        username = QString::fromWCharArray(acUsername);
    }
    else {
        // If for some reason we don't get it, use env
        username = qgetenv("USER");
        if (username.isEmpty())
            username = qgetenv("USERNAME");
    }

    delete[] acUsername;

    return username;
}

#endif

#ifdef Q_OS_LINUX
#include <QProcess>
#include <QString>
bool LinuxUtils::isAdmin()
{
    //return geteuid() == 0;
    // OR
    return getuid() != geteuid();
}

QString LinuxUtils::userName()
{
    QProcess process;
    process.setProgram("whoami");
    process.start(QIODevice::ReadOnly);
    process.waitForFinished(1000);
    QString username = process.readAllStandardOutput();
    // If for some reason we don't get it, use env
    username = qgetenv("USER");
    if (username.isEmpty())
        username = qgetenv("USERNAME");
    return username;
}
#endif



QString SystemUtils::userName()
{
#ifdef Q_OS_WIN
    return WinUtils::userName();
#endif
#ifdef Q_OS_LINUX
    return LinuxUtils::userName();
#endif
}

bool SystemUtils::isAdmin()
{
#ifdef Q_OS_WINDOWS
    return WinUtils::isAdmin();
#endif
#ifdef Q_OS_LINUX
    LinuxUtils::isAdmin();
#endif
    return "";
}
