#include "mainwindow.h"

// TODO: variabalize json paths
MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // PATHS
    QPushButton* browseExe = new QPushButton("...");
    browseExe->setFixedWidth(20);
    m_exePath = new QLineEdit;
    m_exePath->setMinimumWidth(400);

    QPushButton* browseProject = new QPushButton("...");
    browseProject->setFixedWidth(20);
    m_projectPath = new QLineEdit;
    m_projectPath->setMinimumWidth(400);

    QPushButton* browseOutput = new QPushButton("...");
    browseOutput->setFixedWidth(20);
    m_outputPath = new QLineEdit;
    m_outputPath->setMinimumWidth(400);

    // CREDENTIALS
    m_loginLineEdit = new QLineEdit;
    m_passwordLineEdit = new QLineEdit;
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);

    // EDITOR
    m_launchEditor = new QCheckBox("Launch Editor?");
    m_exitAfterBuild = new QCheckBox("Exit after build?");

    // BUTTONS
    m_buildButton = new QPushButton("Build!");
    m_buildButton->setMaximumWidth(80);
    m_commandLine = new QLabel;

    QPushButton* saveConfigButton = new QPushButton("Save conf");
    saveConfigButton->setMaximumWidth(80);

    // LAYOUTS
    QGridLayout* layout = new QGridLayout;
    layout->addWidget(new QLabel("Unity.exe path:"), 0, 0);
    layout->addWidget(m_exePath, 0, 1);
    layout->addWidget(browseExe, 0, 2);

    layout->addWidget(new QLabel("Unity project folder:"), 1, 0);
    layout->addWidget(m_projectPath, 1, 1);
    layout->addWidget(browseProject, 1, 2);

    layout->addWidget(new QLabel("Build folder:"), 2, 0);
    layout->addWidget(m_outputPath, 2, 1);
    layout->addWidget(browseOutput, 2, 2);

    layout->addWidget(new QLabel("Login:"), 3, 0);
    layout->addWidget(m_loginLineEdit, 3, 1);
    layout->addWidget(new QLabel("Password:"), 4, 0);
    layout->addWidget(m_passwordLineEdit, 4, 1);

    layout->addWidget(m_launchEditor, 5, 0);
    layout->addWidget(m_exitAfterBuild, 5, 1);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignCenter);
    buttonsLayout->addWidget(saveConfigButton);
    buttonsLayout->addWidget(m_buildButton);

    layout->addLayout(buttonsLayout, 6, 0, 1, 3);

    m_fakeBuildProgressBar = new QProgressBar;
    m_fakeBuildProgressBar->hide();
    layout->addWidget(m_fakeBuildProgressBar, 7, 0, 1, 3);

    setLayout(layout);

    // CONNECTS
    connect(browseExe, &QPushButton::clicked, this, &MainWindow::browseExe);
    connect(browseProject, &QPushButton::clicked, this, &MainWindow::browseProject);
    connect(browseOutput, &QPushButton::clicked, this, &MainWindow::browseOutput);
    connect(saveConfigButton, &QPushButton::clicked, this, &MainWindow::saveConf);
    connect(m_buildButton, &QPushButton::clicked, this, &MainWindow::build);
    connect(m_launchEditor, &QCheckBox::stateChanged, this, &MainWindow::updateExitAfterBuild);

    // THREAD CONFIG
    Builder* builder = new Builder;
    builder->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, builder, &QObject::deleteLater);
    connect(this, &MainWindow::launchBuild, builder, &Builder::doBuild);
    connect(builder, &Builder::buildFinished, this, &MainWindow::handleBuildFinished);
    workerThread.start();

    // LOAD CONFIG
    QString configFile("config.json");
    QFile file(configFile);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray fileContent = file.readAll();
        QJsonDocument jsonContent(QJsonDocument::fromJson(fileContent));

        QTextStream(stdout) << "Loaded conf: \n"
                                << "exePath:" << jsonContent["exePath"].toString() << "\n"
                                << "projectPath:" << jsonContent["projectPath"].toString() << "\n"
                                << "outputPath:" << jsonContent["outputPath"].toString() << "\n"
                                << "login:" << jsonContent["login"].toString() << "\n"
                                << "password:" << "**********" << "\n"
                                << "launchEditor:" << (jsonContent["launchEditor"].toBool() == 1 ? "true" : "false") << "\n"
                                << "exitAfterBuild:" << (jsonContent["exitAfterBuild"].toBool() == 1 ? "true" : "false") << "\n"
                                   ;

        m_exePath->setText(jsonContent["exePath"].toString());
        m_projectPath->setText(jsonContent["projectPath"].toString());
        m_outputPath->setText(jsonContent["outputPath"].toString());
        m_loginLineEdit->setText(jsonContent["login"].toString());
        m_passwordLineEdit->setText(jsonContent["password"].toString());
        m_launchEditor->setChecked(jsonContent["launchEditor"].toBool());
        m_exitAfterBuild->setChecked(jsonContent["exitAfterBuild"].toBool());

        updateExitAfterBuild(jsonContent["launchEditor"].toBool());
    }  
}

