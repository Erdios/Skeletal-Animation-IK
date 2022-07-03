#include "MoveFigure.h" 


MoveFigure::MoveFigure(QWidget* parent, float _scale):ReadbvhFile(parent,_scale) {
    
    openRestFile();
    createNewFile();
    if (motion != nullptr) {
        data = motion;
    }
    
    // initialize QVector4D for the parennt
    firstJoint.setX(0);
    firstJoint.setY(0);
    firstJoint.setZ(0);
    firstJoint.setW(1.0f);

    goalPos[0] = 0;
    goalPos[1] = 0;
    goalPos[2] = 0;
    goalPos[3] = 0;

    animationOn = 0;

    endEffectorID = -1;
    clickedJoint = -1;
    useLambda = false;
    frameNumForNewFile = 0;
}

void MoveFigure::cleanUp() 
{
    openRestFile();
    createNewFile();
    if (motion != nullptr) {
        data = motion;
    }

    // initialize QVector4D for the parennt
    firstJoint.setX(0);
    firstJoint.setY(0);
    firstJoint.setZ(0);
    firstJoint.setW(1.0f);

    goalPos[0] = 0;
    goalPos[1] = 0;
    goalPos[2] = 0;
    goalPos[3] = 0;

    animationOn = 0;

    endEffectorID = -1;
    clickedJoint = -1;
    useLambda = false;
    frameNumForNewFile = 0;
}


void MoveFigure::openRestFile(){

    Load("rest.bvh");

}

void MoveFigure::createNewFile() 
{
    // open a new file and a rest.bvh file
    BVHFile.open("bvhFileHere.bvh", std::ios::in|std::ios::out|std::ios::trunc);
    frameData.open("frameData.bvh", std::ios::in|std::ios::out|std::ios::trunc);
    


    std::fstream BVHFileeee;
    BVHFileeee.open("rest.bvh", std::ios::in);
    
    std::string line;
    
    // check if files are opened successfully
    if (!BVHFile.is_open() || !BVHFileeee.is_open() || !frameData.is_open()) {
        
        return;
    }
   
    
    // copy basic structure from 'rest.bvh' file
    for (int count = 0; (!BVHFileeee.eof()); count++) {
   		
   	    std::getline(BVHFileeee, line);

        // enter the line into the new .bvh file
   	    BVHFile << line << "\n";

        // check if we reach the MOTION part, if so, we stop copying the content from rest.bvh
        if (line == "MOTION")
        {
            break;
        }
        
    }


    // close files
    BVHFile.close();
    frameData.close();
    BVHFileeee.close();
    
}


void MoveFigure::setEndEffectorID(int EID)
{

    endEffectorID = EID;

}


void MoveFigure::getJointsLink(int EID)
{
    if (EID == -1) 
    {
        return;
    }


    // find the whole link between hip to the end effector
    jointsLink.clear();
    jointsLink.push_back(joints[EID]);

    while (jointsLink.back()->parent != NULL) {
        
        jointsLink.push_back(jointsLink.back()->parent);
    }

}


void MoveFigure::setFrame(int _frameNum)
{

    data = motion + _frameNum * num_channel;

}


void MoveFigure::setScale(double _scale)
{
    scale = _scale;
}


void MoveFigure::ClickJoint(float _x, float _y)  
{
    // get screen coordinate
    float x = _x;
    float y = height() - _y;// inverse because the differences between qt screen coord sys and opengl screen coord sys
    qDebug() << "screen coord:" << x << "," << y;

    
    // prepare matrix for calculating screen coordinate of the joint
    QMatrix4x4 projectMatrix;
    const qreal retinaScale = devicePixelRatio();
    projectMatrix.viewport(0, 0, width() * retinaScale, height() * retinaScale);
    projectMatrix.ortho(-50.5 * width() / height(), 50.5 * width() / height(), -50.5, 50.5, -150, 150);
    
    
    // initialize clicked Joint index
    clickedJoint = -1;
    
    
    // compare the clicked point coord and the joint coord
    for (int i=0;i<joints.size();i++)
    {// per joint

        
        // get the world cooridnate of the clicked joint
        endEffectorPos = ForwardKinematicsGeneral(i) * firstJoint;

        EEScreenPos = projectMatrix * endEffectorPos;

        //* print for debug
        qDebug() <<"end effector:"<< EEScreenPos.x() << "," << EEScreenPos.y();


        // check if the clicked point is within the radius of the current joint
        if (abs(EEScreenPos.x() - x) < 10 * scale && abs(EEScreenPos.y() - y) < 10 * scale)
        {
            // store the clicked joint
            clickedJoint = i;

            
            qDebug() << "EE: " << endEffectorPos;
            setEndEffectorID(i);
            
            //* print for debug
            qDebug() << "Joint index: " << i;
            break;
        }

    }//per joint

}


