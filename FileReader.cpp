
#include <filesystem>
#include <fstream>

#include "FileReader.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

vector<FileReader::Document> FileReader::readDir(const string &folderPath) {
    vector<FileReader::Document> documents;
    for (const auto &entry: filesystem::directory_iterator(folderPath)) {
            if (entry.path().extension() == ".json") {
                ifstream file(entry.path());
                json documentJson;
                file >> documentJson;

                Document document;
                document.url = documentJson["url"].get<string>();
                document.descr = documentJson["descr"].get<string>();

                documents.push_back(document);
            }
        }
    return documents;
    }

