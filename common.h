#ifndef DELETETHIS_COMMON_H
#define DELETETHIS_COMMON_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>


using namespace std;
using json = nlohmann::json;

void lock(const string &name);
void unlock(const string &name);
int getCurrPk(const string &path);
void tableCheck(const string& tableName, const json& structure);

#endif //DELETETHIS_COMMON_H
