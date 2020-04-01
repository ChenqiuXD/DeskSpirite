#include "deskspirite.h"
#include "ui_deskspirite.h"

DeskSpirite::DeskSpirite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeskSpirite)
{
    ui->setupUi(this);

    // Fix the size of window to prevent misconduct
    this->setFixedSize(100, 160);

    // Move the spirite to the center of the Screen
    QScreen *screen=QGuiApplication::primaryScreen ();
    QRect mm=screen->availableGeometry() ;
    int screen_width = mm.width();
    int screen_height = mm.height();
    this->move(QPoint(screen_width/2, screen_height/2));

    updateId = startTimer(60);
    currentFrame = 0;
    imgPrefix = "/home/davlee/QtCode/try/Madoka/idle/000";
    imgSuffix = ".png";

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

void DeskSpirite::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == updateId) {
        update();// use paintEvent to update
    }
}

void DeskSpirite::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    imgName = imgPrefix + std::to_string(currentFrame) + imgSuffix;
    image.load(QString::fromStdString(imgName));
    currentFrame += 1;

    if(currentFrame>=8){currentFrame = 0;}

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

void DeskSpirite::on_DeskSpirite_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);

    QMenu *pMenu = new QMenu(this);
    QAction *pClose = new QAction(tr("Close spirite"), this);
    QAction *pAttack = new QAction(tr("Attack"), this);
    QAction *pJump = new QAction(tr("Jump"), this);

    pClose->setData(1);
    pAttack->setData(2);
    pJump->setData(3);

    //Add the action onto the menu
    pMenu->addAction(pClose);
    pMenu->addAction(pAttack);
    pMenu->addAction(pJump);

    //Connect the signal and slot
    connect(pClose, &QAction::triggered, this, &DeskSpirite::onTaskBoxContextMenuEvent);
    connect(pAttack, &QAction::triggered, this, &DeskSpirite::onTaskBoxContextMenuEvent);
    connect(pJump, &QAction::triggered, this, &DeskSpirite::onTaskBoxContextMenuEvent);

    //Display where the mouse clicked
    pMenu->exec(cursor().pos());

    //release
    QList<QAction*> list = pMenu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete pMenu;
}

void DeskSpirite::onTaskBoxContextMenuEvent()
{
    QAction *pEvent = qobject_cast<QAction *>(this->sender());
    int iType = pEvent->data().toInt();

    switch (iType)
    {
    case 1:
        QApplication::quit();
        break;
    case 2:
        break;
    case 3:
        break;
    default:
        break;
    }
}

