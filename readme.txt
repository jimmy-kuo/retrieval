### 系统说明
	version：1.0
	*号为特别注意事项
	使用时请先编译附带的caffe

## retrieval使用说明
-------
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

## 人车属性提取使用说明
--------
	1. 车的属性提取代码和特征提取代码合并在一个文件中
		即： ./cfeature/Feature.cpp
	    a) 使用PictureAttrExtraction函数即可从车辆图片中提取color和type属性id
		b) 使用示例见： ./test/testVehicleAttr.cpp
	2. 人的属性提取代码在./personAttr/目录下
		即： personAttr.cpp
	    a) 使用示例见: ./test/testPersonAttr.cpp
		*b) 使用时注意修改./include/personAttr.h文件中的ROOT_DIR_ATTR为personAttrFile目录的路径

## 特征提取代码使用说明
--------
	1. 特征提取代码在./cfeature/目录下
	2. 具体使用方法和车辆属性提取方法一致，仅使用函数不同
		a) 具体使用方法见:  ./test/testVehicleAttr.cpp
		a) 人车特征提取均可以使用PictureFeatureExtraction函数
		b) 注意,人和车的特征提取方法仅在initNet时不同，需要输入对应的prototxt文件和model文件
	3. 基本prototxt和model文件均在./model/路径下
		a) 附带有_person文件名的均为人的特征提取所要使用的文件，_vehicle同理为车所需
		*b) 特别注意：
			使用时修改prototxt文件中的source路径，使得与程序内的file_list文件路径一致

## 数据库使用说明
--------
	1. 本封装代码，仅支持mysql
	2. 在./include/featureSql.h中，修改FeatureSql()函数内的数据库的相关参数
	3. 建立数据库的代码未给出：
		a) 请使用searchWithUdType(user_define_type)函数进行mysql的查询
		b) 具体使用细节见 ./test/testSql.cpp
	4. 建立数据库的列名(个数与所给属性对应)可任意，但值应该均为整数
		a) 具体属性值和整数对应关系如下：
			人： ./personAttrFile/att_index.txt
			车： ./VehicleAttr/color_class.txt
				 ./VehicleAttr/vehicle_type.txt
		b) 可以使用sql封装代码中的InitMapColor, InitMapType, InitPersonAttr
		   分别初始化车的颜色对应关系，车的车型对应关系，人的属性对应关系
		   并且由_id_map_color, _id_map_type, _id_person_attr三个变量分别获取
		   eg. 1.初始化之后，string value = _id_person_attr["hair"][0];
			    即hair属性的0值为value = “black”
			   2. string value = _id_map_color[0];
				即车的color颜色属性0值对应value=“棕”色

作者:
    email: slh@pku.edu.cn