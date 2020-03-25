#ifndef DESKSPIRITE_H
#define DESKSPIRITE_H

#include <QWidget>
#include <QtGui>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>

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

private:
    bool mousePress;
    QPoint movePoint;

    Ui::DeskSpirite *ui;

    // Use the following three function to drag the spirite around
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

};

#endif // DESKSPIRITE_H
