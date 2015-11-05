
#include "padnavigator.h"
#include <QtGui/QtGui>


int main(int argc, char *argv[])
{
 QApplication app(argc, argv);
 Q_INIT_RESOURCE(padnavigator);

 PadNavigator navigator(QSize(3,3));

#if defined(Q_QS_SYMBIAN)
 navigator.showMaximized();
#else
 navigator.show();
#endif
 return app.exec();

}