void MoveFigure::setGoalPos(double x, double y)
// GOAL:
    // get the world coord for goal position
{

    // receive screen position of goal pos
    goalPos[0] = x;
    goalPos[1] = height() - y;
    goalPos[2] = EEScreenPos.z();
    goalPos[3] = EEScreenPos.w();


    // prepare matrix for calculating screen coordinate of the joint
    QMatrix4x4 projectMatrix;
    const qreal retinaScale = devicePixelRatio();
    projectMatrix.viewport(0, 0, width() * retinaScale, height() * retinaScale);
    projectMatrix.ortho(-50.5 * width() / height(), 50.5 * width() / height(), -50.5, 50.5, -150, 150);


    
    // convert screen position of goal pos to world position    
    goalPos = projectMatrix.inverted() * goalPos;


    /*goalPos[0] = (x - endEffectorPos[0])/1000 + endEffectorPos[0];
    goalPos[1] = (height() - y - endEffectorPos[1])/1000 + endEffectorPos[1];
    goalPos[2] = endEffectorPos.z();
    goalPos[3] = endEffectorPos.w();*/


    qDebug() << "Goal pos: " << goalPos;
    qDebug() << "EE pos: " << endEffectorPos;
    
}


void MoveFigure::Jacobian(int EID)
{

    // resize the jacobian based on the number of joints in the whole link
        //  3 is for rotation number per joint, another 3 is because the the coordinate px, py, pz, alpha x, alpha y, alpha z 
    jacobian.clear();
    jacobian.resize(jointsLink.size() * 3 * 3);


    // check if it is possible to go on
    if (jacobian.size() <= 0 || jointsLink.size() <= 0 || data == nullptr)
    {
        return;
    }


    // delta theta for calculating partial derivatives
    double rotateStep = 0.001f;

    // matrix that gain from forward kinematics. for transforming the bone to the right place
    // calculate the original end effector for current frame

    // get the current position of the end effector for current frame
    endEffectorPos = ForwardKinematicsGeneral(clickedJoint) * firstJoint;

    
    // this is for storing value new coordinate for the end effector
    QVector4D newEndEffectorCoord; 
    
    // calculate Jacobian
    for (int i = 0; i < jointsLink.size(); i++)
    { // per channel
    
        // calculate partial derivatives, with orther X_ROTATION -> Y_ROTATION -> Z_ROTATION
        for (int j = 0; j < 3; j++) {

            // get the rotation channel index for this joint
            newEndEffectorCoord = ForwardKinematics(clickedJoint, jointsLink[i]->index, rotateStep, axisID[j]) * firstJoint;
            

            // get partial derivatives
            jacobian[i * 3 + j]                               = ((double)newEndEffectorCoord.x() - (double)endEffectorPos.x()) / rotateStep;
            jacobian[(jointsLink.size() * 3) + i * 3 + j]     = ((double)newEndEffectorCoord.y() - (double)endEffectorPos.y()) / rotateStep;
            jacobian[(jointsLink.size() * 3 * 2) + i * 3 + j] = ((double)newEndEffectorCoord.z() - (double)endEffectorPos.z()) / rotateStep;
        }
    }// per channel


    for (int i = 0; i < jacobian.size(); i++)
    {
        if (isnan(jacobian[i]))
        {
            for (int i = 0; i < jacobian.size(); i++)
            {
                jacobian[i] = 0;
            }
        }
    }


}


Eigen::MatrixXf MoveFigure::PseudoInverse(float* originalMatrix)
{

    Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> m(originalMatrix, 3, jointsLink.size() * 3);

    Eigen::MatrixXf invResult;
    if (useLambda == false) {
        invResult = m.transpose()* (m * m.transpose()).inverse();
    }
    else {
        invResult = m.transpose() * (m * m.transpose() + pow(lambda / 50.0, 2.0) * Eigen::MatrixXf::Identity(m.rows(), m.rows())).inverse();
    }
    for (int i=0;i<invResult.size();i++)
    {
        if (isnan(invResult(i))) 
        {
            invResult.setZero();
        }
    }

    return invResult;

}


void MoveFigure::CalculateAngle(int _frameNum) 
{
    if (endEffectorID < 0) {
        return;
    }
    //  GET¡¡GOAL POSITION
    // get the goal position by adding the shifted value (stored in the goalPos hhhh) with the position of end effector
    
    // Calculate Jacobian
    setFrame(_frameNum);
    setScale(scale);
    getJointsLink(endEffectorID);
    Jacobian(endEffectorID);


    // Goal position minus the original position of end effector
    QVector4D minusResult = goalPos - endEffectorPos;


    // Calculate pseudoinverse
    Eigen::MatrixXf jacInv = PseudoInverse(jacobian.data());

    // Multiple the two matrices and get a group of angle for the joints in jointsLink
    float posDifference[3] = { minusResult[0], minusResult[1], minusResult[2] };


    // turn the difference of 
    Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>> posDiff(posDifference, 3, 1);
    
    // calculate new angle
    auto angle = jacInv * posDiff;
    
    
    // add the new angles into data array to update angles of the joints
    for (int i=0;i<jointsLink.size();i++) 
    {
        for (int j = 0; j < jointsLink[i]->channels.size(); j++)
        {

            
            if (jointsLink[i]->channels[j]->type == axisID[0])
            {
                data[jointsLink[i]->channels[j]->index] = data[jointsLink[i]->channels[j]->index] + angle(i * 3);
            }
            else if (jointsLink[i]->channels[j]->type == axisID[1])
            {
                data[jointsLink[i]->channels[j]->index] = data[jointsLink[i]->channels[j]->index] + angle(i * 3 + 1);
            }
            else if (jointsLink[i]->channels[j]->type == axisID[2])
            {
                data[jointsLink[i]->channels[j]->index] = data[jointsLink[i]->channels[j]->index] + angle(i * 3 + 2);
            }
        
        }
           
    }
}


