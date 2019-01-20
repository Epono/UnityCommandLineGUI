#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QPushButton>
#include <QDateEdit>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QLabel>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:
    void browseExe();
    void browseProject();
    void browseOutput();
    void saveConf();
    void build();

private:
    QLineEdit* m_exePath;
    QLineEdit* m_projectPath;
    QLineEdit* m_outputPath;
    QLabel* m_commandLine;
};

#endif // MAINWINDOW_H
