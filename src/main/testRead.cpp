//
// Created by slh on 17-9-6.
//

#include "Retrieval.h"
#include <sys/time.h>

double elapsed ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return  tv.tv_sec + tv.tv_usec * 1e-6;
}

int main() {

    int dataCount = 20000000;

    /// Init FeatureIndex
    retrieval::FeatureIndex fea;

    { /// I/O Read

        const char* FileName = "index_IVFPQ_10000000";

        fea.ReadIndexFromFile(FileName);

        std::cout<<"Index Read Done" << std::endl;

    }

    int dimension = fea.getDimension();

    std::cout<<"Dimension is: "<< dimension << std::endl;

    float* data = new float[dataCount * dimension];

    /// load multi data
    std::string inputFile = "/home/slh/vehicleIndex/data_float_2000000_";
    for(int j=0;j<5;j++){
        char s = '0' + j;
        std::string name =  inputFile + s;
        std::cout<<name<<std::endl;
        FILE* file = fopen(name.c_str(), "rb");
        if(!file){
            std::cout<<"File Wrong"<<std::endl;
            return 1;
        }
        fread(data + j*dataCount* dimension/10, sizeof(float), dataCount* dimension / 10, file);
        fclose(file);
    }

    for(int j=6;j<10;j++){
        char s = '0' + j;
        std::string name =  inputFile + s;
        std::cout<<name<<std::endl;
        FILE* file = fopen(name.c_str(), "rb");
        if(!file){
            std::cout<<"File Wrong"<<std::endl;
            return 1;
        }
        fread(data + (j-1)*dataCount* dimension/10, sizeof(float), dataCount * dimension/ 10, file);
        fclose(file);
    }

    for(int k=1000000;k<1000100;k++){
        std::cout<<data[k]<<" ";
    }
    std::cout<<std::endl;


    /// load data
    FILE* file = fopen("/home/slh/vehicleIndex/data_float_2000000_10", "rb");
    if(!file){
        std::cout<<"File Wrong"<<std::endl;
        return 1;
    }
    fread(data +9*dataCount* dimension/10, sizeof(float), dataCount * dimension /10, file);
    fclose(file);

    std::cout<<"File Done"<<std::endl;

    /// open train verbose, default false
    fea.setTranVerbose(true);

    /// Train Index
    /// In here, Put all data for Training
    if(! fea.isTrainIndex()){

        std::cout<<"Begin Train"<<std::endl;
        fea.TrainIndex(dataCount, data);

    }

    { /// Add Retrieval data && Retrieval

        std::cout<<"Begin Add Data"<<std::endl;

        fea.AddItemList(dataCount, data);

        std::cout<<"Add Data Done"<<std::endl;

        int Ktop = 10;

        int nquery = 5;

        float* Dis = new float[nquery * Ktop];

        long* ids = new long[nquery * Ktop];

        if( fea.getTotalIndex() != 0 ){

            double t1 = elapsed();
            fea.RetievalIndex(nquery, data, Ktop, ids, Dis);
            double t2 = elapsed();

            printf("time: %lf \n", t2-t1);

        }

        printf("I=\n");
        for(int i = 0; i < nquery; i++) {
            for(int j = 0; j < Ktop; j++)
                printf("%5ld ", ids[i * Ktop + j]);
            printf("\n");
        }

        printf("D=\n");
        for(int i = 0; i < nquery; i++) {
            for(int j = 0; j < Ktop; j++)
                printf("%7g ", Dis[i * Ktop + j]);
            printf("\n");
        }


    }

    return 0;
}