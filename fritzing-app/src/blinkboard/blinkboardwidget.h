#ifndef BLINKBOARDWIDGET_H
#define BLINKBOARDWIDGET_H

#include <QGroupBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

class BlinkBoardWidget : public QScrollArea {
  Q_OBJECT
public:
  BlinkBoardWidget(QWidget *parent = nullptr);
  ~BlinkBoardWidget() = default;

  void updateContent();
  void resetLayout();

protected slots:
  void handleButtonClick();

protected:
  QPushButton *m_actionButton;
  QVBoxLayout *m_mainLayout;
  QGroupBox *m_groupBox;
};
#endif
