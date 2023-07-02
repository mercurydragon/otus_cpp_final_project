#include <fstream>
#include <sstream>
#include <utility>
#include "IndexIO.h"

void IndexIO::saveIdfs(const idfMap &idfs) {
    ofstream indexFile(ifdFilePath);
    if (indexFile.is_open()) {
        for (const auto &pair: idfs) {
            indexFile << pair.first << "-" << pair.second << endl;
        }
        indexFile.close();
    } else {
        cerr << "Unable to open file: " << ifdFilePath << endl;
    }
}

void IndexIO::saveDocFreq(const vector<DocumentFreqData> &termInDocFreqs, double avgdl) {
    ofstream file(docTermFreqFilePath);

    if (file.is_open()) {
        file << avgdl << endl;
        for (const auto &item: termInDocFreqs) {
            file << item.url << "-" << item.len << ":" << endl;
            for (const auto &innerPair: item.termFreqs) {
                file << "\t" << innerPair.first << ":" << innerPair.second << endl;
            }
            file << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << docTermFreqFilePath << endl;
    }
}

idfMap IndexIO::readIdf() {
    idfMap idfs;
    ifstream indexFile(ifdFilePath);
    string line;
    if (indexFile.is_open()) {
        while (getline(indexFile, line)) {
            auto [url, idf] = splitDocString(line);
            idfs[url] = idf;
        }
        indexFile.close();
    } else {
        throw invalid_argument("Unable to open file: " + ifdFilePath);
    }
    return idfs;
}

tuple<vector<DocumentFreqData>, double> IndexIO::readTermInDocFreq() {
    ifstream file(docTermFreqFilePath);
    string line;

    double avgdl = 0;
    vector<DocumentFreqData> termInDocFreqs;
    if (file.is_open()) {
        getline(file, line);
        avgdl = stod(line);
        auto item = DocumentFreqData();
        while (getline(file, line)) {
            if (line.empty())
                continue;

            if (line.back() == ':') {
                if (not item.url.empty()) termInDocFreqs.push_back(item);
                const auto [url, len] = splitDocString(line.substr(0, line.size() - 1));
                item = DocumentFreqData(url, len, unordered_map<string, int>());
            } else {
                size_t delimiterPos = line.find(':');
                if (delimiterPos != string::npos) {
                    string innerKey = line.substr(1, delimiterPos - 1);
                    int value = stoi(line.substr(delimiterPos + 1));
                    item.termFreqs[innerKey] = value;
                }
            }
        }
        termInDocFreqs.push_back(item);
        file.close();
    } else {
        throw invalid_argument("Unable to open file: " + docTermFreqFilePath);
    }
    return {termInDocFreqs, avgdl};
}

tuple<string, double> IndexIO::splitDocString(const string &input) {
    string first;
    double second;

    // Find the position of the dash '-'
    size_t dashPos = input.find('-');
    if (dashPos != string::npos) {
        // Extract the string part before the dash
        first = input.substr(0, dashPos);
        second = stod(input.substr(dashPos + 1));

        // Extract the integer part after the dash
        std::istringstream iss(input.substr(dashPos + 1));
        iss >> second;
    }

    return {first, second};
}

IndexIO::IndexIO(string docTermFreqFilePath, string ifdFilePath) : docTermFreqFilePath(std::move(
        docTermFreqFilePath)), ifdFilePath(std::move(ifdFilePath)) {}

