#include "FaceRecogConfig.h"


FaceDetectorSSD::FaceDetectorSSD()
{
	SetDefaults();
}

FaceDetectorSSD::FaceDetectorSSD(double scaleFactor, int nmsThreshold, cv::Size minSize, cv::Size maxSize,
                               cv::Size evalSize, int minNeighbours, double overlapThreshold)
{
    SetDefaults();
    initializeParameters(scaleFactor, nmsThreshold, minSize, maxSize,
                         evalSize, minNeighbours, overlapThreshold);
}

void FaceDetectorSSD::SetDefaults()
{
    initializeParameters(1.2, 4, Size(20, 20), Size(60, 60), Size(40, 40), 0, 0.1);
}

void FaceDetectorSSD::initializeParameters(double scaleFactor, int nmsThreshold, cv::Size minSize,
                                          cv::Size maxSize, cv::Size evalSize, int minNeighbours, double overlapThreshold)
{
    if (scaleFactor > 1)        this->scaleFactor = scaleFactor;
    if (nmsThreshold >= 0)      this->nmsThreshold = nmsThreshold;
    if (minSize != Size())      this->minSize = minSize;
    if (maxSize != Size())      this->maxSize = maxSize;
    if (evalSize != Size())     this->evalSize = evalSize;
    if (minNeighbours >= 0)     this->minNeighbours = minNeighbours;
    if (overlapThreshold >= 0)  this->overlapThreshold = overlapThreshold;
}

int FaceDetectorSSD::loadDetector(string name, FlipMode faceFlipMode)
{
    bool success_load_cascade = false;
    #if FACE_RECOG_USE_CUDA
	Ptr<cv::cuda::CascadeClassifier> cascade = cv::cuda::CascadeClassifier::create(name);
    success_load_cascade = !cascade.empty();
    #else
    FACE_RECOG_NAMESPACE::CascadeClassifier cascade;
    success_load_cascade = cascade.load(name);
    #endif

	if (!success_load_cascade)
	{
		cerr << "ERROR: Could not load classifier cascade " << name << endl;
		return -1;
	}

    #if FACE_RECOG_USE_CUDA
    cascade->setScaleFactor(scaleFactor);
    cascade->setMinObjectSize(minSize);
    cascade->setMaxObjectSize(maxSize);
    cascade->setMinNeighbors(minNeighbours);
    #endif

	faceFinder.push_back(cascade);
	stageCount.push_back(1);
    faceFlipModes.push_back(faceFlipMode);
	return 0;
}

void FaceDetectorSSD::assignImage(FACE_RECOG_MAT frame)
{
    size_t iFrame = frames.size();
    FlipMode fm = (iFrame < faceFlipModes.size()) ? faceFlipModes[iFrame] : NONE;
    frames.push_back(imFlip(frame, fm));
}

int FaceDetectorSSD::findFaces(vector<vector<Rect> >& faces)
{
	size_t nClassifiers = faceFinder.size();
    size_t nImages = frames.size();
	if (nImages != nClassifiers)
	{
		cerr << "ERROR in findFaces, different number of images and classifiers" << endl;
		return -1;
	}

    #pragma omp parallel for
	for (long c = 0; c < nClassifiers; ++c)
	{
        #if FACE_RECOG_USE_CUDA
        auto cascade_gpu = faceFinder[c];
        cascade_gpu->detectMultiScale(frames[c], foundObjects_gpu);
        cascade_gpu->convert(foundObjects_gpu, faces[c]);
        #else
		faceFinder[c].detectMultiScale(frames[c], faces[c], scaleFactor, nmsThreshold, CASCADE_SCALE_IMAGE, minSize, maxSize);
        #endif
	}
	return 0;
}

double FaceDetectorSSD::evaluateConfidence(Track& track, FACE_RECOG_MAT& image)
{
	/*EVALUATE CONFIDENCE FOR UNMATCHED TARGETS*/
    size_t nFaces = faceFinder.size();
	if (nFaces != stageCount.size()) {
		cerr << "ERROR in evaluateConfidence, different number of classifiers and stage count" << endl;
		return -1;
	}

	vector<double> confidences(nFaces);
	vector<Rect> trackersROI;

    // get current bbox for confidence evaluation, with resize to specified config size
	Rect face = track.bbox();
    std::vector<FACE_RECOG_MAT> croppedFaces(nFaces);

    double score = 0;
    #pragma omp parallel for
    for (long f = 0; f < nFaces; ++f) {
        croppedFaces[f] = imResize(imFlip(image(face).clone(), faceFlipModes[f]), evalSize);
        double tempWeights = 0;
        confidences[f] = faceFinder[f].classify(croppedFaces[f], tempWeights);
    }
    double maxConfidence = *std::max_element(confidences.begin(), confidences.end());
    return maxConfidence;

    ////////////////////////////////////////// OLD CODE (GPU vs. CPU) ==> gpu no 'classify' available
    //#if FACE_RECOG_USE_CUDA
    //auto cascade_gpu = faceFinder[i];
    //cascade_gpu->detectMultiScale(croppedTarget, foundObjects_gpu);
    //cascade_gpu->convert(foundObjects_gpu, trackersROI);
    //#else
    //faceFinder[i].detectMultiScale(croppedTarget, trackersROI, scaleFactor, minNeighbours, CV_HAAR_SCALE_IMAGE);
    //#endif
}

