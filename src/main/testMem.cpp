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

    long long dataCount = 2000000;

    float* data = new float[2000000*1024];




    int ui;
    std::cin>>ui;

    return 0;
}