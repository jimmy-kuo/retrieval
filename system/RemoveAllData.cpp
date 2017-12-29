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
#include <utils.h>
#include <AuxIndexStructures.h>
#include <fstream>
#include <vector>

using namespace std;
using namespace feature_index;


std::string ROOT_OTHER_FILE = "/home/slh/faiss_index/model/";
// Person Model File
std::string person_proto_file = ROOT_OTHER_FILE + "person.prototxt";
std::string person_proto_weight = ROOT_OTHER_FILE + "person.caffemodel";
// Binary Model File
std::string binary_proto_file = ROOT_OTHER_FILE + "binary.prototxt";
std::string binary_proto_weight = ROOT_OTHER_FILE + "binary.caffemodel";
// car
std::string car_proto_file = ROOT_OTHER_FILE + "deploy_google_multilabel.prototxt";
std::string car_proto_weight = ROOT_OTHER_FILE + "wd_google_id_model_color_iter_100000.caffemodel";


std::string file_list_name ="./file_list";
#define INPUT_PARAM 1


int main(int argc,char** argv){
    google::InitGoogleLogging(argv[0]);
    FeatureIndex index = FeatureIndex();

    if(argc < 4 ){
        std::cout<<"argc : "<<argc<<" is not enough"<<std::endl;
        return 1;
    }

    std::string type = argv[1];
    std::string indexFile = argv[2];
    std::string infoFile = argv[3];
    int GpuNum = atoi(argv[4]);

    if(type != "person" && type != "car" && type != "binary"){
        std::cout<<"Type Error: Only 'car', 'person' are supported."<<std::endl;
        return 1;
    }
    faiss::Index* cpu_index_person = faiss::read_index(indexFile.c_str(), false);
    int hasNum = cpu_index_person->ntotal;
    std::cout<<"This index has  : "<<hasNum<<" ,all will be deleted."<<std::endl;

    faiss::gpu::StandardGpuResources resources_person;
    faiss::gpu::GpuClonerOptions* options = new faiss::gpu::GpuClonerOptions();
    options->usePrecomputed = false;
    faiss::gpu::GpuIndexIVFPQ* index_person = dynamic_cast<faiss::gpu::GpuIndexIVFPQ*>(
            faiss::gpu::index_cpu_to_gpu(&resources_person,GpuNum,cpu_index_person, options));

    faiss::IDSelector* ids = new faiss::IDSelectorRange(0, hasNum);
    index_person->remove_ids(*ids);

    { // I/O
        const char *outfilename = indexFile.c_str();
        faiss::Index * cpu_index = faiss::gpu::index_gpu_to_cpu (index_person);
        write_index (cpu_index, outfilename);
        delete cpu_index;
    }

    std::string deletefile = "rm " + infoFile;
    system(deletefile.c_str());
    printf ("done delete \n");

    return 0;
}
