#include "flippablepad.h"
#include "padnavigator.h"
#include "splashitem.h"
#include <QGraphicsRotation>

#include <QtGui/QtGui>
#ifndef QT_NO_OPENGL
#include <QtOpenGL/QtOpenGL>
#endif


PadNavigator::PadNavigator(const QSize &size, QWidget *parent)
    : QGraphicsView(parent)
{
    SplashItem *splash = new SplashItem;
    splash->setZValue(1);

    //Pad item

    FlippablePad *pad = new FlippablePad(size);
    QGraphicsRotation *flipRotation = new QGraphicsRotation(pad);
    QGraphicsRotation *xRotation = new QGraphicsRotation(pad);
    QGraphicsRotation *yRotation = new QGraphicsRotation(pad);
    flipRotation->setAxis(Qt::YAxis);
    xRotation->setAxis(Qt::XAxis);
    yRotation->setAxis(Qt::YAxis);
    pad->setTransformations(QList<QGraphicsTransform *>()
                            << flipRotation
                            << xRotation << yRotation

                            );

    // Back (proxy widget) item
    QGraphicsProxyWidget *backItem = new QGraphicsProxyWidget(pad);
    QWidget *widget = new QWidget;
    form.setupUi(widget);
    form.hostName->setFocus();
    backItem->setWidget(widget);
    backItem->setVisible(false);
    backItem->setFocus();
    backItem->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    const QRectF r = backItem->rect();
    backItem->setTransform(QTransform()
                           .rotate(180, Qt::YAxis)
                           .translate(-r.width()/2, -r.height()/2)
                           );

    // Selection item
    RoundRectItem *selectionItem = new RoundRectItem(QRectF(-60, -60, 120, 120), Qt::gray, pad);

    selectionItem->setZValue(0.5);

    // Splash animations
    QPropertyAnimation *smoothSplashMove = new QPropertyAnimation(splash,"y");
    QPropertyAnimation *smoothSplashOpacity = new QPropertyAnimation(splash, "opacity");
    smoothSplashMove->setEasingCurve(QEasingCurve::InQuad);
    smoothSplashMove->setDuration(250);
    smoothSplashOpacity->setDuration(250);

    // Selection animation
    QPropertyAnimation *smoothXSelection = new QPropertyAnimation(selectionItem, "x");
    QPropertyAnimation *smoothYSelection = new QPropertyAnimation(selectionItem, "y");
    QPropertyAnimation *smoothXRotation = new QPropertyAnimation(xRotation, "angle");
    QPropertyAnimation *smoothYRotation = new QPropertyAnimation(yRotation, "angle");
    smoothXSelection->setDuration(125);
    smoothYSelection->setDuration(125);
    smoothXRotation->setDuration(125);
    smoothYRotation->setDuration(125);
    smoothXSelection->setEasingCurve(QEasingCurve::InOutQuad);
    smoothYSelection->setEasingCurve(QEasingCurve::InOutQuad);
    smoothXRotation->setEasingCurve(QEasingCurve::InOutQuad);
    smoothYRotation->setEasingCurve(QEasingCurve::InOutQuad);

    // Flip animation setup
    QPropertyAnimation *smoothFlipRotation = new QPropertyAnimation(flipRotation, "angle");
    QPropertyAnimation *smoothFlipScale = new QPropertyAnimation(pad, "scale");
    QPropertyAnimation *smoothFlipXRotation = new QPropertyAnimation(xRotation, "angle");
    QPropertyAnimation *smoothFlipYRotation = new QPropertyAnimation(yRotation, "angle");
    QParallelAnimationGroup *flipAnimation = new QParallelAnimationGroup(this);
    smoothFlipScale->setDuration(500);
    smoothFlipRotation->setDuration(500);
    smoothFlipXRotation->setDuration(500);
    smoothFlipYRotation->setDuration(500);
    smoothFlipScale->setEasingCurve(QEasingCurve::InOutQuad);
    smoothFlipRotation->setEasingCurve(QEasingCurve::InOutQuad);
    smoothFlipXRotation->setEasingCurve(QEasingCurve::InOutQuad);
    smoothFlipYRotation->setEasingCurve(QEasingCurve::InOutQuad);
    smoothFlipScale->setKeyValueAt(0, qvariant_cast<qreal>(1.0));
    smoothFlipScale->setKeyValueAt(0.5, qvariant_cast<qreal>(0.7));
    smoothFlipScale->setKeyValueAt(1, qvariant_cast<qreal>(1.0));
    flipAnimation->addAnimation(smoothFlipRotation);
    flipAnimation->addAnimation(smoothFlipScale);
    flipAnimation->addAnimation(smoothFlipXRotation);
    flipAnimation->addAnimation(smoothFlipYRotation);

    // Flip animation delayed property assignment
    QSequentialAnimationGroup *setVariablesSequence = new QSequentialAnimationGroup;
    QPropertyAnimation *setFillAnimation = new QPropertyAnimation(pad, "fill");
    QPropertyAnimation *setBackItemVisibleAnimation = new QPropertyAnimation(backItem, "visible");
    QPropertyAnimation *setSelectionItemVisibleAnimation = new QPropertyAnimation(selectionItem, "visible");
    setFillAnimation->setDuration(0);
    setBackItemVisibleAnimation->setDuration(0);
    setVariablesSequence->addPause(250);
    setVariablesSequence->addAnimation(setBackItemVisibleAnimation);
    setVariablesSequence->addAnimation(setSelectionItemVisibleAnimation);
    setVariablesSequence->addAnimation(setFillAnimation);
    flipAnimation->addAnimation(setVariablesSequence);

    //Build the state machine
    QStateMachine *stateMachine = new QStateMachine(this);
    QState *splashState = new QState(stateMachine);
    QState *frontState = new QState(stateMachine);
    QHistoryState *historyState = new QHistoryState(frontState);
    QState *backState = new QState(stateMachine);
    frontState->assignProperty(pad, "fill" , false);
    frontState->assignProperty(splash, "opacity", 0.0);
    frontState->assignProperty(backItem, "visible", false);
    frontState->assignProperty(flipRotation, "angle", qvariant_cast<qreal>(0.0));
    frontState->assignProperty(selectionItem, "visible", true);
    backState->assignProperty(pad, "fill", true);
    backState->assignProperty(backItem, "visible", true);
    backState->assignProperty(xRotation, "angle", qvariant_cast<qreal>(0.0));
    backState->assignProperty(yRotation, "angle", qvariant_cast<qreal>(180.0));
    backState->assignProperty(selectionItem, "visible", false);
    stateMachine->addDefaultAnimation(smoothXRotation);
    stateMachine->addDefaultAnimation(smoothYRotation);
    stateMachine->addDefaultAnimation(smoothXSelection);
    stateMachine->addDefaultAnimation(smoothYSelection);
    stateMachine->setInitialState(splashState);

    //Transitions
    QEventTransition *anyKeyTransition = new QEventTransition(this, QEvent::KeyPress, splashState );
    anyKeyTransition->setTargetState(frontState);
    anyKeyTransition->addAnimation(smoothSplashMove);
    anyKeyTransition->addAnimation(smoothSplashOpacity);

    QKeyEventTransition *enterTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Enter, backState);
    QKeyEventTransition *returnTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Return, backState);
    QKeyEventTransition *backEnterTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Enter, frontState);
    QKeyEventTransition *backReturnTransition = new QKeyEventTransition(this, QEvent::KeyPress, Qt::Key_Return, frontState);

    enterTransition->setTargetState(historyState);
    returnTransition->setTargetState(historyState);
    backEnterTransition->setTargetState(backState);
    backReturnTransition->setTargetState(backState);
    enterTransition->addAnimation(flipAnimation);
    returnTransition->addAnimation(flipAnimation);
    backEnterTransition->addAnimation(flipAnimation);
    backReturnTransition->addAnimation(flipAnimation);

    //Create substates for each icon; store in temporary grid
    int columns = size.width();
    int rows = size.height();
    QVector< QVector< QState * > > stateGrid;
    stateGrid.resize(rows);
    for( int y = 0; y < rows; ++y)
    {
        stateGrid[y].resize(columns);
        for (int x = 0; x < columns; ++x)
            stateGrid[y][x] = new QState(frontState);
    }

    frontState->setInitialState(stateGrid[0][0]);
    selectionItem->setPos(pad->iconAt(0,0)->pos());

    // Enable key navigation using state transitions
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            QState *state = stateGrid[y][x];
                QKeyEventTransition *rightTransition = new QKeyEventTransition(this, QEvent::KeyPress,
                                                                                      Qt::Key_Right, state);
                QKeyEventTransition *leftTransition = new QKeyEventTransition(this, QEvent::KeyPress,
                                                                                     Qt::Key_Left, state);
                QKeyEventTransition *downTransition = new QKeyEventTransition(this, QEvent::KeyPress,
                                                                                     Qt::Key_Down, state);
                QKeyEventTransition *upTransition = new QKeyEventTransition(this, QEvent::KeyPress,
                                                                                   Qt::Key_Up, state);
                rightTransition->setTargetState(stateGrid[y][(x + 1) % columns]);
                leftTransition->setTargetState(stateGrid[y][((x - 1) + columns) % columns]);
                downTransition->setTargetState(stateGrid[(y + 1) % rows][x]);
                upTransition->setTargetState(stateGrid[((y - 1) + rows) % rows][x]);

                RoundRectItem *icon = pad->iconAt(x, y);
                state->assignProperty(xRotation,"angle" , -icon->x() / 6.0);
                state->assignProperty(yRotation, "angle" , icon->y() / 6.0);
                state->assignProperty(selectionItem, "x", icon->x());
                state->assignProperty(selectionItem, "y", icon->y());
                frontState->assignProperty(icon, "visible", true);
                backState->assignProperty(icon, "visible", false);

                QPropertyAnimation *setIconVisibleAnimation = new QPropertyAnimation(icon, "visible");
                setIconVisibleAnimation->setDuration(0);
                setVariablesSequence->addAnimation(setIconVisibleAnimation);

        }

    }


    //Scene
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QPixmap(":/images/blue_angle_swirl.jpg"));
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->addItem(pad);
    scene->setSceneRect(scene->itemsBoundingRect());
    setScene(scene);

    //Adjust splash item to scene contents;
    const QRectF sbr = splash->boundingRect();
    splash->setPos(-sbr.width() / 2, scene->sceneRect().top() - 2);
    frontState->assignProperty(splash, "y" , splash->y() - 100.0);
    scene->addItem(splash);

    //View
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMinimumSize(50, 50);
    setViewportUpdateMode(FullViewportUpdate);
    setCacheMode(CacheBackground);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
#ifndef QT_NO_OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif


    stateMachine->start();

}

void PadNavigator::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene()->sceneRect(),Qt::KeepAspectRatio);
}

