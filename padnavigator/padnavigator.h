#ifndef PADNAVIGATOR_H
#define PADNAVIGATOR_H
#include<QGraphicsView>

#include "ui_form.h"

class QState;
class QStateMachine;
class Ui_Form;


class PadNavigator : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PadNavigator(const QSize &size, QWidget *parent = 0);


protected:
    void resizeEvent(QResizeEvent *event);


private:
    Ui_Form form;



};

#endif // PADNAVIGATOR_H
