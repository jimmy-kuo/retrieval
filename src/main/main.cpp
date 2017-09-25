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
    retrieval::FeatureIndex fea(dataCount);

    int dimension = fea.getDimension();

    float* data = (float*)malloc(sizeof(float)*20000000 * dimension);

    /// calc num
    int cou = 10;

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
        fread(data+j*2000000*dimension, sizeof(float), 2000000*dimension, file);
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
        fread(data+(j-1)*2000000*dimension, sizeof(float), 2000000*dimension, file);
        fclose(file);
    }

//    for(int k=6000000;k<6000010;k++){
//        for(int j=0;j<1024;j++)
//            std::cout<<*(data+k*1024+j)<<" ";
//        std::cout<<std::endl;
//    }
//    std::cout<<std::endl;
//    /// single file
//    FILE* file = fopen("../test/data_wendeng_108w_color", "rb");
//    if(!file){
//        std::cout<<"File Wrong"<<std::endl;
//        return 1;
//    }
//    fread(data, sizeof(float), dataCount / cou * dimension, file);
//    fclose(file);

//    /// circle reality
//    for(int j=1;j<cou;j++){
//        for(int k=0;k<dataCount/cou;k++) {
//            for(int l=0;l<dimension;l++) {
//                data[j * dataCount / cou * dimension + dataCount / cou * dimension + l] = data[dataCount / cou * dimension + l];
//            }
//        }
//    }

    std::cout<<"File Done"<<std::endl;

    /// open train verbose, default false
    fea.setTranVerbose(true);

    /// Train Index
    /// In here, Put all data for Training
    if(! fea.isTrainIndex()){

        std::cout<<"Begin Train"<<std::endl;
        fea.TrainIndex(dataCount, data);

    }
//    int uiuiui;
//    std::cin>>uiuiui;
    std::cout<<"Train Done"<<std::endl;

    { /// I/O write

        const char* outFileName = "index_IVFPQ_10000000";

        fea.WriteIndexToFile(outFileName);

        std::cout<<"Index Save Done" << std::endl;

    }

    { /// Add Retrieval data && Retrieval

        std::cout<<"Begin Add Data"<<std::endl;

        /// test for 5000000
        fea.AddItemList(dataCount/2, data);

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