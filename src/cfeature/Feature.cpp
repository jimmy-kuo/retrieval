//
// Created by slh on 17-10-27.
//

//
// Created by dell on 17-5-4.
//
#include <cstdio>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include "Feature.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "caffe/layers/memory_data_layer.hpp"
#include "caffe/util/db.hpp"
using namespace caffe;
namespace feature_index{

    /**
     *
     * @param proto_file
     * @param proto_weight
     * @return caffe::Net<float>*
     * @usage  1. init the network
     *
     */
    caffe::Net<float>* Feature::InitNet(std::string proto_file, std::string proto_weight) {
        //net work init
        std::string pretrained_binary_proto(proto_weight);
        std::string proto_model_file(proto_file);
        Net<float>* net(new Net<float>(proto_model_file, caffe::TEST));
        net->CopyTrainedLayersFrom(pretrained_binary_proto);
        return net;
    }

    /**
     *
     * @param proto_file
     * @param proto_weight
     *
     */
    Feature::Feature(std::string proto_file, std::string proto_weight) {
        feature_extraction_net = InitNet(proto_file, proto_weight);
    }

    /**
     *
     * @param CPU_MODE
     * @param GPU_ID
     * @return
     *
     */
    int Feature::InitGpu(const char *CPU_MODE, int GPU_ID) {
        //GPU init
        if (CPU_MODE != NULL && (strcmp(CPU_MODE, "GPU") == 0)) {
            int device_id = 0;
            device_id = GPU_ID;
            Caffe::SetDevice(device_id);
            Caffe::set_mode(Caffe::GPU);
        }
        else {
            Caffe::set_mode(Caffe::CPU);
        }
        return 0;
    }

