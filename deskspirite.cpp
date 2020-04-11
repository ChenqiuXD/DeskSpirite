#include "deskspirite.h"
#include "ui_deskspirite.h"

DeskSpirite::DeskSpirite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeskSpirite)
{
    ui->setupUi(this);

    // Set the windows icon. BUT UNLUCKLY, IT DOES NOT WORK

    // Fix the size of window to minimize the blank area of the window
    this->setFixedSize(WIN_SIZE_WIDTH, WIN_SIZE_HEIGHT);

    // Move the spirite to the center of the Screen
    QScreen *screen=QGuiApplication::primaryScreen ();
    QRect mm=screen->availableGeometry() ;
    int screen_width = mm.width();
    int screen_height = mm.height();
    this->SCREEN_WIDTH = screen_width;
    this->curScreenPos[0] = screen_width*3/4;
    this->curScreenPos[1] = screen_height*2/3;

    updateId = startTimer(FPS_FREQ);
    currentFrame = 0;
    idleCount = 0;
    stateNum = State::idle;
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/paused/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/idle/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/drag/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/jump/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/attack/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/runLeft/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/runRight/000");
    imgSuffix = ".png";

    // Close the menu bar and set the background transparent.
//    this->setWindowFlags(Qt::FramelessWindowHint \
//                         | Qt::WindowStaysOnTopHint\
//                         |Qt::Tool);
//    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

DeskSpirite::~DeskSpirite()
{
    delete ui;
}

void DeskSpirite::timerEvent(QTimerEvent *event)
{
    // The dragging part position update is in the mouseMove function
    // The two position update blobs are seperated because once integrated, the draging would be slowed.
    if(this->stateNum != State::draging){
        // First move the window
        this->move(QPoint(int(this->curScreenPos[0]), int(this->curScreenPos[1])));
        this->curScreenPos[0] += this->velocity;

        int right_border = this->SCREEN_WIDTH-this->ORIGIN_RIGHT_DOWN_2[0]+this->ORIGIN_LEFT_UP[0];
        int left_border = this->ORIGIN_LEFT_UP[0];
        if(this->curScreenPos[0]>=right_border){
            this->curScreenPos[0] = right_border;
        }else if(this->curScreenPos[0]<=left_border){
            this->curScreenPos[0] = left_border;
        }
    }

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
    // Thus the program has to classfy the status and then draw the spirite
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
        this->curScreenPos[0] = movePos.x()-movePoint.x();
        this->curScreenPos[1] = movePos.y()-movePoint.y();
        move(movePos-movePoint);
        event->accept();
    }
}

void DeskSpirite::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left){
        this->stateNum = State::runLeft;
        this->velocity = -this->VELOCITY_INTERVAL;
    }else if(event->key() == Qt::Key_Right){
        this->stateNum = State::runRight;
        this->velocity = +this->VELOCITY_INTERVAL;
    }
}

void DeskSpirite::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Left || event->key() == Qt::Key_Right){
        this->stateNum = State::idle;
        this->velocity = 0;
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

