//
// Created by slh on 17-10-27.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "featureSql.h"
#include <mysql/mysql.h>

using namespace FeatureSQL;
using namespace std;

void FeatureSQL::FeatureSql::InitMapColor(std::string file, int count) {
    std::ifstream vehicle_color (file, std::ios::in);
    this->_id_map_color = new id_map_color[count];
    for(int i=0;i<count;i++){
        int temp;
        vehicle_color >> temp >>_id_map_color[i].info ;
    }
    return ;
}
// 3000
void FeatureSQL::FeatureSql::InitMapType(std::string file, int count) {
    std::ifstream vehicle_type (file, std::ios::in);
    this->_id_map_type = new id_map[count];
    int temp;
    while(cin>>temp){
        vehicle_type >>_id_map_color[temp].info ;
    }
    return ;
}

void FeatureSQL::FeatureSql::InitPersonAttr(std::string file, int count) {
    std::ifstream person_attr (file, std::ios::in);
    int temp,a,b;
    std::string name,name1,before="";
    int num =0;
    while(cin>>temp>>name>>a>>name1>>b){
        if(before == "" || before != name){
            std::string* te = new  std::string[20];
            te[a] = name1;
            _id_person_attr.insert(std::pair<std::string, std::string*>(name, te));
            num ++;
            before = name;
            continue;

        }else{
            std::string* te = _id_person_attr[name];
            te[a] = name1;
        }
    }
    return ;
}

int* FeatureSQL::FeatureSql::searchWithColor(std::string colorName, int id, int& row_count) {

    string sql = "select id from car where color_id = ";
    stringstream ss;
    ss<<id;
    mysql_query( &_mysql, (sql + ss.str()).c_str() );
    MYSQL_RES *sqlresult = NULL;
    sqlresult = mysql_store_result( &_mysql );

    return HandleResult(sqlresult, row_count);

}

int* FeatureSQL::FeatureSql::searchWithType(std::string typeName, int id, int& row_count) {

    string sql = "select id from car where type_id = ";
    stringstream ss;
    ss<<id;
    mysql_query( &_mysql, (sql + ss.str()).c_str() );
    MYSQL_RES *sqlresult = NULL;
    sqlresult = mysql_store_result( &_mysql );

    return HandleResult(sqlresult, row_count);
}

int* FeatureSQL::FeatureSql::HandleResult(MYSQL_RES *res, int& row_co) {

    int row_count = mysql_num_rows( res );
    row_co = row_count;
    int* result = new int[row_count];

    /// result
    MYSQL_ROW row = NULL;
    row = mysql_fetch_row( res );
    int num = 0;
    while ( NULL != row )
    {
        result[num] = atoi(row[0]);
        row = mysql_fetch_row( res );
    }

    mysql_free_result(res);

    return result;
}

int* FeatureSQL::FeatureSql::searchWithUdType(std::string table, std::vector<std::string> typeName,
                                              std::vector<std::string> relation, std::vector<int> id, int& row_count){
    // essemble table
    string sql = AssembleSQL(table, typeName, relation, id);
    std::cout<<sql<<std::endl;
    if(sql == ""){
        row_count = 1;
        return NULL;
    }

    mysql_query( &_mysql, (sql).c_str() );
    MYSQL_RES *sqlresult = NULL;
    sqlresult = mysql_store_result( &_mysql );

    return HandleResult(sqlresult, row_count);
}

std::string FeatureSQL::FeatureSql::AssembleSQL(std::string table, std::vector<std::string> typeName,
                                                std::vector<std::string> relation, std::vector<int> id){
    if(typeName.size() != relation.size()+1 || typeName.size() != id.size() || relation.size()+1 != id.size()){
        std::cout<<"SQL wrong, please check your query"<<std::endl;
        return "";
    }
    int size = typeName.size();
    bool islabel = false;
    std::string sq = "select id from " + table + " where ";
    for(int j=0;j<size;j++){
        stringstream ss;
        ss << id[j];
        if(!islabel) {
            sq += "(" + typeName[j] + " = " + ss.str();
            islabel = true;
        }else{
            sq += typeName[j] + " = " + ss.str();
        }
        if(j+1 == size){
            if(islabel)
                sq += ");";
        }else{
            if(relation[j] == "and" ){
                islabel = false;
                sq += " ) and ";
            }else{
                sq += " or ";
            }

        }
    }

    return sq;
}