#include "blinkboardwidget.h"
#include <QFrame>

BlinkBoardWidget::BlinkBoardWidget(QWidget *parent)
    : QScrollArea(parent), m_actionButton(new QPushButton(tr("Connect to BlinkBoard"))),
      m_mainLayout(new QVBoxLayout()), m_groupBox(new QGroupBox("")) {

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
}

void BlinkBoardWidget::updateContent() {
  // Update your widget content here
}

void BlinkBoardWidget::resetLayout() {
  // Reset your widget layout here
}

void BlinkBoardWidget::handleButtonClick() {
  // Handle button clicks here
}
