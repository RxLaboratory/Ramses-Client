#include "ducli.h"

#include <QString>

// For convenience
QString tr(const char *sourceText, const char *disambiguation = nullptr, int n = -1) {
    return qApp->tr(sourceText, disambiguation, n);
}

void DuCLI::initParser(QCommandLineParser *parser)
{
    parser->addPositionalArgument(
        "database",
        tr("The path to the *.ramses database to use."),
        "[db.ramses]"
        );

#ifdef Q_OS_WIN
    // On windows, we need the option to hide the console
    parser->addOption( QCommandLineOption("hide_console", tr("Hides the console when showing the UI.")) );
#endif

    parser->addOptions({
                        { "hide_banner", tr("Suppress printing banner.") },
                        { {"l", "log_level"}, tr("Set logging level. One of: debug|information|warning|error."), "loglevel" },
                        { "reinit", tr("Reset all the settings to their default values.") },
                        });
}

DuCLI::DuCLI() {}
