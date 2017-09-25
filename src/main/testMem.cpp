//
// Created by slh on 17-9-22.
//

#include "Retrieval.h"
#include <sys/time.h>

double elapsed ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return  tv.tv_sec + tv.tv_usec * 1e-6;
}

#define MAX_DATA_COUNT 2000000

int main() {

    long long dataCount = 10000000;

    float* data = new float[2000000*1024];
    float* data1 = new float[2000000*1024];
    float* data2 = new float[2000000*1024];
    float* data3 = new float[2000000*1024];
    float* data4 = new float[2000000*1024];
    float* p[5]={data,data1,data2,data3,data4};
    /// Init FeatureIndex
    retrieval::FeatureIndex fea(dataCount);

    int dimension = fea.getDimension();

    /// calc num
    int cou = 10;

    /// load multi data
    std::string inputFile = "/home/slh/vehicleIndex/data_float_2000000_";

    if(dataCount > 2000000){

        for(int j=0;j<5;j++){
            char s = '0' + j;
            std::string name =  inputFile + s;
            std::cout<<name<<std::endl;
            FILE* file = fopen(name.c_str(), "rb");
            if(!file){
                std::cout<<"File Wrong"<<std::endl;
                return 1;
            }
            fread(p[j], sizeof(float), 2000000*dimension, file);
            for(int k=1900000;k<1900010;k++){
                for(int jk=0;jk<1024;jk++)
                    std::cout<<p[j][k*1024+jk]<<" ";
                std::cout<<std::endl;
            }
            std::cout<<std::endl;
            fclose(file);
        }


    }
    int ui;
    std::cin>>ui;

    return 0;
}