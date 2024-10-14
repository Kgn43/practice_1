#include "practice_1.h"


int getPkFormStr(const string& input){
    arr<string> splited = splitToArr(input, ';');
    return stoi(splited[0]);
}


int main() {
    string strcrt = "strktr.json";
    json structureJSON;
    try{
        ifstream jsonFile (strcrt);
        if (!jsonFile.is_open()){
            stringstream serr;
            serr << "The structure file does not exist";
            throw runtime_error(serr.str());
        }
        else {
            structureJSON = json::parse(jsonFile);
            jsonFile.close();
            makeStructure(structureJSON);
        }
    }
    catch(exception& ex) {
        cout << ex.what() << endl;
        return -1;
    }
    string input;
    bool isExit = false;
    cout << "Enter command" << endl;
    while (!isExit){
        getline(cin, input);
        arr<string> query = splitToArr(input);
        if (query.size == 0){
            cout << "query was not entered" << endl;
            continue;
        }
        commands comm = commandRequest(query[0]);
        try{
            switch (comm) {
                case Exit:
                    isExit = true;
                    break;
                case Insert:
                    insert(structureJSON, query);
                    break;
                case Del:
                    del(structureJSON, query);
                    break;
                case Select:

                    break;
                case Wrong:
                    system("cls");
                    cout << "Wrong syntax" << endl;
                    break;
            }
        }
        catch(exception& ex) {
            cout << ex.what() << endl;
        }
        cout << "Enter command" << endl;
    }
    return 0;


}
