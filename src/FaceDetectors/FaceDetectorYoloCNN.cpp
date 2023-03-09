#include "FaceRecogConfig.h"
#ifdef FACE_RECOG_HAS_YOLO_CNN


template<typename Dtype>
Dtype lap(Dtype x1_min,Dtype x1_max,Dtype x2_min,Dtype x2_max)
{
    if(x1_min < x2_min) {
        if(x1_max < x2_min)
            return 0;
        else if (x1_max > x2_min) {
                if (x1_max < x2_max)
                    return x1_max - x2_min;
                else
                    return x2_max - x2_min;
        }
        
    }
    else if(x1_min < x2_max) {
        if(x1_max < x2_max)
            return x1_max-x1_min;
        else
            return x2_max-x1_min;
    }

    return 0;
}

template int lap(int x1_min, int x1_max, int x2_min, int x2_max);
template float lap(float x1_min, float x1_max, float x2_min, float x2_max);

FaceDetectorYoloCNN::FaceDetectorYoloCNN()
{
    std::string model = "../yolo-face-deploy.prototxt";
    std::string proto = "../yolo-face.caffemodel"; 
    char *labelname[] = {"face"};
    Caffe::set_mode(Caffe::GPU);
    net.reset(new caffe::Net<float>(model, caffe::TEST));
    net->CopyTrainedLayersFromBinaryProto(proto);
}

int FaceDetectorYoloCNN::findFaces(std::vector<std::vector<cv::Rect> >& faces)
{
    m_faces.clear();
    loadData(frames[0]);
    net->Forward();
    Blob<float>* output_layer = net->output_blobs()[0];
    const float* begin = output_layer->cpu_data();
    const float* end = begin + output_layer->channels();
    std::vector<float> result(begin, end);
    std::vector<std::vector<int> > bboxs;
    float threashold = 0.3;
    float pro_obj[121][2];
    int idx_class[121];
    float overlap;
    float overlap_thresh = 0.9;
   
    getBox(result, &pro_obj[0][0],idx_class,bboxs,threashold,frames[0]);

    std::vector<bool> mark(bboxs.size(),true);

    for(int i = 0; i < bboxs.size(); ++i) {
        for(int j = i+1; j < bboxs.size(); ++j) {
            int overlap_x = lap(bboxs[i][0],bboxs[i][2],bboxs[j][0],bboxs[j][2]);
            int overlap_y = lap(bboxs[i][1],bboxs[i][3],bboxs[j][1],bboxs[j][3]);
            overlap = (overlap_x*overlap_y)*1.0/((bboxs[i][0]-bboxs[i][2])*(bboxs[i][1]-bboxs[i][3])+(bboxs[j][0]-bboxs[j][2])*(bboxs[j][1]-bboxs[j][3])-(overlap_x*overlap_y));
            if(overlap > overlap_thresh) {
                if(bboxs[i][4] > bboxs[j][4])
                    mark[j] = false;
                else
                    mark[i] = false;
            }
        }
    }
    for(int i = 0; i < bboxs.size();++i) {
        if(mark[i]) {
            cv::Point point1(bboxs[i][1],bboxs[i][2]);
            cv::Point point2(bboxs[i][3],bboxs[i][4]);
            // cv::rectangle(frames[0], cv::Rect(point1,point2),cv::Scalar(0,bboxs[i][0]/20.0*225,255),bboxs[i][5]/8);
            // char ch[100];
            // sprintf(ch,"%s %.2f",labelname[bboxs[i][0]-1], bboxs[i][5]*1.0/100);
            // std::string temp(ch);
            // cv::putText(frames[0],temp,point1,CV_FONT_HERSHEY_COMPLEX,0.4,cv::Scalar(255,255,255));
            m_faces.push_back(cv::Rect(point1, point2));
        }
    }
    faces[0] = m_faces;
    return 1;
}

