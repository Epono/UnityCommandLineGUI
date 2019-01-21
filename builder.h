#ifndef BUILDER_H
#define BUILDER_H

#include <QObject>
#include <QProcess>

class Builder : public QObject
{
    Q_OBJECT

public slots:
    void doBuild(const QString &commandLine);

signals:
    void buildFinished(int code);
};

#endif // BUILDER_H