void FaceDetectorSSD::flipFaces(size_t index, vector<vector<Rect> >& faces)
{
    for (size_t i = 0; i < faces[index].size(); ++i)
    {
        Rect face_i = faces[index][i];
        faces[index][i].x = frames[index].cols - face_i.x - face_i.width;
    }
}

vector<Rect> FaceDetectorSSD::mergeDetections(vector<vector<Rect> >& faces)
{
    size_t nFaces = faces.size();
    bool frontalOnly = nFaces == 1;
    if (!frontalOnly)
        for (size_t f = 0; f < nFaces; ++f)
            if (faceFlipModes[f] == HORIZONTAL)
                flipFaces(f, faces);
    return util::mergeDetections(faces, overlapThreshold, frontalOnly);
}



#############################################################################################################################

#include "SSD_Detector.h"

using namespace caffe;


SSD_Detector::SSD_Detector(const string& model_file,
                   const string& weights_file,
                   const string& mean_file,
                   const string& mean_value, 
                   bool useGpu) {
  Caffe::set_mode(useGpu ? Caffe::GPU : Caffe::CPU);

  /* Load the network. */
  net_.reset(new Net<float>(model_file, TEST));
  net_->CopyTrainedLayersFrom(weights_file);

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  Blob<float>* input_layer = net_->input_blobs()[0];
  num_channels_ = input_layer->channels();
  CHECK(num_channels_ == 3 || num_channels_ == 1)
    << "Input layer should have 1 or 3 channels.";
  input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

  /* Load the binaryproto mean file. */
  SetMean(mean_file, mean_value);
}

std::vector<vector<float> > SSD_Detector::Detect(const cv::Mat& img) {
  Blob<float>* input_layer = net_->input_blobs()[0];
  input_layer->Reshape(1, num_channels_,
                       input_geometry_.height, input_geometry_.width);
  /* Forward dimension change to all layers. */
  net_->Reshape();

  std::vector<cv::Mat> input_channels;
  WrapInputLayer(&input_channels);

  Preprocess(img, &input_channels);

  net_->Forward();

  /* Copy the output layer to a std::vector */
  Blob<float>* result_blob = net_->output_blobs()[0];
  const float* result = result_blob->cpu_data();
  const int num_det = result_blob->height();
  vector<vector<float> > detections;
  for (int k = 0; k < num_det; ++k) {
    if (result[0] == -1) {
      // Skip invalid detection.
      result += 7;
      continue;
    }
    vector<float> detection(result, result + 7);
    detections.push_back(detection);
    result += 7;
  }
  return detections;
}

/* Load the mean file in binaryproto format. */
void SSD_Detector::SetMean(const string& mean_file, const string& mean_value) {
  cv::Scalar channel_mean;
  if (!mean_file.empty()) {
    CHECK(mean_value.empty()) <<
      "Cannot specify mean_file and mean_value at the same time";
    BlobProto blob_proto;
    ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

    /* Convert from BlobProto to Blob<float> */
    Blob<float> mean_blob;
    mean_blob.FromProto(blob_proto);
    CHECK_EQ(mean_blob.channels(), num_channels_)
      << "Number of channels of mean file doesn't match input layer.";

    /* The format of the mean file is planar 32-bit float BGR or grayscale. */
    std::vector<cv::Mat> channels;
    float* data = mean_blob.mutable_cpu_data();
    for (int i = 0; i < num_channels_; ++i) {
      /* Extract an individual channel. */
      cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
      channels.push_back(channel);
      data += mean_blob.height() * mean_blob.width();
    }

    /* Merge the separate channels into a single image. */
    cv::Mat mean;
    cv::merge(channels, mean);

    /* Compute the global mean pixel value and create a mean image
     * filled with this value. */
    channel_mean = cv::mean(mean);
    mean_ = cv::Mat(input_geometry_, mean.type(), channel_mean);
  }
  if (!mean_value.empty()) {
    CHECK(mean_file.empty()) <<
      "Cannot specify mean_file and mean_value at the same time";
    stringstream ss(mean_value);
    vector<float> values;
    string item;
    while (getline(ss, item, ',')) {
      float value = std::atof(item.c_str());
      values.push_back(value);
    }
    CHECK(values.size() == 1 || values.size() == num_channels_) <<
      "Specify either 1 mean_value or as many as channels: " << num_channels_;

    std::vector<cv::Mat> channels;
    for (int i = 0; i < num_channels_; ++i) {
      /* Extract an individual channel. */
      cv::Mat channel(input_geometry_.height, input_geometry_.width, CV_32FC1,
          cv::Scalar(values[i]));
      channels.push_back(channel);
    }
    cv::merge(channels, mean_);
  }
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void SSD_Detector::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
  Blob<float>* input_layer = net_->input_blobs()[0];

  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

void SSD_Detector::Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels) {
  /* Convert the input image to the input image format of the network. */
  cv::Mat sample;
  if (img.channels() == 3 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
  else if (img.channels() == 4 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
  else if (img.channels() == 4 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
  else if (img.channels() == 1 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
  else
    sample = img;

  cv::Mat sample_resized;
  if (sample.size() != input_geometry_)
    cv::resize(sample, sample_resized, input_geometry_);
  else
    sample_resized = sample;

  cv::Mat sample_float;
  if (num_channels_ == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
    sample_resized.convertTo(sample_float, CV_32FC1);

  cv::Mat sample_normalized;
  cv::subtract(sample_float, mean_, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == net_->input_blobs()[0]->cpu_data())
    << "Input channels are not wrapping the input layer of the network.";
}
