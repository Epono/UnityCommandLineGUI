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
#include <QThread>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>

#include <builder.h>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void launchBuild(const QString &commandLine);

public slots:
    void browseExe();
    void browseProject();
    void browseOutput();
    void saveConf();
    void build();
    void updateExitAfterBuild(int newCheckState);
    void handleBuildFinished(int code);

private:
    QThread workerThread;

    QLineEdit* m_exePath;
    QLineEdit* m_projectPath;
    QLineEdit* m_outputPath;
    QLabel* m_commandLine;
    QLineEdit* m_loginLineEdit;
    QLineEdit* m_passwordLineEdit;
    QCheckBox* m_launchEditor;
    QCheckBox* m_exitAfterBuild;
    QPushButton* m_buildButton;

    QTimer* m_timerForFakeProgressBar;
    QProgressBar* m_fakeBuildProgressBar;
};

#endif // MAINWINDOW_H