    /**
     * @param count
     *      file count
     * @param _net:
     *      caffe net
     * @return
     *      float result
     *
     */
    float* Feature::PictureFeatureExtraction(int count, caffe::Net<float> *_net, std::string blob_name) {
        std::string extract_feature_blob_names(blob_name);
        /////modify by su
        std::cout<<"batch size: "<< _net->blob_by_name(extract_feature_blob_names)->num()<<std::endl;
        int num_mini_batches = count / _net->blob_by_name(extract_feature_blob_names)->num();
        // init memory
        float* feature_dbs = new float[count * TOTALBYTESIZE ];
        std::vector<caffe::Blob<float>*> input_vec;
        Datum datum;
        const boost::shared_ptr<Blob<float> > feature_blob =
                                                      _net->blob_by_name(extract_feature_blob_names);
        int batch_size = feature_blob->num();
        int dim_features = feature_blob->count() / batch_size;
        for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index) {
            //std::cout<<"start"<<std::endl;
            _net->Forward(input_vec);
            const float* feature_blob_data;
            for (int n = 0; n < batch_size; ++n) {
                feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);
                for (int d = 0; d < dim_features ; ++d) {
                    feature_dbs[d + (n + batch_index*batch_size)*dim_features] = feature_blob_data[d];
                } // for (int d = 0; d < dim_features / 8; ++d)
            }  // for (int n = 0; n < batch_size; ++n)
        }  // for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index)

        // write the remain batch
        bool isRemain=false;
        int remain = count - num_mini_batches*(_net->blob_by_name(extract_feature_blob_names)->num());
        if(remain >0 ){
            isRemain=true;
            _net->Forward(input_vec);
        }
        if(isRemain){
            const float* feature_blob_data;
            for (int n = 0; n < remain; ++n) {//data new
                feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);
                for (int d = 0; d < dim_features; ++d) {
                    feature_dbs[(num_mini_batches*batch_size+n)*dim_features +d] = feature_blob_data[d];
                } // for (int d = 0; d < dim_features/8; ++d)
            }  // for (int n = 0; n < remian; ++n)
        }  // for (int i = 0; i < num_features; ++i)

        std::cout<<"Successfully"<<std::endl;
        return feature_dbs;

    }

    /**
     *
     * @param count
     * @param _net
     * @param feature_blob_name
     * @param Attr_blob_name
     * @return
     */
    float* Feature::PictureAttrFeatureExtraction(int count, caffe::Net<float> *_net, std::string feature_blob_name,
                                                      std::string Attr_color_name, std::string Attr_type_name,
                                                      int* color_re, int* type_re) {
        const int color_type = 11;
        const int car_type = 1232;

        std::string extract_feature_blob_names(feature_blob_name);
        /// modify by su
        std::cout<<"batch size: "<< _net->blob_by_name(extract_feature_blob_names)->num()<<std::endl;
        int num_mini_batches = count / _net->blob_by_name(extract_feature_blob_names)->num();
        /// init memory
        float* feature_dbs = new float[count * TOTALBYTESIZE ];

        std::vector<caffe::Blob<float>*> input_vec;
        Datum datum;
        const boost::shared_ptr<Blob<float> > feature_blob =
                                                      _net->blob_by_name(extract_feature_blob_names);

        const boost::shared_ptr<Blob<float> > attr_type_blob =
                                                      _net->blob_by_name(Attr_type_name);

        const boost::shared_ptr<Blob<float> > attr_color_blob =
                                                      _net->blob_by_name(Attr_color_name);

        int batch_size = feature_blob->num();
        int dim_features = feature_blob->count() / batch_size;
        for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index) {
            /// std::cout<<"start"<<std::endl;
            _net->Forward(input_vec);
            const float* feature_blob_data;
            const float* feature_color_data;
            const float* feature_type_data;
            for (int n = 0; n < batch_size; ++n) {

                feature_color_data =
                        attr_color_blob->cpu_data() + attr_color_blob->offset(n);

                feature_type_data =
                        attr_type_blob->cpu_data() + attr_type_blob->offset(n);

                /// color
                float max = -1;
                int max_id = -1;
                for(int k = 0; k< color_type; k++){
                    if(feature_color_data[k] > max){
                        max = feature_color_data[k];
                        max_id = k;
                    }
                }
                color_re[n + batch_index*batch_size] = max_id;

                /// car
                max = -1;
                max_id = -1;
                for(int k = 0; k< car_type; k++){
                    if(feature_type_data[k] > max){
                        max = feature_type_data[k];
                        max_id = k;
                    }
                }
                type_re[n + batch_index*batch_size] = max_id;

                /// feature
                feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);
                for (int d = 0; d < dim_features ; ++d) {
                    feature_dbs[d + (n + batch_index*batch_size)*dim_features] = feature_blob_data[d];
                } // for (int d = 0; d < dim_features / 8; ++d)
            }  // for (int n = 0; n < batch_size; ++n)
        }  // for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index)

        /// write the remain batch
        bool isRemain=false;
        int remain = count - num_mini_batches*(_net->blob_by_name(extract_feature_blob_names)->num());
        if(remain >0 ){
            isRemain=true;
            _net->Forward(input_vec);
        }
        if(isRemain){
            const float* feature_blob_data;
            const float* feature_color_data;
            const float* feature_type_data;
            for (int n = 0; n < remain; ++n) {//data new

                feature_color_data =
                        attr_color_blob->cpu_data() + attr_color_blob->offset(n);

                feature_type_data =
                        attr_type_blob->cpu_data() + attr_type_blob->offset(n);

                /// color
                float max = -1;
                for(int k = 0; k< color_type; k++){
                    if(feature_color_data[k] > max){
                        max = feature_color_data[k];
                    }
                }
                color_re[num_mini_batches*batch_size + n] = max;

                /// car
                max = -1;
                for(int k = 0; k< car_type; k++){
                    if(feature_type_data[k] > max){
                        max = feature_type_data[k];
                    }
                }
                type_re[num_mini_batches*batch_size + n] = max;

                feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);
                for (int d = 0; d < dim_features; ++d) {
                    feature_dbs[(num_mini_batches*batch_size+n)*dim_features +d] = feature_blob_data[d];
                } // for (int d = 0; d < dim_features/8; ++d)
            }  // for (int n = 0; n < remian; ++n)
        }  // for (int i = 0; i < num_features; ++i)

        std::cout<<"Successfully"<<std::endl;
        return feature_dbs;
    }


    /**
     *
     * @param count
     * @param _net
     * @param Attr_color_name
     * @param Attr_type_name
     * @param color_re
     * @param type_re
     */
    void Feature::PictureAttrExtraction(int count, caffe::Net<float> * _net, std::string Attr_color_name,
                                             std::string Attr_type_name, int* color_re, int* type_re){

        std::string extract_feature_blob_names(Attr_type_name);
        /// modify by su
        std::cout<<"batch size: "<< _net->blob_by_name(extract_feature_blob_names)->num()<<std::endl;
        int num_mini_batches = count / _net->blob_by_name(extract_feature_blob_names)->num();
        /// init memory

        std::vector<caffe::Blob<float>*> input_vec;
        Datum datum;

        const boost::shared_ptr<Blob<float> > attr_type_blob =
                                                      _net->blob_by_name(Attr_type_name);

        const boost::shared_ptr<Blob<float> > attr_color_blob =
                                                      _net->blob_by_name(Attr_color_name);

        int batch_size = attr_type_blob->num();
        int dim_colors = attr_color_blob->count() / batch_size;
        int dim_types = attr_type_blob->count() / batch_size;

        std::cout<<"dim_colors: "<< dim_colors<<" dim_types: "<<dim_types<<std::endl;
        for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index) {
            /// std::cout<<"start"<<std::endl;
            _net->Forward(input_vec);
            const float* feature_blob_data;
            const float* feature_color_data;
            const float* feature_type_data;
            for (int n = 0; n < batch_size; ++n) {

                feature_color_data =
                        attr_color_blob->cpu_data() + attr_color_blob->offset(n);

                feature_type_data =
                        attr_type_blob->cpu_data() + attr_type_blob->offset(n);

                /// color
                float max = -1;
                int max_id = -1;
                for(int k = 0; k< dim_colors; k++){
                    if(feature_color_data[k] > max){
                        max = feature_color_data[k];
                        max_id = k;
                    }
                }
                color_re[n + batch_index*batch_size] = max_id;

                /// car
                max = -1;
                max_id = -1;
                for(int k = 0; k< dim_types; k++){
                    if(feature_type_data[k] > max){
                        max = feature_type_data[k];
                        max_id = k;
                    }
                }
                type_re[n + batch_index*batch_size] = max_id;


            }  // for (int n = 0; n < batch_size; ++n)
        }  // for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index)

        /// write the remain batch
        bool isRemain=false;
        int remain = count - num_mini_batches*(_net->blob_by_name(extract_feature_blob_names)->num());
        if(remain >0 ){
            isRemain=true;
            _net->Forward(input_vec);
        }
        if(isRemain){
            const float* feature_blob_data;
            const float* feature_color_data;
            const float* feature_type_data;
            for (int n = 0; n < remain; ++n) {//data new

                feature_color_data =
                        attr_color_blob->cpu_data() + attr_color_blob->offset(n);

                feature_type_data =
                        attr_type_blob->cpu_data() + attr_type_blob->offset(n);

                /// color
                float max = -1;
                for(int k = 0; k< dim_colors; k++){
                    if(feature_color_data[k] > max){
                        max = feature_color_data[k];
                    }
                }
                color_re[num_mini_batches*batch_size + n] = max;

                /// car
                max = -1;
                for(int k = 0; k< dim_types; k++){
                    if(feature_type_data[k] > max){
                        max = feature_type_data[k];
                    }
                }
                type_re[num_mini_batches*batch_size + n] = max;


            }  // for (int n = 0; n < remian; ++n)
        }  // for (int i = 0; i < num_features; ++i)

        std::cout<<"Successfully"<<std::endl;
        return ;
    }


    float* Feature::MemoryPictureAttrFeatureExtraction(int count, caffe::Net<float> *_net, std::string feature_blob_name,
                                                      std::string Attr_color_name, std::string Attr_type_name,
                                                      int* color_re, int* type_re, std::vector<cv::Mat> pic_list, std::vector<int> label) {
        caffe::MemoryDataLayer<float> *m_layer = (caffe::MemoryDataLayer<float> *)_net->layers()[0].get();
        m_layer->AddMatVector(pic_list, label);
        const int color_type = 11;
        const int car_type = 1232;

        std::string extract_feature_blob_names(feature_blob_name);
        /// modify by su
        std::cout<<"batch size: "<< _net->blob_by_name(extract_feature_blob_names)->num()<<std::endl;
        int num_mini_batches = count / _net->blob_by_name(extract_feature_blob_names)->num();
        /// init memory
        float* feature_dbs = new float[count * TOTALBYTESIZE ];

        std::vector<caffe::Blob<float>*> input_vec;
        Datum datum;
        const boost::shared_ptr<Blob<float> > feature_blob =
                _net->blob_by_name(extract_feature_blob_names);

        const boost::shared_ptr<Blob<float> > attr_type_blob =
                _net->blob_by_name(Attr_type_name);

        const boost::shared_ptr<Blob<float> > attr_color_blob =
                _net->blob_by_name(Attr_color_name);

        int batch_size = feature_blob->num();
        int dim_features = feature_blob->count() / batch_size;
        for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index) {
            /// std::cout<<"start"<<std::endl;
            _net->Forward(input_vec);
            const float* feature_blob_data;
            const float* feature_color_data;
            const float* feature_type_data;
            for (int n = 0; n < batch_size; ++n) {

                feature_color_data =
                        attr_color_blob->cpu_data() + attr_color_blob->offset(n);

                feature_type_data =
                        attr_type_blob->cpu_data() + attr_type_blob->offset(n);

                /// color
                float max = -1;
                int max_id = -1;
                for(int k = 0; k< color_type; k++){
                    if(feature_color_data[k] > max){
                        max = feature_color_data[k];
                        max_id = k;
                    }
                }
                color_re[n + batch_index*batch_size] = max_id;

                /// car
                max = -1;
                max_id = -1;
                for(int k = 0; k< car_type; k++){
                    if(feature_type_data[k] > max){
                        max = feature_type_data[k];
                        max_id = k;
                    }
                }
                type_re[n + batch_index*batch_size] = max_id;

                /// feature
                feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);
                for (int d = 0; d < dim_features ; ++d) {
                    feature_dbs[d + (n + batch_index*batch_size)*dim_features] = feature_blob_data[d];
                } // for (int d = 0; d < dim_features / 8; ++d)
            }  // for (int n = 0; n < batch_size; ++n)
        }  // for (int batch_index = 0; batch_index < num_mini_batches; ++batch_index)

        /// write the remain batch
        bool isRemain=false;
        int remain = count - num_mini_batches*(_net->blob_by_name(extract_feature_blob_names)->num());
        if(remain >0 ){
            isRemain=true;
            _net->Forward(input_vec);
        }
        if(isRemain){
            const float* feature_blob_data;
            const float* feature_color_data;
            const float* feature_type_data;
            for (int n = 0; n < remain; ++n) {//data new

                feature_color_data =
                        attr_color_blob->cpu_data() + attr_color_blob->offset(n);

                feature_type_data =
                        attr_type_blob->cpu_data() + attr_type_blob->offset(n);

                /// color
                float max = -1;
                for(int k = 0; k< color_type; k++){
                    if(feature_color_data[k] > max){
                        max = feature_color_data[k];
                    }
                }
                color_re[num_mini_batches*batch_size + n] = max;

                /// car
                max = -1;
                for(int k = 0; k< car_type; k++){
                    if(feature_type_data[k] > max){
                        max = feature_type_data[k];
                    }
                }
                type_re[num_mini_batches*batch_size + n] = max;

                feature_blob_data = feature_blob->cpu_data() + feature_blob->offset(n);
                for (int d = 0; d < dim_features; ++d) {
                    feature_dbs[(num_mini_batches*batch_size+n)*dim_features +d] = feature_blob_data[d];
                } // for (int d = 0; d < dim_features/8; ++d)
            }  // for (int n = 0; n < remian; ++n)
        }  // for (int i = 0; i < num_features; ++i)

        std::cout<<"Successfully"<<std::endl;
        return feature_dbs;
    }

}