double FaceDetectorYoloCNN::evaluateConfidence(Target& target, FACE_RECOG_MAT& image)
{ 
	Rect face = target.bbox();
    FACE_RECOG_MAT croppedFace = image(face).clone();
    m_faces.clear();
    loadData(croppedFace);
    net->Forward();
    Blob<float>* output_layer = net->output_blobs()[0];
    const float* begin = output_layer->cpu_data();
    const float* end = begin + output_layer->channels();
    std::vector<float> result(begin, end);
    std::vector<std::vector<int> > bboxs;
    float pro_obj[121][2];
    int idx_class[121];
    float threashold = 0.1;
    float overlap;
    float overlap_thresh = 0.8;

    getBox(result, &pro_obj[0][0],idx_class,bboxs,threashold,frames[0]);

    return bboxs.size();
}

void FaceDetectorYoloCNN::assignImage(FACE_RECOG_MAT frame)
{
    frames.emplace_back(frame);
}

void FaceDetectorYoloCNN::loadData(cv::Mat& image)
{
    Blob<float>* input_layer = net->input_blobs()[0];
    int width, height;
    width = input_layer->width();
    height = input_layer->height();
    int size = width*height;
    cv::Mat image_resized;
    cv::resize(image, image_resized, cv::Size(height, width));
    float* input_data = input_layer->mutable_cpu_data();
    int temp,idx;
    for(int i = 0; i < height; ++i) {
        uchar* pdata = image_resized.ptr<uchar>(i);
        for(int j = 0; j < width; ++j) {
            temp = 3*j;
            idx = i*width+j;
            input_data[idx] = (pdata[temp+2]/127.5)-1;
            input_data[idx+size] = (pdata[temp+1]/127.5)-1;
            input_data[idx+2*size] = (pdata[temp+0]/127.5)-1;
        }
    }
}

void FaceDetectorYoloCNN::getBox(std::vector<float> result,float* pro_obj,int* idx_class,std::vector<std::vector<int> >& bboxs,float thresh,cv::Mat image)
{
    float pro_class[121];
    int idx;
    float max_idx;
    float max;
    for(int i = 0; i < 11; ++i) {
        for(int j = 0; j < 11;++j) {
            max = 0;
            max_idx = 0;
            idx = 1*(i*11+j);
            for(int k = 0; k < 1; ++k) {
                if (result[idx+k] > max) {
                    max = result[idx+k];
                    max_idx = k+1;
                }
            }
            idx_class[i*11+j] = max_idx;
            pro_class[i*11+j] = max;
            pro_obj[(i*11+j)*2] = max*result[11*11*1+(i*11+j)*2];
            pro_obj[(i*11+j)*2+1] = max*result[11*11*1+(i*11+j)*2+1];
        }
    }
    std::vector<int> bbox;
    int x_min,x_max,y_min,y_max;
    float x,y,w,h;
    for(int i = 0; i < 11;++i) {
        for(int j = 0; j < 11;++j) {
            for(int k = 0; k < 2; ++k) {
                if(pro_obj[(i*11+j)*2 + k] > thresh) {
                    //std::cout << "(" << i << "," << j << "," << k << ")" << " prob="<<pro_obj[(i*7+j)*2 + k] << " class="<<idx_class[i*7+j]<<std::endl;
                    idx = 121*1 + 121*2 + ((i*11+j)*2+k)*4;
                    x = image.cols*(result[idx++]+j)/11;
                    y = image.rows*(result[idx++]+i)/11;
                    w = image.cols*result[idx]*result[idx++];
                    h = image.rows*result[idx]*result[idx];
                    //std::cout << x <<" "<< y << " " << w <<" "<< h <<std::endl;
                    x_min = x - w/2;
                    y_min = y - h/2;
                    x_max = x + w/2;
                    y_max = y + h/2;
                    bbox.clear();
                    bbox.push_back(idx_class[i*7+j]);
                    bbox.push_back(x_min);
                    bbox.push_back(y_min);
                    bbox.push_back(x_max);
                    bbox.push_back(y_max);
                    bbox.push_back(int(pro_obj[(i*11+j)*2 + k]*100));
                    bboxs.push_back(bbox);
                }
            }
        }
    }
}

#endif/*FACE_RECOG_HAS_YOLO_CNN*/