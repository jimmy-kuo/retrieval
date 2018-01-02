//
// Created by slh on 17-11-15.
//

#include <feature.h>
#include "binary.h"
#include "boost/algorithm/string.hpp"
#include <gpu/StandardGpuResources.h>
#include <gpu/GpuIndexIVFPQ.h>
#include <gpu/GpuAutoTune.h>
#include <index_io.h>
#include <fstream>
#include <vector>

using namespace std;
using namespace feature_index;


std::string ROOT_OTHER_FILE = "../data/";
// Person Model File
std::string person_proto_file = ROOT_OTHER_FILE + "dperson.prototxt";
std::string person_proto_weight = ROOT_OTHER_FILE + "person.caffemodel";
// Binary Model File
std::string binary_proto_file = ROOT_OTHER_FILE + "dbinary.prototxt";
std::string binary_proto_weight = ROOT_OTHER_FILE + "dbinary.caffemodel";
// car
std::string car_proto_file = ROOT_OTHER_FILE + "dcar.prototxt";
std::string car_proto_weight = ROOT_OTHER_FILE + "car.caffemodel";


std::string file_list_name ="./file_list";
#define INPUT_PARAM 1


int main(int argc,char** argv){
    google::InitGoogleLogging(argv[0]);
    FeatureIndex index = FeatureIndex();

    if(argc <= 3 ){
        std::cout<<"argc : "<<argc<<" is not enough"<<std::endl;
        return 1;
    }

    std::string type = argv[1];
    std::string indexFile = argv[2];
    std::string infoFile = argv[3];

    if( type != "num" && argc < 7){
        std::cout<<"type wrong, 'num' is supported when argc has only three."<<std::endl;
        return 1;
    }

    faiss::Index* cpu_index_person = faiss::read_index(indexFile.c_str(), false);
    int hasNum = cpu_index_person->ntotal;


    if( type == "num" && argc < 7){
        Info_String *info = new Info_String[hasNum];
        if(hasNum != 0) {
            FILE *_f = fopen(infoFile.c_str(), "rb");
            fread(info, sizeof(Info_String), hasNum, _f);
            fclose(_f);
        }
        std::cout<<"This index has  : "<<hasNum<<std::endl;
        std::cout<<"Last info in this index file:  "<< info[hasNum - 1].info<<std::endl;
        return 1;
    }

    std::string related_path;
    std::string FileList;
    int count;
    int GpuNum;
    if(argc > 3){
        related_path = argv[4]; // end with /
        FileList = argv[5];
        count = atoi(argv[6]);
        GpuNum = atoi(argv[7]);
    }
    Info_String *info = new Info_String[hasNum + count];
    if(hasNum != 0) {
        FILE *_f = fopen(infoFile.c_str(), "rb");
        fread(info, sizeof(Info_String), hasNum, _f);
        fclose(_f);
    }

    std::cout<<"This index has  : "<<hasNum<<" this script will append data behind it. please make sure."<<std::endl;
    if(type != "person" && type != "car" && type != "binary"){
        std::cout<<"Type Error: Only 'car', 'person' are supported."<<std::endl;
        return 1;
    }

    std::vector<std::string> file_name_list;
    std::ofstream output(file_list_name,std::ios::out);
    std::ifstream input(FileList, std::ios::in);
    std::string temp;
    for(int i=0;i<count;i++){
        getline(input, temp);
        boost::split(file_name_list, temp, boost::is_any_of(" ,!"), boost::token_compress_on);
        if(file_name_list.size() < INPUT_PARAM + 1){
            std::cout<<"File List context wrong, need "<<INPUT_PARAM+1<< " ,but provided only "<<file_name_list.size()<<std::endl;
            return 1;
        }
        output<< related_path + "/" +  file_name_list[0]<<std::endl;
        std::string tmp=file_name_list[0];
        for(int j=1;j<=INPUT_PARAM;j++){
            tmp +=" "+ file_name_list[j];
        }
        memcpy(info[hasNum+i].info, tmp.c_str(), tmp.length());
    }
    output.close();
    input.close();
    caffe::Net<float>* net;
    if(type == "person") {
        net = index.InitNet(person_proto_file, person_proto_weight);
    }else if (type == "car"){
        net = index.InitNet(car_proto_file, car_proto_weight);
    }else if(type=="binary"){
        net = index.InitNet(binary_proto_file, binary_proto_weight);
    }else{
        std::cout<<"Type Error: Only 'car', 'person' are supported."<<std::endl;
        return 0;
    }

    float* data ;
    if(type == "binary"){
        index.InitGpu("GPU", GpuNum);
        //TODO: BINARY
        //        data = index.(count, net, "pool5/7x7_s1");
    }else if(type == "car"){
        index.InitGpu("GPU", GpuNum);
        data = index.PictureFeatureExtraction(count, net, "pool5/7x7_s1");
    }else{
        index.InitGpu("GPU", GpuNum);
        data = index.PictureFeatureExtraction(count, net, "loss3/feat_normalize");
    }
    /// int count, caffe::Net<float> * _net, std::string blob_name
    delete net;
    printf ("done feature \n");

    faiss::gpu::StandardGpuResources resources_person;
    faiss::gpu::GpuClonerOptions* options = new faiss::gpu::GpuClonerOptions();
    options->usePrecomputed = false;
    faiss::gpu::GpuIndexIVFPQ* index_person = dynamic_cast<faiss::gpu::GpuIndexIVFPQ*>(
            faiss::gpu::index_cpu_to_gpu(&resources_person,GpuNum,cpu_index_person, options));
    index_person->add(count, data);

    { // I/O
        const char *outfilename = indexFile.c_str();
        faiss::Index * cpu_index = faiss::gpu::index_gpu_to_cpu (index_person);
        write_index (cpu_index, outfilename);
        delete cpu_index;
    }

    FILE* _f = fopen(infoFile.c_str(), "wb");
    fwrite(info, sizeof(Info_String), hasNum + count, _f);
    fclose(_f);

    printf ("done save \n");

    delete[] data;
    return 0;
}