// calculate the forward kinematics for an end effector
QMatrix4x4 MoveFigure::ForwardKinematics(int EEID, int _jointID_Rotate, double _rotateStep, int _axis)
{
    QMatrix4x4 forwardMatrix;
    double deltaTheta = 0;

    std::vector<Joint*> _jointsLink;

    // get the link for the chosen end effector
    _jointsLink.push_back(joints[EEID]);

    // loop through all the parents to form the link
    while (_jointsLink.back()->parent != NULL)
    {

        _jointsLink.push_back(_jointsLink.back()->parent);

    }

    // calculate the matrix for forward kinematics
    for (int i = _jointsLink.size() - 1; i >= 0; i--) // go through the parents of end effector
    {

        // translation by Channel and scale
        if (_jointsLink[i]->parent == NULL) // this will shift the whole figure to a new place in world coordinate
        {
            forwardMatrix.translate(data[0] * scale, data[1] * scale, data[2] * scale);

        }
        else // For child joints, apply translation from the parent joint
        {
            forwardMatrix.translate(_jointsLink[i]->offset[0] * scale, _jointsLink[i]->offset[1] * scale, _jointsLink[i]->offset[2] * scale);
        }



        // rotate by Channel
        for (int k = 0; k < _jointsLink[i]->channels.size(); k++) {
            
            // get the channel for current joint
            Channel* channel = _jointsLink[i]->channels[k];

            // reset the deltaTheta
            if (_jointsLink[i]->index == _jointID_Rotate && channel->type == _axis) {
                
                deltaTheta = _rotateStep;
            }
            else 
            {
                deltaTheta = 0;
            }

            // rotate
            if (channel->type == X_ROTATION) {
                forwardMatrix.rotate(data[channel->index]+ deltaTheta, 1.0f, 0.0f, 0.0f);
            }
            else if (channel->type == Y_ROTATION) {
                forwardMatrix.rotate(data[channel->index]+ deltaTheta, 0.0f, 1.0f, 0.0f);
            }
            else if (channel->type == Z_ROTATION) {
                forwardMatrix.rotate(data[channel->index]+ deltaTheta, 0.0f, 0.0f, 1.0f);
            }
        }

        
    }

    return forwardMatrix;
}


QMatrix4x4 MoveFigure::ForwardKinematicsGeneral(int endID) 

{
    QMatrix4x4 forwardMatrix;
    std::vector<Joint*> _jointsLink;

    // get the link for the chosen end effector
    _jointsLink.push_back(joints[endID]);
    
    // loop through all the parents to form the link
    while(_jointsLink.back()->parent != NULL) 
    { 
        
        _jointsLink.push_back(_jointsLink.back()->parent);
        
    }


    // calculate the matrix for forward kinematics
    for (int i = _jointsLink.size() - 1; i >= 0; i--) // go through the parents of end effector
    {

        // translation by Channel and scale
        if (_jointsLink[i]->parent == NULL) // this will shift the whole figure to a new place in world coordinate
        {
            forwardMatrix.translate(data[0] * scale, data[1] * scale, data[2] * scale);

        }
        else // For child joints, apply translation from the parent joint
        {
            forwardMatrix.translate(_jointsLink[i]->offset[0] * scale, _jointsLink[i]->offset[1] * scale, _jointsLink[i]->offset[2] * scale);
        }



        // rotate by Channel
        for (int k = 0; k < _jointsLink[i]->channels.size(); k++) {

            // get the channel for current joint
            Channel* channel = _jointsLink[i]->channels[k];


            // rotate
            if (channel->type == X_ROTATION) {
                forwardMatrix.rotate(data[channel->index], 1.0f, 0.0f, 0.0f);
            }
            else if (channel->type == Y_ROTATION) {
                forwardMatrix.rotate(data[channel->index], 0.0f, 1.0f, 0.0f);
            }
            else if (channel->type == Z_ROTATION) {
                forwardMatrix.rotate(data[channel->index], 0.0f, 0.0f, 1.0f);
            }
        }


    }

    return forwardMatrix;
}