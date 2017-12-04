//
// Created by slh on 17-10-27.
//

#ifndef RETRIEVAL_VEHICLEFEATURE_H
#define RETRIEVAL_VEHICLEFEATURE_H

#ifndef USE_OPENCV
#define USE_OPENCV
#include <iostream>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <caffe/caffe.hpp>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif
namespace feature_index {
#define TOTALBYTESIZE 1024
#define ONEBYTESIZE 8
    // short cut the type of unsigned char
    typedef unsigned char uchar;
    // define info string
    struct Info_String
    {
        char info[100];
    };

    /**
     *   @author Su
     *   @class FeatureIndex
     *   @usage 1. init network, extract or detect iamge feature
     *          2. return the feature
     */
    class Feature{
    public:
        /// define init net work func
        caffe::Net<float> *InitNet(std::string proto_file, std::string proto_weight);
        /// define public mem and function
        /// init function
        inline Feature(){ feature_extraction_net = NULL; }
        /// only init feature
        Feature(std::string proto_file,std::string proto_mode);

        /// init gpu
        int InitGpu(const char *CPU_MODE, int GPU_ID);

        /// feature extract from picture init from net
        float* PictureFeatureExtraction(int count, caffe::Net<float> * _net, std::string blob_name);
        /// Attr feature extract
        float* PictureAttrFeatureExtraction(int count, caffe::Net<float> * _net, std::string feature_blob_name,
                                            std::string Attr_color_name, std::string Attr_type_name, int* color_re, int* type_re);
        /// Attr feature extract
        void PictureAttrExtraction(int count, caffe::Net<float> * _net, std::string Attr_color_name,
                                   std::string Attr_type_name, int* color_re, int* type_re);

        float* MemoryPictureAttrFeatureExtraction(int count, caffe::Net<float> *_net, std::string feature_blob_name,
                                                                std::string Attr_color_name, std::string Attr_type_name,
                                                                int* color_re, int* type_re, std::vector<cv::Mat> pic_list,
                                                                std::vector<int> label);
        /// / float to binary
        uchar* floatToUnsignedChar(const float* data, int count);
    private:
        /// define const var
        const static int batch_size = 1;
        /// define private var
        std::string BLOB_NAME ;// "fc_hash/relu";
        caffe::Net<float> *feature_extraction_net;
        std::map<int, int> LabelList;// Evaluate
    };
}
#endif //RETRIEVAL_VEHICLEFEATURE_H
