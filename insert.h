#ifndef DELETETHIS_INSERT_H
#define DELETETHIS_INSERT_H

#include <string>
#include "nlohmann/json.hpp"
#include "arr.h"
#include "common.h"

using namespace std;
using json = nlohmann::json;


struct insComm{
    arr<string> target;
    arr<string> data;
};

void sizeCheck(int insertionSize, const string& tableName, const json& structure);
insComm toInsQuery(arr<string> query);
void insert(const json& structure, arr<string> query);

#endif //DELETETHIS_INSERT_H
