1. 在retrieval目录下新建目录build, 并进入该目录
    mkdir build
    cd build
2. 在build目录下输入
    cmake ../
    make clean
    make retrieval && make 
3. 在bin/目录下生成有测试代码，在lib/下为检索库


使用说明:
    1.  保证系统已经安装openblas
    2.  并且在/usr/local/lib 或者 /usr/lib下能找到openblas的链接库
    3.  若出现IndexFlat* IndexIVFPQ链接问题，请在faiss/目录下重新编译
            eg.  make clean && make
    4.  测试代码main.cpp与testRead.cpp仅供使用参考，实际运行时缺少数据文件(文件较大)，若需要请联系作者
    5.  参数建议使用默认，按照main.cpp提供的示例进行使用，若需要修改需要了解PQ算法，或者联系作者咨询
    6.  faiss代码有所修改，删去了不必要的部分


作者:
    email: sk_slh@bupt.edu.cn