#include "OpenGLWidget.h"
#include "QtGUI.h"
#include <iostream>
#include <Qlayout>


int main(int argc, char** argv){

    QApplication app(argc, argv);

// Main window
    MyMainWindow *mainWindow = new MyMainWindow();
    mainWindow->resize(2000,1000);

// Set Layout
    
    mainWindow->show();
    return app.exec();
    
    

}