void MainWindow::updateExitAfterBuild(int newCheckState) {
    if(newCheckState == Qt::Unchecked) {
        m_exitAfterBuild->setEnabled(false);
    } else {
        m_exitAfterBuild->setEnabled(true);
    }
}

void MainWindow::browseExe() {
    QString exePath = QFileDialog::getOpenFileName(nullptr, QString(), "C:\\Program Files\\Unity\\Hub\\Editor", QString("Executable (*.exe)"));
    m_exePath->setText(exePath);
}

void MainWindow::browseProject() {
    QString projectPath = QFileDialog::getExistingDirectory(nullptr, QString(), QString());
    m_projectPath->setText(projectPath);

    if(m_outputPath->text().isEmpty()) {
        m_outputPath->setText(projectPath + "\\Builds\\Windows");
    }
}

void MainWindow::browseOutput() {
    QString outputPath = QFileDialog::getExistingDirectory(nullptr, QString(), QString());
    m_outputPath->setText(outputPath);
}

void MainWindow::saveConf() {
    QFile file(QString("config.json"));
    if(file.open(QIODevice::WriteOnly)) {
        QJsonObject jsonContent;
        jsonContent["exePath"] = m_exePath->text();
        jsonContent["projectPath"] = m_projectPath->text();
        jsonContent["outputPath"] = m_outputPath->text();
        jsonContent["login"] = m_loginLineEdit->text();
        jsonContent["password"] = m_passwordLineEdit->text();
        jsonContent["launchEditor"] = m_launchEditor->isChecked();
        jsonContent["exitAfterBuild"] = m_exitAfterBuild->isChecked();

        QJsonDocument saveDoc(jsonContent);

        file.write(saveDoc.toJson());
    }
}

// Creates the folder if it's empty
void MainWindow::build() {
    // TODO: checks
    QString line = "\"" + m_exePath->text() + "\" -projectPath \"" + m_projectPath->text() + "\" -buildWindows64Player \"" + m_outputPath->text() + "/Windows.exe\"";
    if(m_launchEditor->isChecked()) {
        line += " -quit";
    } else {
        line += " -batchmode -nographics -quit";
    }
    line += " -username " + m_loginLineEdit->text() + " -password " + m_passwordLineEdit->text();
    line += " -stackTraceLogType Full";

    QTextStream(stdout) << line << "\n";

    m_commandLine->setText(line);

    m_buildButton->setEnabled(false);

    m_fakeBuildProgressBar->show();
    m_fakeBuildProgressBar->setValue(0);

    emit launchBuild(line);

    m_fakeBuildProgressBar->setMaximum(0);
    m_fakeBuildProgressBar->setMinimum(0);

    /*
    m_timerForFakeProgressBar = new QTimer;
    connect(m_timerForFakeProgressBar, &QTimer::timeout, this, [=](){
        m_fakeBuildProgressBar->setValue();
    });
    m_timerForFakeProgressBar->start(100);*/
}

void MainWindow::handleBuildFinished(int code) {
    m_buildButton->setEnabled(true);
    m_fakeBuildProgressBar->setMaximum(100);
    m_fakeBuildProgressBar->setValue(100);
    m_fakeBuildProgressBar->setValue(100);

    QTextStream(stdout) << code << "\n";
    if(code == 0) {
        // Success
        // TODO: go to, run
        QMessageBox::information(this, "Success", "Build was successful");
    } else {
        // Error
        // TODO: find and display logs
        QMessageBox::critical(this, "Error", "Build was not successful");
    }
}

MainWindow::~MainWindow() {
    workerThread.quit();
    workerThread.wait();
}
