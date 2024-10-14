#include "select.h"


selectComm toSelectQuery(arr<string> query){
    if (query.size <= 3){
        stringstream serr;
        serr << "wrong command: syntax error";
        throw runtime_error(serr.str());
    }
    if (query[2] == "from"){
        stringstream serr;
        serr << "wrong command: there is nothing to select";
        throw runtime_error(serr.str());
    }
    size_t fromPlc = 0;
    for (size_t i = 0; i < query.size; ++i){
        if (query[i] == "from") fromPlc = i;
    }
    if (fromPlc == 0){
        stringstream serr;
        serr << "wrong command: there is no keyword\"from\'";
        throw runtime_error(serr.str());
    }
    if (query[fromPlc + 1] == "where"){
        stringstream serr;
        serr << "wrong command: there is no tables";
        throw runtime_error(serr.str());
    }
    size_t wherePlc = query.size;
    for (size_t i = 0; i < query.size; ++i){
        if (query[i] == "from") fromPlc = i;
    }
    selectComm output;
    for (size_t i = 1; i < fromPlc; ++i){//записать условие
        output.columns.push_back(query[i]);
    }
    for (size_t i = fromPlc + 1; i < wherePlc; ++i){//записать условие
        output.tables.push_back(query[i]);
    }
    if (wherePlc == query.size){
        output.condition = {};
    }
    else {
        for (size_t i = wherePlc + 1; i < query.size; ++i){//записать условие
            output.condition.push_back(query[i]);
        }
    }
    return output;
}


bool isItColumn(json structure, const string& name) {
    for (json::iterator it = structure["structure"].begin(); it != structure["structure"].end(); ++it) {
        for (const auto & colName : it.value()){
            if (colName == name) return true;
        }
    }
    return false;
}


string findTableName(json structure, const string& name){
    for (json::iterator it = structure["structure"].begin(); it != structure["structure"].end(); ++it) {
        for (const auto & colName : it.value()){
            if (colName == name) return it.key();
        }
    }
    return "";
}


int getIndexFromStr(const string& input){
    string number;
    for (const auto& ch : input){
        if (ch == ';'){
            break;
        }
        else {
            number += ch;
        }
    }
    return stoi(number);
}


arr<int> addIfNotUnic(arr<int> arr1, arr<int> arr2){
    arr<int> output;
    for (size_t i = 0; i < arr1.size; ++i){
        for (size_t j = 0; j < arr2.size; ++j){
            if (arr1[i] == arr2[j]){
                output.push_back(arr1[i]);
                break;
            }
        }
    }
    return output;
}


arr<int> connectCondition(const arr<arr<arr<int>>> &arr1){
    arr<int> outputArr;
    arr<int> andArr;
    for (size_t i = 0; i < arr1.size; i++){
        for (size_t j = 0; j < arr1[i].size; j++){
            if (j == 0){
                andArr = arr1[i][j];
            }
            else{
                andArr = addIfNotUnic(andArr, arr1[i][j]);
            }
        }
        outputArr.unicAppend(andArr);
        outputArr.sort();
    }
    return outputArr;
}


