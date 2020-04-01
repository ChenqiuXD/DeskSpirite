#ifndef DESKSPIRITE_H
#define DESKSPIRITE_H

#include <stddef.h>
#include <string.h>
#include <iostream>

#include <QWidget>
#include <QtGui>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QAction>
#include <QScreen>
#include <QTimer>

namespace Ui {
class DeskSpirite;
}

class DeskSpirite : public QWidget
{
    Q_OBJECT

public:
    explicit DeskSpirite(QWidget *parent = 0);
    ~DeskSpirite();

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_DeskSpirite_customContextMenuRequested(const QPoint &pos);

private:
    bool mousePress;
    QPoint movePoint;
    int updateId;
    int currentFrame;
    std::string imgPrefix;
    std::string imgSuffix;
    std::string imgName;
    QString img;
    QPixmap image;

    Ui::DeskSpirite *ui;

    // Use the following three function to drag the spirite around
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void onTaskBoxContextMenuEvent();
    void timerEvent(QTimerEvent *event);
};

#endif // DESKSPIRITE_H
