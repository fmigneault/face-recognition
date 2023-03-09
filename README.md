# FaceRecog #

------------------------------------------------------------------

## Details ##

On-line Face Detection, Tracking and Recognition.

### Citation and License ###

Relevant Papers and Reports:

- [Conception de système de reconnaissance de visages spatio-temporelle sur vidéos à partir d’une seule image de référence](https://www.researchgate.net/publication/323401703)
  <br> \[Full Text Available\] \[Language: French\] 

  ```bibtex
  @mastersthesis{fmigneault2017ets,
    title={Conception de syst{\`e}me de reconnaissance de visages spatiotemporelles sur vid{\'e}os {\`a} partir d'une seule image de r{\'e}f{\'e}rence},
    author={Charette-Migneault, Francis},
    journal={Master's thesis in Automated Production Engineering, University of Quebec},
    pages={173},
    year={2017},
    month={dec},
    school={{\'E}cole de technologie sup{\'e}rieure, Universit{\'e} du Qu{\'e}bec},
    note={supervisor: Granger, Eric},
    doi={10.13140/RG.2.2.20640.07683}
  }
  ```

- [Using Adaptive Trackers for Video Face Recognition from a Single Sample Per Person](https://www.researchgate.net/publication/328118135)
  <br> \[Full Text Available\] \[Language: English\]

  ```bibtex
  @inproceedings{fmigneault2018ipta,
    title={Using Adaptive Trackers for Video Face Recognition from a Single Sample Per Person},
    author={Charette-Migneault, Francis and Granger, Eric and Mokhayeri, Fania},
    booktitle={2018 Eighth International Conference on Image Processing Theory, Tools and Applications (IPTA)},
    pages={1--6},
    year={2018},
    organization={IEEE},
    doi={10.1109/IPTA.2018.8608163}
  }
  ```

See: [LICENSE](./LICENSE)

### Contents ###

| Algorithm                   | Description                                                                       |
|-----------------------------|-----------------------------------------------------------------------------------|
| **Viola-Jones**             | face/eye detectors (Haar/LBP)                                                     |
| **Fast-DT**                 | face detector+tracker (FAce STructured Detection and Tracking)                    |
| **Camshift**                | tracker                                                                           |
| **Compressive Tracking**    | tracker                                                                           |
| **FRCNN**                   | Faster R-CNN face detection (Faster Region-based Convolutional Neural Networks)   |
| **KCF**                     | Kernelized Correlation Filters tracker                                            |
| **SSD**                     | Single Shot MultiBox Detector (CNN models with PASCAL-VOC, COCO or ILSVRC)        |
| **YOLO**                    | Real-Time Object Detection using CNN                                              |
| **EoESVM**                  | Ensemble of Exemplar-SVM face recognition                                         |
| **FaceNet**                 | Face recognition using deep convolutional neural network                          |

  *__Note:__ see [CONTRIBUTORS](./CONTRIBUTORS.md) for references*
  
------------------------------------------------------------------

## Requirements ##

### *Mandatory* ###

|Name         |Details                              |Repository|
|-------------|-------------------------------------|----------|
|Boost        | tested with 1.64.0                  | [Boost](http://www.boost.org/) |
|Common (C++) | utility functions                   | [CommonCpp](https://bitbucket.org/TeamLIVIA/common-cpp)  |
|Eigen        | 3.3 supported                       | [Eigen](http://eigen.tuxfamily.org/)  |
|OpenCV       | 3.2 supported, 2.4 not maintained   | [OpenCV](https://github.com/opencv/opencv)  |
|mvector      | multi-dimensional vector            | fork: [fmigneault/mvector](https://github.com/fmigneault/mvector) original: [carlobaldassi/mvector](https://github.com/carlobaldassi/mvector) |

### *Optional* ###

|Name                |Details                                                               |Repository|
|--------------------|----------------------------------------------------------------------|----------|
| | |
|Caffe               | To use FRCNN and YOLO                                                | [Caffe](https://github.com/BVLC/caffe) |
|FRCNN               | To use FRCNN, not tested with Python                                 | [FRCNN](https://github.com/ShaoqingRen/faster_rcnn) alternative: [pyFRCNN](https://github.com/rbgirshick/py-faster-rcnn) |
|YOLO                | To use YOLO, not tested with V2 [1]                                  | [camel007/YOLO](https://github.com/camel007/caffe-yolo-face-detection) or [xingwangsfu/YOLO](https://github.com/xingwangsfu/caffe-yolo) |
| | |
|ExemplarSVM         | To use ESVM (with LIBSVM [2][3] or alternatively LIBLINEAR [4][5])   | [ESVM](https://bitbucket.org/TeamLIVIA/ExemplarSVM) | 
|FeatureExtractorHOG | To use ESVM                                                          | [feHOG](https://bitbucket.org/TeamLIVIA/FeatureExtractorHOG) |
| | |
|TensorFlow          | To use FaceNet                                                       | [TensorFlow](https://github.com/tensorflow/tensorflow) |
|FaceNet             | To use FaceNet                                                       | [FaceNet](https://github.com/davidsandberg/facenet) |

>[1] DarkNet YOLO v2: [Project](https://pjreddie.com/darknet/yolo/)  
[2] LIBSVM original: [cjlin1/libsvm](https://github.com/cjlin1/libsvm)  
[3] LIBSVM OpenMP fork: [TeamLIVIA/libsvm](https://bitbucket.org/TeamLIVIA/libsvm/src/openmp-windows-binaries) (parallelism)  
[4] LIBLINEAR original: [cjlin1/liblinear](https://github.com/cjlin1/liblinear)  
[5] LIBLINEAR multi-core: [TeamLIVIA/LIBLINEAR-multicore](https://bitbucket.org/TeamLIVIA/liblinear-multicore)  

### Data files ###

To use the [Exemplar-SVM](https://bitbucket.org/TeamLIVIA/ExemplarSVM) face recognition classifier, please download these [data files](https://drive.google.com/open?id=0Bw9khIGD6Jbba0tkSTJWVFNkeFU) and place them under the root directory.  
Alternatively, place the files elsewhere and modify the corresponding paths in the `config.txt` file.

------------------------------------------------------------------

## Build/Install ##

A `Makefile` and `CMakeLists` are provided to build the project.  
Please report any issue if some dependencies where omitted by mistake.

### *Notes* ###

* The OpenCV `objdetect` module requires some additional functions to employ *Fast-DT*.  
  Please refer to [OPENCV_DIFF](./OPENCV_DIFF.md) instructions.

* The projet contains multiple logging operations for evaluation and debugging purposes.  
  These operations are enabled automatically when in DEBUG configuration, but are completely disabled in RELEASE.  
  Therefore, to obtain the best processing times and performances, the project should be compiled in RELEASE.  
  It is to be noted that running any test evaluation will require DEBUG configuration to output the results.
  
---------------------------------------------------------------------

## Usage ##

```
./FaceRecog <opencv_root> 
            [-o <output_directory>] [-i <images_directory>] [-c <config_file_path>] 
            [-p <frames_path>|-t <test_file_path>|-v <video_path>] [-r <result_file_path>]
```

|Argument            |Option|Description
|--------------------|------|----------------------------
|`opencv_root`       |      | Path to your local OpenCV source directory. Employed for loading cascade classifier files. [^noteStar] 
|`output_directory`  | `-o` | Alternative directory to output results (default: `./output`). 
|`images_directory`  | `-i` | Alternative directory to output image track ROIs (default: `./images`). 
|`config_file_path`  | `-c` | Alternative `config.txt` file path (default: `./config.txt`). 
|`frames_path_regex` | `-p` | Path to image video frames to employ as input instead of video file or camera input. Must specify the expected *continuous* frame names using a regex such as `./%04d.jpg`. [^note1] [^note2] [^note3] 
|`result_file_path`  | `-r` | Path to a text file (existing or not) to output recognition results (default: `./result.txt`). [^note4]
|`test_file_path`    | `-t` | Path to text file containing on each line the same expression as specified by `-p`. Test sequences are then processed sequentially using the specified paths. [^note1] [^note2] [^note3]
|`video_path`        | `-v` | Path to an unique video file to employ as input frames. [^note1] [^note3]

### *Additional Notes* ###

Additional parameters are obtained from the `config.txt` file.
These include multiple option toggles, type indexes, threshold parameters, training values, etc.  

[^noteStar]: Mandatory input argument.  

[^note1]: Option is ignored if an on-line camera input index and type are accordingly set in the `config.txt` file.  

[^note2]: Image frame sequences must have continuous file names to ensure proper loading. For example, if an input regex is `%04d.jpg`, files must be named `0000.jpg`, `0001.jpg`, `0002.jpg`, and so on. Skipping an index will result in failure to load frames adequately. See `py/Rename-Generate Frame-Sequence` utility to generate missing sequential frames as required. Note that using the `%04d.jpg` regex format from the command line could require an input such as `%%04d.jpg` (two `%`) to nullify the special character.  

[^note3]: Options `-p`, `-t` and `-v` are mutually exclusive. Option `-t` has priority over the others, while `-p` has priority over `-v` if these options are specified simultaneously.  

[^note4]: Option `-r` is activated automatically with default value if not specified when also specifying `-p` or `-t`. If none of  `-p`, `-t` or `-v` options is specified, `-r` is automatically disabled even if passed as input argument.  

------------------------------------------------------------------

## Change Log ##

See [CHANGELOG](./CHANGELOG.md)

------------------------------------------------------------------

## Contributors ##

See [CONTRIBUTORS](./CONTRIBUTORS.md)
