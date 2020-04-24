#include "deskspirite.h"
#include "ui_deskspirite.h"

DeskSpirite::DeskSpirite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeskSpirite)
{
    ui->setupUi(this);

    //TODO:
    // Set the windows icon. BUT UNLUCKLY, IT DOES NOT WORK

    this->setFixedSize(WIN_SIZE_WIDTH, WIN_SIZE_HEIGHT);

    // Move the spirite to the center of the Screen
    QScreen *screen=QGuiApplication::primaryScreen ();
    QRect mm=screen->availableGeometry() ;
    int screen_width = mm.width();
    int screen_height = mm.height();
    this->SCREEN_WIDTH = screen_width;
    this->curScreenPos[0] = screen_width*2/3;
    this->curScreenPos[1] = screen_height*7/12;

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
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/jumpLeft/000");
    imgPrefixs.push_back("/home/davlee/QtCode/try/Madoka/jumpRight/000");
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
    // The dragging part position update is in the mouseMove function
    // The two position update blobs are seperated because once integrated, the draging would be slowed.
    if(stateNum != State::draging){
        // Move the window horizontally
        move(QPoint(int(curScreenPos[0]), int(curScreenPos[1])));
        if(curScreenPos[0] + velocity_hor <= this->SCREEN_WIDTH-ORIGIN_RIGHT_DOWN_2[0]/2 &&
           curScreenPos[0] + velocity_hor > 0){
            curScreenPos[0] += velocity_hor;
        }

        // Move the window vertically
        if(stateNum == State::jumpLeft || stateNum == State::jumpRight){
            move(QPoint(int(curScreenPos[0]), int(curScreenPos[1])));
            curScreenPos[1] += velocity_ver;
            velocity_ver += VELOCITY_INTERVAL_VER;
            if(velocity_ver == +(INIT_VER_COEFFICIENT+1) * VELOCITY_INTERVAL_VER){
                if(this->isLeftPressed){
                    stateNum = State::runLeft;
                }else if(this->isRightPressed){
                    stateNum = State::runRight;
                }else{
                    stateNum = State::idle;
                }
                velocity_ver = 0;
            }
        }
    }

    if (event->timerId() == updateId) {
        if(stateNum==State::idle || stateNum==State::paused){
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
        curScreenPos[0] = movePos.x()-movePoint.x();
        curScreenPos[1] = movePos.y()-movePoint.y();
        move(movePos-movePoint);
        event->accept();
    }
}

void DeskSpirite::updateKeyState(QKeyEvent *event)
{
    if(event->type() == QEvent::KeyPress){
        if(event->key() == Qt::Key_Up){
            if(stateNum == State::jumpLeft || stateNum == State::jumpRight){
                return;
            }else if(stateNum == State::runLeft){
                stateNum = State::jumpLeft;
                velocity_ver = -VELOCITY_INTERVAL_VER * INIT_VER_COEFFICIENT;
            }else if(stateNum == State::runRight){
                stateNum = State::jumpRight;
                velocity_ver = -VELOCITY_INTERVAL_VER * INIT_VER_COEFFICIENT;
            }else{
                stateNum = State::jumpRight;
                velocity_ver = -VELOCITY_INTERVAL_VER * INIT_VER_COEFFICIENT;
            }
        }else if(event->key() == Qt::Key_Left){
            if(stateNum != State::jumpLeft && stateNum != State::jumpRight){
                stateNum = State::runLeft;
                velocity_hor = -VELOCITY_INTERVAL_HOR;
            }else{
                velocity_hor = -VELOCITY_INTERVAL_HOR;
            }
            if(!this->isRightPressed){this->isLeftPressed = true;}
        }else if(event->key() == Qt::Key_Right){
            if(stateNum != State::jumpLeft && stateNum != State::jumpRight){
                stateNum = State::runRight;
                velocity_hor = +VELOCITY_INTERVAL_HOR;
            }else{
                velocity_hor = +VELOCITY_INTERVAL_HOR;
            }
            if(!this->isLeftPressed){this->isRightPressed = true;}
        }
    }else if(event->type() == QEvent::KeyRelease){
        if(event->key() == Qt::Key_Left){
            if(stateNum == State::runLeft){
                stateNum = State::idle;
            }
            velocity_hor = 0;
            this->isLeftPressed = false;
        }else if(event->key() == Qt::Key_Right){
            if(stateNum == State::runRight){
                stateNum = State::idle;
            }
            velocity_hor = 0;
            this->isRightPressed = false;
        }
    }

    currentFrame = 0;

}

void DeskSpirite::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()){return;}
    this->updateKeyState(event);
}

void DeskSpirite::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()){return;}
    updateKeyState(event);
}

void DeskSpirite::on_DeskSpirite_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);

    QMenu *pMenu = new QMenu(this);
    QAction *pClose = new QAction(tr("Close spirite"), this);
    QAction *pAttack = new QAction(tr("Attack"), this);
    QAction *todo = new QAction(tr("Add todo"), this);

    pAttack->setData(1);
    todo->setData(2);
    pClose->setData(3);

    //Add the action onto the menu
    pMenu->addAction(pAttack);
    pMenu->addAction(todo);
    pMenu->addAction(pClose);

    //Connect the signal and slot
    connect(pClose, &QAction::triggered, this, &DeskSpirite::onTaskBoxContextMenuEvent);
    connect(pAttack, &QAction::triggered, this, &DeskSpirite::onTaskBoxContextMenuEvent);
    connect(todo, &QAction::triggered, this, &DeskSpirite::onTaskBoxContextMenuEvent);

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
        dialog = new todoWindow(this);
        dialog->setGeometry(this->width()+width(), this->height()+height(), TODOWIDTH, TODOHEIGHT);
        dialog->exec();
        break;
    case 3:
        QApplication::quit();
        break;
    default:
        break;
    }
}

