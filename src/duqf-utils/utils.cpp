#include "utils.h"

void FileUtils::setReadWrite(QFile *file)
{
    file->setPermissions(QFile::ReadOther | QFile::WriteOther | QFile::ReadUser | QFile::WriteUser | QFile::ReadOwner | QFile::WriteOwner);
}

void FileUtils::setReadWrite(QString path)
{
    QFile file(path);
    if (file.exists())
    {
        setReadWrite(&file);
    }
}

void FileUtils::move(QString from, QString to)
{
    QFile origin(from);
    origin.rename(to);
    setReadWrite(&origin);
}

void FileUtils::copy(QString from, QString to)
{
    QFile origin(from);
    origin.copy(to);
    setReadWrite(to);
}

void FileUtils::remove(QString path)
{
    QFile file(path);
    setReadWrite(&file);
    file.remove();
}

bool MediaUtils::isBusy(MediaUtils::RenderStatus status)
{
    if ( status == Launching ) return true;
    else if ( status == Encoding ) return true;
    else if ( status == FramesConversion ) return true;
    else if ( status == FFmpegEncoding ) return true;
    else if ( status == AERendering ) return true;
    else if ( status == BlenderRendering ) return true;
    else if ( status == Cleaning ) return true;

    return false;
}

QString MediaUtils::statusString(MediaUtils::RenderStatus status)
{
    if ( status == Initializing ) return "Initializing...";
    else if ( status == Waiting ) return "Ready";
    else if ( status == Launching ) return "Launching...";
    else if ( status == Encoding ) return "Running...";
    else if ( status == FramesConversion ) return "Conerting frames...";
    else if ( status == FFmpegEncoding ) return "FFmpeg encoding process...";
    else if ( status == AERendering ) return "After Effects rendering process...";
    else if ( status == BlenderRendering ) return "Blender rendering process...";
    else if ( status == Cleaning ) return "Cleaning...";
    else if ( status == Finished ) return "Process finished!";
    else if ( status == Stopped ) return "Process has been stopped.";
    else if ( status == Error ) return "An error has occured.";
    else return "Unknown status";
}

QString MediaUtils::sizeString(qint64 size)
{
    QString sizeText = "";

    if ( size < 1024*1024*10.0 )
    {
        size = size / 1024;
        sizeText = QString::number( int(size*100)/100) + " kB";
    }
    else if ( size < 1024*1024*1024*10.0 )
    {
        size = size / 1024 / 1024;
        sizeText = QString::number( int(size*100)/100) + " MB";
    }
    else
    {
        size = size / 1024 / 1024 / 1024;
        sizeText = QString::number( int(size*100)/100) + " GB";
    }
    return sizeText;
}

QString MediaUtils::bitrateString(qint64 bitrate)
{
    QString sizeText = "";

    if ( bitrate < 1024*10.0 )
    {
        sizeText = QString::number( int(bitrate*100)/100) + " bps";
    }
    else if ( bitrate < 1024*1024*10.0 )
    {
        bitrate = bitrate / 1024;
        sizeText = QString::number( int(bitrate*100)/100) + " kbps";
    }
    else if ( bitrate < 1024*1024*1024*10.0 )
    {
        bitrate = bitrate / 1024 / 1024;
        sizeText = QString::number( int(bitrate*100)/100) + " Mbps";
    }
    else
    {
        bitrate = bitrate / 1024 / 1024 / 1024;
        sizeText = QString::number( int(bitrate*100)/100) + " Gbps";
    }
    return sizeText;
}

double MediaUtils::convertFromBps(qint64 value, MediaUtils::BitrateUnit to)
{
    if (to == kbps) return value / 1024;
    if (to == Mbps) return value / 1024 / 1024;
    return value;
}

double MediaUtils::convertFromBytes(qint64 value, MediaUtils::SizeUnit to)
{
    if (to == kB) return value / 1024;
    if (to == MB) return value / 1024 / 1024;
    if (to == GB) return value / 1024 / 1024 / 1024;
    return value;
}

qint64 MediaUtils::convertToBps(qint64 value, MediaUtils::BitrateUnit from)
{
    if (from == kbps) return value * 1024;
    if (from == Mbps) return value * 1024 * 1024;
    return value;
}

qint64 MediaUtils::convertToBytes(qint64 value, MediaUtils::SizeUnit from)
{
    if (from == kB) return value * 1024;
    if (from == MB) return value * 1024 * 1024;
    if (from == GB) return value * 1024 * 1024 * 1024;
    return value;
}

QRegularExpression RegExUtils::getRegEx(QString name, QString replace, QString by)
{
    QFile regExFile(":/regex/" + name );
    if (regExFile.open(QFile::ReadOnly))
    {
        QString regExStr = regExFile.readAll();
        if (replace != "") regExStr = regExStr.replace(replace, by);
        return QRegularExpression( regExStr.trimmed() );
    }
    return QRegularExpression();
}

double Interpolations::linear(double val, double fromMin, double fromMax, double toMin, double toMax)
{
    double dFrom = fromMax - fromMin;
    double dTo = toMax - toMin;
    double dVal = val - fromMin;

    return (dVal * dTo)/dFrom + toMin;
}

QString MediaUtils::durationToTimecode(double duration)
{
    QTime d(0,0,0);
    int secs = int( duration );
    int msecs = int( (duration - secs) * 1000 );
    d = d.addSecs( secs );
    d = d.addMSecs( msecs );
    return d.toString("hh:mm:ss.zzz");
}

qint64 FileUtils::getDirSize(QDir d)
{
    qint64 size = 0;
    //calculate total size of current directories' files
    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden|QDir::Dirs|QDir::NoDotAndDotDot;
    foreach(QFileInfo fi, d.entryInfoList(fileFilters)) {
        if (fi.isFile()) size+= fi.size();
        else size += getDirSize( QDir(fi.absoluteFilePath()) );
    }
    return size;
}

void FileUtils::openInExplorer(QString path, bool askForCreation)
{
    const QFileInfo fileInfo(path);

    if(!fileInfo.exists() && askForCreation)
    {
        QMessageBox::StandardButton rep = QMessageBox::question(nullptr,
                                                                "The folder does not exist",
                                                                "This folder:\n\n" + path + "\n\ndoes not exist yet.\nDo you want to create it now?",
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes);
        if (rep == QMessageBox::Yes) QDir(path).mkpath(".");
        else return;
    }
    else if (!fileInfo.exists()) return;

#ifdef Q_OS_WIN
    QString param("");
    if (!fileInfo.isDir())
        param = "/select,";
    param += QDir::toNativeSeparators(fileInfo.canonicalFilePath());
    QProcess::startDetached("explorer.exe", QStringList(param));
#endif
#ifdef Q_OS_MAC
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                                     .arg(fileInfo.canonicalFilePath());
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
#endif
#ifdef Q_OS_LINUX
    QString p = fileInfo.canonicalFilePath();
    if (fileInfo.isFile()) p = fileInfo.dir().canonicalPath();
    QProcess::execute("xdg-open", QStringList(p));
#endif
}

