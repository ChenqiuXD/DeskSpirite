#ifndef DESKSPIRITE_H
#define DESKSPIRITE_H

#include <stddef.h>
#include <string.h>
#include <iostream>
#include <vector>

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
#include <QDebug>

using namespace std;

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
    // Some frequent changeparameters:
    int FPS_FREQ = 80;
    int WIN_SIZE_WIDTH = 160;
    int WIN_SIZE_HEIGHT = 160;
    int STAT_CHG_COUNT_IDL_PAUSED = 5;
    // Different action has different pic size: paused, idle -> 156 × 96, run, attack etc. -> 128 × 128
    int ORIGIN_LEFT_UP[2] = {10, 10};
    int ORIGIN_RIGHT_DOWN_1[2] = {106, 166};
    int ORIGIN_RIGHT_DOWN_2[2] = {138, 138};

    // The state of deskspirite
    enum State{paused, idle, draging, drop, attack};
    int frameCount[5] = {10, 8, 1, 9, 9};    // Number of frames corresponding to the enum index
    int stateNum;
    int idleCount; //Used to transfer between idle and paused state (Always being idle is too annoying...)

    bool mousePress;
    QPoint movePoint;
    int updateId;
    int idleTimerId;
    int currentFrame;
    string imgCurPrefix;
    vector<string> imgPrefixs;
    string imgSuffix;
    string imgName;
    QString img;
    QPixmap image;
    int iterState;  //Used to change states between idle and paused

    Ui::DeskSpirite *ui;

    // Use the following three function to drag the spirite around
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void onTaskBoxContextMenuEvent();
    void timerEvent(QTimerEvent *event);
};

#endif // DESKSPIRITE_H
