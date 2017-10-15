#include "FaceRecog.h"


int IFaceDetector::loadDetector(string name)
{
    return -1;
}

void IFaceDetector::assignImage(FACE_RECOG_MAT frame)
{
    frames.push_back(frame);
}

void IFaceDetector::flipFaces(size_t index, vector<vector<Rect> >& faces)
{
    for (size_t i = 0; i < faces[index].size(); ++i)
    {
        Rect face_i = faces[index][i];
        faces[index][i].x = frames[index].cols - face_i.x - face_i.width;
    }
}

vector<Rect> IFaceDetector::mergeDetections(vector<vector<Rect> >& faces)
/*TODO: this has to be done for a generic number of faces
(this implementation is for compatibility with Duffner&Obodez experiments)*/
{
    bool frontalOnly = faces.size() == 1;
    return util::mergeDetections(faces, overlapThreshold, frontalOnly);
}
