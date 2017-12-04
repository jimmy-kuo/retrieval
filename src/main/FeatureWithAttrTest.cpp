//
// Created by slh on 17-10-27.
//

#include <Feature.h>
#include "boost/algorithm/string.hpp"
#include "Retrieval.h"
#include <faiss/index_io.h>
#include "featureSql.h"
#include <fstream>
#include <vector>

using namespace std;
using namespace Feature;

#define DATA_BINARY 371
#define FAISS_GPU 10

/// Search one file :
///     1. extract feature
///     2. search (faiss search)
///     3. mysql SQL query
///     4. intersection '2.' and '3.' results

int main(int argc,char** argv){
    google::InitGoogleLogging(argv[0]);
    FeatureIndex index = FeatureIndex();
    // TODO:: proto File and model file change
    std::string proto_file = "/home/slh/retrieval/model/deploy_vehicle.prototxt";
    std::string proto_weight = "/home/slh/retrieval/model/wd_google_id_model_color_iter_100000.caffemodel";

    if(argc <= 1 ){
        std::cout<<"argc : "<<argc<<" is not enough"<<std::endl;
        return 1;
    }

    string file_list = argv[1];
    int count = 1;
    string index_filename = argv[2];

    Info_String* info = new Info_String[count];
    string temp;
    // TODO:: caffe read images path and file_list change
    // change root_dir and file_list
    std::string ROOT_DIR = "/home/slh/data/test/new/";
    std::string file_list_name ="/home/slh/retrieval/model/file_list";
    std::ofstream output(file_list_name,std::ios::out);
    for(int k =0; k<count; k++){
        strcpy(info[k].info, (ROOT_DIR + file_list).c_str());
        output<<info[k].info<<std::endl;
    }
    output.close();

    caffe::Net<float>* net = index.InitNet(proto_file, proto_weight);

    float* data ;
    int* color_re = new int[count];
    int* type_re = new int[count];

    /// 1. extract feature
    ///    Vehicle Attr fetch details are in testVehicleAttr.cpp
    data = index.PictureAttrFeatureExtraction(count, net, "pool5/7x7_s1",
                                              "color/classifier", "model_loss1/classifier",
                                              color_re, type_re);

    /// 2. Init FeatureIndex:
    ///     Retrieval details are in testRead.cpp && testRetrieval.cpp
    retrieval::FeatureIndex fea;
    { /// I/O Read
        const char* FileName = "index_IVFPQ";
        fea.ReadIndexFromFile(FileName);
        std::cout<<"Index Read Done" << std::endl;
    }

    /// 2. retrieval result return
    int nq = 1;
    int k = 100;
    std::vector<faiss::Index::idx_t> nns (k * nq);
    std::vector<float>               dis (k * nq);
    fea.setProbe(15);
    fea.RetievalIndex(nq, data, k,  nns.data(), dis.data());

    /// 3. SQL query
    ///   Sql query details are in test/testSql.cpp,
    ///   Person Attr fetch details are in testPersonAttr.cpp
    ///   Vehicle Attr fetch details are in testVehicleAttr.cpp
    FeatureSQL::FeatureSql sql;
    int row_count = 0 ;
    ///   Query color
    int* sql_result = sql.searchWithColor("", 0, row_count);


    /// 4. intersection '2.' and '3.' results
    int* total_res = new int[k];
    int hasNum = 0;
    for(int kk=0;kk<100;kk++){
        for(int i=0; i<row_count; i++){
            if( sql_result[i] == nns[kk] ){
                total_res[hasNum] = kk;
                hasNum++;
            }
        }
    }
    std::cout<<hasNum<<std::endl;
    delete data;
    return 0;
}
