#include "deskspirite.h"
#include "ui_deskspirite.h"

DeskSpirite::DeskSpirite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeskSpirite)
{
    ui->setupUi(this);

    // Fix the size of window to prevent misconduct
    this->setFixedSize(100, 160);

    // Close the menu bar and set the background transparent.
    this->setWindowFlags(Qt::FramelessWindowHint \
                         | Qt::WindowStaysOnTopHint\
                         |Qt::Tool);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

DeskSpirite::~DeskSpirite()
{
    delete ui;
}

void DeskSpirite::paintEvent(QPaintEvent* event)
{
    QPixmap image("/home/davlee/QtCode/try/Madoka/idle/0000.png");

    QPainter painter(this);
    painter.drawPixmap(10,10,106,166,image);
}

void DeskSpirite::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        mousePress = true;
    }
    movePoint = event->globalPos() - pos();
    event->accept();
}

void DeskSpirite::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mousePress = false;
    event->accept();
}

void DeskSpirite::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePress){
        QPoint movePos = event->globalPos();
        move(movePos-movePoint);
        event->accept();
    }
}
