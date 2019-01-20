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

    // BUTTONS
    QPushButton* buildButton = new QPushButton("Build!");
    m_commandLine = new QLabel;

    QPushButton* saveConfigButton = new QPushButton("Save conf");

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

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignCenter);
    buttonsLayout->addWidget(saveConfigButton);
    buttonsLayout->addWidget(buildButton);

    layout->addLayout(buttonsLayout, 3, 0, 1, 3);

    setLayout(layout);

    // CONNECTS
    connect(browseExe, SIGNAL(clicked()), this, SLOT(browseExe()));
    connect(browseProject, SIGNAL(clicked()), this, SLOT(browseProject()));
    connect(browseOutput, SIGNAL(clicked()), this, SLOT(browseOutput()));
    connect(saveConfigButton, SIGNAL(clicked()), this, SLOT(saveConf()));
    connect(buildButton, SIGNAL(clicked()), this, SLOT(build()));

    // LOAD CONFIG
    QString configFile("config.json");
    QFile file(configFile);
    if(file.open(QIODevice::ReadOnly)) {
        QByteArray fileContent = file.readAll();
        QJsonDocument jsonContent(QJsonDocument::fromJson(fileContent));

        QTextStream(stdout) << "Loaded conf: \n"
                                << "exePath:" << jsonContent["exePath"].toString() << "\n"
                                << "projectPath:" << jsonContent["projectPath"].toString() << "\n"
                                << "outputPath:" << jsonContent["outputPath"].toString() << "\n";

        m_exePath->setText(jsonContent["exePath"].toString());
        m_projectPath->setText(jsonContent["projectPath"].toString());
        m_outputPath->setText(jsonContent["outputPath"].toString());
    }
}

void MainWindow::browseExe() {
    QString exePath = QFileDialog::getOpenFileName(nullptr, QString(), "C:\\Program Files\\Unity\\Hub\\Editor", QString("Executable (*.exe)"));
    m_exePath->setText(exePath);
}

void MainWindow::browseProject() {
    QString projectPath = QFileDialog::getExistingDirectory(nullptr, QString(), QString());
    m_projectPath->setText(projectPath);
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

        QJsonDocument saveDoc(jsonContent);

        file.write(saveDoc.toJson());
    }
}

void MainWindow::build() {
    // TODO: checks
    QString line("\"" + m_exePath->text() + "\" -projectPath \"" + m_projectPath->text() + "\" -buildWindows64Player \"" + m_outputPath->text() + "/Windows.exe\"" + " -batchmode -nographics");
    m_commandLine->setText(line);
    QProcess::execute(line);
}
