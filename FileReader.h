//
// Created by galya on 30.06.23.
//

#ifndef BM25UTIL_FILEREADER_H
#define BM25UTIL_FILEREADER_H
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;


class FileReader {
public:
    struct Document {
        string url;
        string descr;
    };

    static vector<Document> readDir(const string &folderPath);
};


#endif //BM25UTIL_FILEREADER_H
