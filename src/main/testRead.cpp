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

    float* data = (float*)malloc(sizeof(float)* dataCount *dimension);
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
        fread(data + j*2000000*dimension, sizeof(float), 2000000* dimension , file);
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
        fread(data + (j-1)*2000000*dimension, sizeof(float), 2000000 * dimension , file);
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
    fread(data +9*2000000* dimension, sizeof(float), 2000000 * dimension , file);
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

        int numOfAdd = dataCount/1000000;

        char s='0';

        std::string newName = "index_IVFPQ_";
        std::string newNameten = "index_IVFPQ_1";
        int j=0;
        for(;j<numOfAdd;j++){
            fea.AddItemList(1000000, data+j*1000000);
            std::cout<<"Add "<<j<<" times Data Done"<<std::endl;

            { /// I/O write
                char temp;
                std::string newName1;

                if(j>=10){
                    temp = s+j-10;
                    newName1 = newNameten + temp + "QW";
                }else {
                    temp = s + j;
                    newName1 = newName + temp + "QW";
                }

                const char* outFileName = newName1.c_str();

                fea.WriteIndexToFile(outFileName);

                std::cout<<"Index "<<j<<" Save Done" << std::endl;

            }

            if(j%5==0){
                int Ktop = 10;

                int nquery = 5;

                float* Dis = new float[nquery * Ktop];

                long* ids = new long[nquery * Ktop];

                if( fea.getTotalIndex() != 0 ){

                    printf("data count: %ld \n", fea.getTotalIndex());

                    double t1 = elapsed();
                    fea.RetievalIndex(nquery, data+j*1024, Ktop, ids, Dis);
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
        }

        std::cout<<"Add Data Done"<<std::endl;

        int Ktop = 10;

        int nquery = 5;

        float* Dis = new float[nquery * Ktop];

        long* ids = new long[nquery * Ktop];

        if( fea.getTotalIndex() != 0 ){

            printf("data count: %ld \n", fea.getTotalIndex());

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