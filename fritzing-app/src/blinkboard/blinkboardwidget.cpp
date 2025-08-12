#include "blinkboardwidget.h"
#include <QFrame>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

BlinkBoardWidget::BlinkBoardWidget(QWidget *parent)
    : QScrollArea(parent), m_actionButton(new QPushButton(tr("test node backend"))),
      m_mainLayout(new QVBoxLayout()), m_groupBox(new QGroupBox("")),
      m_nodeProcess(new QProcess(this)) {

  auto frame = new QFrame(this);

  m_mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  m_mainLayout->setObjectName("BlinkBoardFrame");

  connect(m_actionButton, SIGNAL(clicked()), this, SLOT(handleButtonClick()));

  auto groupLayout = new QVBoxLayout();
  groupLayout->setObjectName("BlinkBoardBoxes");
  groupLayout->addWidget(m_actionButton);

  m_groupBox->setLayout(groupLayout);
  m_mainLayout->addWidget(m_groupBox);
  frame->setLayout(m_mainLayout);

  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setWidget(frame);

  // Connect Node.js process signals
  connect(m_nodeProcess, &QProcess::readyReadStandardOutput, 
          this, &BlinkBoardWidget::handleNodeProcessOutput);
  connect(m_nodeProcess, &QProcess::readyReadStandardError, 
          this, &BlinkBoardWidget::handleNodeProcessError);

  // Add Process State Monitoring - Qt 5 compatible syntax
  connect(m_nodeProcess, SIGNAL(started()), this, SLOT(handleNodeStarted()));
  connect(m_nodeProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), 
          this, SLOT(handleNodeError(QProcess::ProcessError)));
  connect(m_nodeProcess, SIGNAL(finished(int,QProcess::ExitStatus)), 
          this, SLOT(handleNodeFinished(int,QProcess::ExitStatus)));
}

void BlinkBoardWidget::updateContent() {
  // Update your widget content here
}

void BlinkBoardWidget::resetLayout() {
  // Reset your widget layout here
}

void BlinkBoardWidget::handleButtonClick() {
  if (m_nodeProcess->state() == QProcess::NotRunning) {
    qDebug() << "Attempting to start Node.js backend...";
    startNodeBackend();
  } else {
    qDebug() << "Node.js backend already running, sending ping...";
    sendCommandToNode("ping");
  }
}

void BlinkBoardWidget::startNodeBackend() {
  // Use hardcoded path to Node.js
  QString nodePath = "/usr/local/bin/node";
  
  // Check if the hardcoded path exists and is executable
  QFileInfo nodeFileInfo(nodePath);
  if (!nodeFileInfo.exists() || !nodeFileInfo.isExecutable()) {
    qDebug() << "❌ Node.js not found at hardcoded path:" << nodePath;
    m_actionButton->setText("Node.js Not Found");
    return;
  }
  
  qDebug() << "✅ Found Node.js at:" << nodePath;
  
  QStringList arguments;
  
  // Get the path to server.js relative to the source directory
  // Go up from build directory to source directory
  QString currentPath = QDir::currentPath();
  qDebug() << "Current working directory:" << currentPath;
  
  // Navigate up from build directory to source directory
  QString sourcePath = currentPath;
  if (sourcePath.contains("/build/")) {
    // Remove everything from /build/ onwards
    int buildIndex = sourcePath.indexOf("/build/");
    sourcePath = sourcePath.left(buildIndex);
    qDebug() << "Detected build directory, using source path:" << sourcePath;
  }
  
  QString serverPath = sourcePath + "/node-backend/server.js";
  
  qDebug() << "Source directory:" << sourcePath;
  qDebug() << "Server path:" << serverPath;
  
  // Check if file exists
  QFileInfo fileInfo(serverPath);
  if (!fileInfo.exists()) {
    qDebug() << "❌ Server.js file not found at:" << serverPath;
    qDebug() << "Tried to find it in:" << sourcePath;
    m_actionButton->setText("File Not Found - Check Path");
    return;
  }
  
  arguments << serverPath;
  
  qDebug() << "Starting Node.js process...";
  qDebug() << "Node path:" << nodePath;
  qDebug() << "Arguments:" << arguments;
  qDebug() << "Working directory:" << QDir::currentPath();
  
  m_nodeProcess->start(nodePath, arguments);
  
  if (m_nodeProcess->waitForStarted(5000)) { // 5 second timeout
    qDebug() << "✅ Node.js backend started successfully!";
    qDebug() << "Process ID:" << m_nodeProcess->processId();
    qDebug() << "Process state:" << m_nodeProcess->state();
    
    m_actionButton->setText("Connected - Click to Test");
    sendCommandToNode("init");
  } else {
    qDebug() << "❌ Failed to start Node.js backend!";
    qDebug() << "Error:" << m_nodeProcess->errorString();
    qDebug() << "Process state:" << m_nodeProcess->state();
    qDebug() << "Exit code:" << m_nodeProcess->exitCode();
    
    // Reset button text
    m_actionButton->setText("Failed - Click to Retry");
  }
}

void BlinkBoardWidget::sendCommandToNode(const QString &command) {
  if (m_nodeProcess->state() == QProcess::Running) {
    QJsonObject jsonCommand;
    jsonCommand["command"] = command;
    jsonCommand["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    
    QJsonDocument doc(jsonCommand);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    
    qDebug() << "Sending to Node.js:" << jsonString;
    m_nodeProcess->write(jsonString.toUtf8() + "\n");
  } else {
    qDebug() << "Cannot send command - Node.js process not running";
    qDebug() << "Process state:" << m_nodeProcess->state();
  }
}

void BlinkBoardWidget::handleNodeProcessOutput() {
  QString output = QString::fromUtf8(m_nodeProcess->readAllStandardOutput());
  QStringList lines = output.split('\n', QString::SkipEmptyParts);
  
  for (const QString &line : lines) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &error);
    
    if (error.error == QJsonParseError::NoError) {
      QJsonObject response = doc.object();
      qDebug() << "✅ Received from Node.js:" << response;
    } else {
      qDebug() << " Raw output from Node.js:" << line;
    }
  }
}

void BlinkBoardWidget::handleNodeProcessError() {
  QString error = QString::fromUtf8(m_nodeProcess->readAllStandardError());
  qDebug() << "❌ Node.js stderr:" << error;
}

void BlinkBoardWidget::handleNodeStarted() {
  qDebug() << "🎉 Node.js process started signal received!";
  qDebug() << "Process ID:" << m_nodeProcess->processId();
  qDebug() << "Process state:" << m_nodeProcess->state();
}

void BlinkBoardWidget::handleNodeError(QProcess::ProcessError error) {
  qDebug() << "💥 Node.js process error occurred:" << error;
  qDebug() << "Error string:" << m_nodeProcess->errorString();
  qDebug() << "Process state:" << m_nodeProcess->state();
  
  // Update button text
  m_actionButton->setText("Error - Click to Retry");
}

void BlinkBoardWidget::handleNodeFinished(int exitCode, QProcess::ExitStatus exitStatus) {
  qDebug() << "🏁 Node.js process finished!";
  qDebug() << "Exit code:" << exitCode;
  qDebug() << "Exit status:" << exitStatus;
  qDebug() << "Process state:" << m_nodeProcess->state();
  
  // Reset button text
  m_actionButton->setText("Disconnected - Click to Connect");
}