arr<int> getPassNum(const json& structure, const arr<arr<arr<string>>>& condition){
    arr<arr<arr<int>>> indexes;
    string firstOperand;
    string secondOperand;
    string oper;
    ifstream firstStream;
    ifstream secondStream;
    string table1Name;
    string table2Name;
    string wayToTable;
    int firstCurrentPk;
    int secondCurrentPk;
    string firstPath;
    string secondPath;
    string firstGottenLine;
    string secondGottenLine;
    arr<string> firstSplitedLine;
    arr<string> secondSplitedLine;
    arr<string> firstTableHeaders;
    arr<string> secondTableHeaders;
    int firstHeaderNumber;
    int secondHeaderNumber;
    for (size_t i = 0; i < condition.size; ++i){
        for (size_t j = 0; j < condition[i].size; ++j){
            if (condition[i][j].size != 3){
                stringstream serr;
                serr << "wrong condition";
                throw runtime_error(serr.str());
            }
            firstOperand = condition[i][j][0];
            oper = condition[i][j][1];
            secondOperand = condition[i][j][2];
            if (isItColumn(structure, secondOperand)){
                if (!isItColumn(structure, firstOperand)){
                    stringstream serr;
                    serr << "wrong table name";
                    throw runtime_error(serr.str());
                }
                else {
                    table1Name = findTableName(structure, firstOperand);//находим имя таблицы
                    tableCheck(structure, table1Name);//проверяем на случай отсутствия
                    table2Name = findTableName(structure, secondOperand);//находим имя таблицы
                    tableCheck(structure, table2Name);//проверяем на случай отсутствия
                    firstPath = static_cast<string>(structure["name"]) + "/" + table1Name + "/" + table1Name;// уже не директории!!
                    secondPath = static_cast<string>(structure["name"]) + "/" + table2Name + "/" + table2Name;
                    firstCurrentPk = getCurrPk(firstPath); //текущий Pk1
                    secondCurrentPk = getCurrPk(secondPath); //текущий Pk2
                    //для всех файлов таблиц
                    for (int k = 0; k <= firstCurrentPk / static_cast<int>(structure["tuples_limit"]) && k <= secondCurrentPk / static_cast<int>(structure["tuples_limit"]); ++k){
                        if (k != 0){//в каком мы файле?
                            wayToTable = "_" + to_string(i) + ".csv";
                        }
                        else {
                            wayToTable = ".csv";
                        }
                        //открываем потоки чтения файлов таблиц
                        firstStream.open(firstPath + wayToTable);
                        secondStream.open(secondPath + wayToTable);
                        //разбиваем заголовки на массив
                        firstTableHeaders = getHeaders(firstPath + wayToTable);
                        secondTableHeaders = getHeaders(secondPath + wayToTable);
                        //находим в этих заголовках индекс колонки из проверяемого условия
                        firstHeaderNumber = firstTableHeaders.find(firstOperand);
                        secondHeaderNumber = secondTableHeaders.find(firstOperand);
                        //считываем строки пока можем
                        while (getline(firstStream, firstGottenLine) && getline(secondStream, secondGottenLine)){
                            if (oper == "="){
                                //разбиваем строку для обращения по индексу колонки
                                firstSplitedLine = splitToArr(firstGottenLine, ';');
                                secondSplitedLine = splitToArr(secondGottenLine, ';');
                                //проверка ОДНОГО условия
                                if (firstSplitedLine[firstHeaderNumber] == secondSplitedLine[secondHeaderNumber]) {
                                    indexes[i][j].push_back(getIndexFromStr(firstGottenLine));
                                }
                            }
                            else {
                                stringstream serr;
                                serr << "wrong operator";
                                throw runtime_error(serr.str());
                            }
                        }
                        firstStream.close();
                        secondStream.close();
                    }
                }
            }
            else {
                table1Name = findTableName(structure, firstOperand);//находим имя таблицы
                tableCheck(structure, table1Name);//проверяем на случай отсутствия
                firstPath = static_cast<string>(structure["name"]) + "/" + table1Name + "/" + table1Name;// уже не директория!!
                firstCurrentPk = getCurrPk(firstPath); //текущий Pk1
                //для всех файлов таблиц
                for (int k = 0; k <= firstCurrentPk / static_cast<int>(structure["tuples_limit"]); ++k){
                    if (k != 0){//в каком мы файле?
                        wayToTable = "_" + to_string(i) + ".csv";
                    }
                    else {
                        wayToTable = ".csv";
                    }
                    //открываем потоки чтения файлов таблиц
                    firstStream.open(firstPath + wayToTable);
                    //разбиваем заголовки на массив
                    firstTableHeaders = getHeaders(firstPath + wayToTable);
                    //находим в этих заголовках индекс колонки из проверяемого условия
                    firstHeaderNumber = firstTableHeaders.find(firstOperand);
                    //считываем строки пока можем
                    while (getline(firstStream, firstGottenLine)){
                        if (oper == "="){
                            //разбиваем строку для обращения по индексу колонки
                            firstSplitedLine = splitToArr(firstGottenLine, ';');
                            //проверка ОДНОГО условия
                            if (firstSplitedLine[firstHeaderNumber] == secondOperand) {
                                indexes[i][j].push_back(getIndexFromStr(firstGottenLine));
                            }
                        }
                        else {
                            stringstream serr;
                            serr << "wrong operator";
                            throw runtime_error(serr.str());
                        }
                    }
                    firstStream.close();
                }
            }
        }
    }
    arr<int> pass = connectCondition(indexes);
    return pass;
}


string getValueByIndex(json structure, const string& tableName, const arr<string>& columnsName, int index){
    string path = static_cast<string>(structure["name"]) + "/" + tableName + "/" + tableName;
    arr<string> headers = getHeaders(path + ".csv");
    int ind;
    for (size_t i = 0; i < columnsName.size; ++i){
        ind = headers.find(columnsName[i]);
        if (ind != -1) break;
    }
    if (index > 1000){//в каком мы файле?
        path += "_" + to_string((index/1000)) + ".csv";
    }
    else {
        path += ".csv";
    }
    ifstream stream;
    string gottenLine;
    stream.open(path);
    arr<string> splitedLine;
    while (getline(stream, gottenLine)){
        if (getIndexFromStr(gottenLine) == index){
            splitedLine = splitToArr(gottenLine, ';');
            return splitedLine[ind];
        }
    }
}


void select(const json& structure, arr<string> inputQuery){
    selectComm query = toSelectQuery(inputQuery);//получаем имя таблиц, колонки и условия для выборки
    for (size_t i = 0; i < query.tables.size; ++i){//для всех таблиц проверяем их существование
        tableCheck(query.tables[i], structure);
    }
    arr<string> cond; //младший сын
    cond = splitToArr(unsplit(query.condition), " OR ");
    arr<arr<string>> condit; //средний сын
    for (size_t i = 0; i < cond.size; ++i){
        condit.push_back(splitToArr(cond[i], " AND "));
    }
    arr<arr<arr<string>>> condition; //старший сын
    for (size_t i = 0; i < condit.size; ++i){
        for (size_t j = 0; j < condit[i].size; ++j){
            condit.push_back(splitToArr(condit[i][j], ' '));
        }
    }
    arr<int> nums = getPassNum(structure, condition);
    string firstWord;
    string secondWord;
    ofstream crossJoin("crossJoin.csv");
    for (size_t i = 0; i < nums.size; ++i){
        firstWord = getValueByIndex(structure, query.tables[0], query.columns, nums[i]);
        for (size_t j = 0; j < nums.size; ++j){
            secondWord = getValueByIndex(structure, query.tables[1], query.columns, nums[j]);
            crossJoin << firstWord << ';' << secondWord << endl;
        }
    }
}
