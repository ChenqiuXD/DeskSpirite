#include "deskspirite.h"
#include "ui_deskspirite.h"

DeskSpirite::DeskSpirite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeskSpirite)
{
    ui->setupUi(this);


    // Fix the size of window to minimize the blank area of the window
    this->setFixedSize(WIN_SIZE_WIDTH, WIN_SIZE_HEIGHT);

    // Move the spirite to the center of the Screen
    QScreen *screen=QGuiApplication::primaryScreen ();
    QRect mm=screen->availableGeometry() ;
    int screen_width = mm.width();
    int screen_height = mm.height();
    this->move(QPoint(screen_width/2, screen_height/2));

    updateId = startTimer(FPS_FREQ);
    currentFrame = 0;
    idleCount = 0;
    stateNum = State::idle;
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/paused/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/idle/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/drag/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/jump/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/attack/000");
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
        if(stateNum==State::idle || stateNum==State::paused){
            // If a iteratioin of frames is completed, increment
            if(currentFrame==frameCount[stateNum]-1) {idleCount++;}
            // Change state between idle and paused
            if(idleCount == STAT_CHG_COUNT_IDL_PAUSED){
                if(stateNum==State::paused) {stateNum=State::idle;}
                else if(stateNum==State::idle)  {stateNum=State::paused;}
                currentFrame = 0;
                idleCount = 0;
            }
        }
        else if(stateNum==State::attack){
            if(currentFrame==frameCount[stateNum]-1){
                stateNum = State::idle;
                currentFrame = 0;
                idleCount = 0;
            }
        }

        update();// use paintEvent to update

        currentFrame += 1;
        if(currentFrame>=frameCount[stateNum]){currentFrame = 0;}
    }
}

void DeskSpirite::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    imgName = imgPrefixs[stateNum] + std::to_string(currentFrame) + imgSuffix;
    image.load(QString::fromStdString(imgName));

    // Different action has different pic size: paused, idle -> 156 × 96, run, attack etc. -> 128 × 128
    QPainter painter(this);
    if(stateNum==State::paused || stateNum==State::idle){
          painter.drawPixmap(ORIGIN_LEFT_UP[0],ORIGIN_LEFT_UP[1],
                             ORIGIN_RIGHT_DOWN_1[0],ORIGIN_RIGHT_DOWN_1[1],image);
    }else{
        painter.drawPixmap(ORIGIN_LEFT_UP[0],ORIGIN_LEFT_UP[1],
                           ORIGIN_RIGHT_DOWN_2[0],ORIGIN_RIGHT_DOWN_2[1],image);
    }

}

void DeskSpirite::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        mousePress = true;
        stateNum = State::draging;
    }
    movePoint = event->globalPos() - pos();
    event->accept();
}

void DeskSpirite::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    stateNum = State::idle;
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

    pAttack->setData(1);
    pClose->setData(2);

    //Add the action onto the menu
    pMenu->addAction(pAttack);
    pMenu->addAction(pClose);

    //Connect the signal and slot
    connect(pClose, &QAction::triggered, this, &DeskSpirite::onTaskBoxContextMenuEvent);
    connect(pAttack, &QAction::triggered, this, &DeskSpirite::onTaskBoxContextMenuEvent);

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
        stateNum = State::attack;
        currentFrame = 0;
        break;
    case 2:
        QApplication::quit();
        break;
    default:
        break;
    }
}

