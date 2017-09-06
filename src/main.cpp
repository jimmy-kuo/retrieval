#include "Retrieval.h"
#include <sys/time.h>

double elapsed ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return  tv.tv_sec + tv.tv_usec * 1e-6;
}

int main() {

    int dataCount = 1080000;

    /// Init FeatureIndex
    retrieval::FeatureIndex fea(dataCount);

    int dimension = fea.getDimension();

    float* data = new float[dataCount * dimension];

    /// load data
    FILE* file = fopen("../test/data_wendeng_108w_color", "rb");
    if(!file){
        std::cout<<"File Wrong"<<std::endl;
        return 1;
    }
    fread(data, sizeof(float), dataCount * dimension, file);
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

    std::cout<<"Train Done"<<std::endl;

    { /// I/O write

        const char* outFileName = "index_IVFPQ";

        fea.WriteIndexToFile(outFileName);

        std::cout<<"Index Save Done" << std::endl;

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