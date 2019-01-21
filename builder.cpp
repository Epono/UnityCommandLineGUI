#include "builder.h"

void Builder::doBuild(const QString &commandLine) {
    int code = QProcess::execute(commandLine);
    emit buildFinished(code);
}
