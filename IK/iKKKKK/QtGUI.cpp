#include "QtGUI.h"

MyMainWindow::MyMainWindow(QWidget *parent):QMainWindow(parent) 
{
    
    // Button for select BVH file
    selectFile->move(1700,100);
    selectFile->resize(200,30);

    connect(selectFile,&QPushButton::clicked,this,&MyMainWindow::selectFileDialog);
    
    // Button for create new movement
    createAnimation->move(1700, 200);
    createAnimation->resize(200, 30);
    connect(createAnimation, &QPushButton::clicked, this, &MyMainWindow::createAnimationOn);


    // check box
    damped->move(1700,300);
    connect(damped, &QCheckBox::clicked, this, &MyMainWindow::checkDamped);

    // Label for lambda
    forLambda->move(1700, 360);
    forLambda->resize(200, 30);

    // Slider
    lambdaValue->move(1700,400);
    lambdaValue->resize(200, 30);
    lambdaValue->setMinimum(1);
    lambdaValue->setMaximum(51);
    lambdaValue->setOrientation(Qt::Horizontal);
    connect(lambdaValue,&QSlider::valueChanged, this, &MyMainWindow::checkDamped);


    // Button for adding new frame
    createFrame->move(1700, 500);
    createFrame->resize(200, 30);
    connect(createFrame, &QPushButton::clicked, this, &MyMainWindow::createNewFrame);

    // Label for frame time
    forFrameTime->move(1700, 560);
    forFrameTime->resize(200, 30);
    
    // Line editor for frame speed
    speed->move(1700, 600);
    speed->resize(200, 30);
    


    // Button for create new bvh file
    generateFile->move(1700, 700);
    generateFile->resize(200, 30);
    connect(generateFile, &QPushButton::clicked, this, &MyMainWindow::createNewFile);

}

void MyMainWindow::selectFileDialog() 
{
    
    openglwidget = new ReadbvhFile(this, 2);
    
    directory = QFileDialog::getOpenFileName(this,tr("Choose a file"),QDir::currentPath());
    
    // turn directory into char*
    QByteArray ba = directory.toLatin1();
    

    if (moveFigureShown == 1) {
        movefigure->hide();
        moveFigureShown = 0;
    }


    if (!directory.isEmpty()) {
    
    // Animation display window
        openglwidget->loadBVHFile(ba.data());
        openglwidget->resize(1600, 1000);
        openglwidget->move(QPoint(0, 0));
        openglwidget->setParent(this);
        openglwidget->show();
        
        // update combo box
        QStringList strList;

        for (int i = 0; i < openglwidget->joints.size(); i++) {
            strList << openglwidget->joints[i]->name.c_str();
        }

        // set the flag to be 1
        animationShown = 1;
    }

    
}

void MyMainWindow::createAnimationOn() {
    
    // check the flag to make sure the openglwidget is shown, so that we can hide it
    if (animationShown == 1 ) {
        (*openglwidget).hide();
        animationShown = 0;
    }
    movefigure = new MoveFigure(this, 2);
    // resize the size of the window
    qDebug() << "check clean up";

    movefigure->resize(1600, 1000);
    
    // update combo box
    QStringList strList;

    for (int i = 0; i < movefigure->joints.size(); i++) {
        strList << movefigure->joints[i]->name.c_str();
    }

    // change flag
    moveFigureShown = 1;

    // show figure
    movefigure->show();
    
}

void MyMainWindow::checkDamped() 
// # check if use damped mode
{
    // check if is move figure mode now
    if (moveFigureShown == 1) 
    {
        movefigure->useLambda = damped->isChecked();
        movefigure->lambda = lambdaValue->value();
    
    }
    
}

void MyMainWindow::createNewFrame() 
{
    if (moveFigureShown == 1) 
    {
        // open frameData file to record frame data
        movefigure->frameData.open("frameData.bvh", std::ios::in|std::ios::out|std::ios::app);

        // check if the file opened successfully
        if (!movefigure->frameData.is_open())
            return;

        // add frame data into the frameData file
        for (int i = 0; i < movefigure->channels.size(); i++)
        {
            movefigure->frameData << std::setiosflags(std::ios::fixed)<<std::setprecision(6)<<movefigure->data[i] << " ";
            
        }
        movefigure->frameData << "\n";

        // add one to the frameNumForNewFile to record the number of frames
        movefigure->frameNumForNewFile++;

        

        // close the file
        movefigure->frameData.close();
    }
}

void MyMainWindow::createNewFile() 
{
    if (moveFigureShown == 1) 
    {
        
        // open files
        movefigure->BVHFile.open("bvhFileHere.bvh", std::ios::in | std::ios::out | std::ios::app);
        movefigure->frameData.open("frameData.bvh", std::ios::in|std::ios::out|std::ios::app);
        
        // check if the file is opened successfully
        if (!movefigure->BVHFile.is_open() || !movefigure->frameData.is_open())
            return;


        // add frame number into bvhfile
        movefigure->BVHFile << "Frames: ";
        movefigure->BVHFile << movefigure->frameNumForNewFile <<"\n";

        // add frame time
        
            // check user 's input time
        double frameTime = speed->text().toDouble();
        
        qDebug() << frameTime;
        if (frameTime > 1 || frameTime <= 0)
            frameTime = 0.5;

        movefigure->BVHFile << "Frame Time: "<<frameTime<<"\n";

        // create line
        std::string line;
        
        while (!movefigure->frameData.eof())
        {
            // get line in framedata
            getline(movefigure->frameData, line);

            
            if (line == "")
            {
                break;
            }
            // write line into BVHFile
            movefigure->BVHFile << line<<"\n";
            

        }

        // close the files
        movefigure->BVHFile.close();
        movefigure->frameData.close();
    }
}

bool MyMainWindow::event(QEvent* e) 
// GOAL: drag screen to adjust view
{

    QMouseEvent* mouse = dynamic_cast<QMouseEvent*>(e);
    
    if (e->type() == QEvent::MouseButtonPress) {
        previousCoord[0] = mouse->x();
        previousCoord[1] = mouse->y();

        // check if click on a joint
        if (moveFigureShown == 1) 
        {
            movefigure->ClickJoint(previousCoord[0], previousCoord[1]);
        }
    }

    if (e->type() == QEvent::MouseMove && mouse->x()<1600) 
    // GOAL:
        // draw the joints of the figure
    {   
        qDebug() << "check drag";
        // check if we are in move figure mode
        if (moveFigureShown == 1) 
        {
            // check if we select a joint by clicking
            if (movefigure->clickedJoint > -1) 
            {
                // check how far did user drag on the screen
                movefigure->setGoalPos( mouse->x(), mouse->y());
                
                movefigure->CalculateAngle(0);
            
            }
        
        }
        
        
        
        



        //// calculate the rotation angle
        //rotateViewAngle = 360 * shiftAmount/width();

        //if (animationShown == 1)
        //    openglwidget->rotateAngle = rotateViewAngle;
        //    
        //else if (moveFigureShown == 1)
        //    movefigure->rotateAngle = rotateViewAngle;

    }


    if (e->type() == QEvent::MouseButtonRelease) 
    // GOAL:
        // clean the clicked joint index
    {
        if (moveFigureShown == 1) 
        {  
            movefigure->clickedJoint = -1;
        }
    }


    return QWidget::event(e);
}