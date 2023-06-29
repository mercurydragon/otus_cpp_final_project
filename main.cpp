//
// Created by galya on 25.06.23.
//

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <sstream>
#include <iterator>
#include <boost/tokenizer.hpp>
#include <stack>
#include "nlohmann/json.hpp"
#include "fixew_priority_queue.h"

using namespace std;
using json = nlohmann::json;
using tokenizer = boost::tokenizer<boost::char_separator<char>>;

const auto EPSILON = 0.25;
const auto B = 0.75;
const auto K1 = 1.5;
const auto DELTA = 0.5;
// переписать indexDocuments по аналогии с _initialize + _calc_idf
// переписать сохранение и считывание с диска, не забываем переменны магические числа (постоянные хардкодим)

//Добавить тупую нормализацию

static const auto IDFS_RESULT_PATH = "idfs_res.txt";

static const auto DOC_FREQ_RESULT_PATH = "word_freq_res.txt";

static const auto TOP_COUNT = 3;

// переписватьподсчет скоров как get_scores, выдать топчик с помощью очереди с приоритетом
// Функция для разделения текста на отдельные слова с использованием Boost tokenizer
vector<string> tokenize(const string &text) {
    vector<string> words;
    boost::char_separator<char> separator(" \t\n\r\f.,;:\"\'()[]{}<>!?-");
    tokenizer tokens(text, separator);

    for (const auto &token: tokens) {
        words.push_back(token);
    }

    return words;
    //добавить тупую нормализацию - для слов больше или равно чем 5 букв откилываем посление 2
}


// Структура для хранения информации о документе
struct Document {
    string url;
    string descr;
};

struct DocumentFreqData {
    DocumentFreqData() {
        url = "";
        len = 0;
        termFreqs = unordered_map<string, int>();
    }

    DocumentFreqData(string url, unsigned int len, const unordered_map<string, int> &termFreqs) : url(std::move(url)),
                                                                                                  len(len),
                                                                                                  termFreqs(
                                                                                                          termFreqs) {}

    string url;
    unsigned int len;
    unordered_map<string, int> termFreqs;
};

void readDir(const string &folderPath, vector<Document> &documents);

unordered_map<string, double> calcIdf(const unordered_map<string, int> &wordFreqs, int corpusSize);

void saveIdfs(unordered_map<string, double> &idfs);

void saveDocFreq(vector<DocumentFreqData> &termInDocFreqs, double avgDocLength);

unordered_map<string, double> readIdf();

double readTermInDocFreq(vector<DocumentFreqData> &termInDocFreqs);

// Функция для индексации документов в папке с использованием алгоритма BM25
double indexDocuments(const string &folderPath,
                      vector<DocumentFreqData> &termInDocFreqs,
                      unordered_map<string, double> &idfs) {

    // возращаем (сохранчем на диск) массив частот документов [{'Hello': 1, 'there': 1, 'good': 1, 'man!': 1}, {'It': 1, 'is': 1, 'quite': 1, 'windy': 1, 'in': 1, 'London': 1}, {'How': 1, 'is': 1, 'the': 1, 'weather': 1, 'today?': 1}]
    // и массив idf {'Hello': 0.5108256237659907, 'there': 0.5108256237659907, 'good': 0.5108256237659907, 'man!': 0.5108256237659907, 'It': 0.5108256237659907, 'is': 0.10946263366414084, 'quite': 0.5108256237659907, 'windy': 0.5108256237659907, 'in': 0.5108256237659907, 'London': 0.5108256237659907, 'How': 0.5108256237659907, 'the': 0.5108256237659907, 'weather': 0.5108256237659907, 'today?': 0.5108256237659907}
    unordered_map<string, double> bm25Scores;
    unordered_map<string, int> wordFreqs;
    vector<Document> documents;

    // Чтение документов из папки
    readDir(folderPath, documents);

    // Индексация документов
    int corpusSize = documents.size();
    unordered_map<string, double> avgDocLengths;
    int totalDocLength = 0;

    for (const auto &document: documents) {
        vector<string> words = tokenize(document.descr);
        unordered_map<string, int> termFreq;

        for (const string &word: words) {
            termFreq[word]++;
        }

        termInDocFreqs.emplace_back(document.url, words.size(), termFreq);

        for (const auto &pair: termFreq) {
            string term = pair.first;
            wordFreqs[term]++;
        }

        totalDocLength += words.size();
    }
    idfs = calcIdf(wordFreqs, corpusSize);

    double avgDocLength = static_cast<double>(totalDocLength) / corpusSize;
    return avgDocLength;
}

unordered_map<string, double> calcIdf(const unordered_map<string, int> &wordFreqs, int corpusSize) {
    unordered_map<string, double> idfs;
    // collect idf sum to calculate an average idf for epsilon value
    double idfSum = 0;

    // collect words with negative idf to set them a special epsilon value.
    // idf can be negative if word is contained in more than half of documents
    vector<string> negative_idfs;
    for (const auto &wordFreq: wordFreqs) {
        double idf = log(corpusSize - wordFreq.second + 0.5) - log(wordFreq.second + 0.5);
        idfs[wordFreq.first] = idf;
        idfSum += idf;
        if (idf < 0)
            negative_idfs.push_back(wordFreq.first);
    }
    auto averageIdf = idfSum / idfs.size();

    auto eps = EPSILON * averageIdf;
    for (const auto &word: negative_idfs) {
        idfs[word] = eps;
    }
    return idfs;
}


void readDir(const string &folderPath, vector<Document> &documents) {
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
}

// Функция для поиска документов по запросу с использованием индекса BM25
FixedPriorityQueue<string, TOP_COUNT> searchDocuments(unordered_map<string, double> &idfs,
                                                      vector<DocumentFreqData> &termInDocFreqs,
                                                      const string &query, double avgdl) {
    // считаем скоры, кладе в очередь с приоритетом вощвращаем топчик
    vector<string> queryTerms = tokenize(query);
    unordered_map<string, double> documentScores;
    FixedPriorityQueue<string, TOP_COUNT> topResult;
    for (const auto &docData: termInDocFreqs) {
        auto termDocFreq = docData.termFreqs;
        double score = 0;
        for (const string &term: queryTerms) {
            double qFreq = 0;
            if (auto search = termDocFreq.find(term); search != termDocFreq.end())
                qFreq = search->second;
            auto ctd = qFreq / (1 - B + B * docData.len / avgdl);

            double idf = 0;
            auto got = idfs.find(term);
            if (got != idfs.end())
                idf = got->second;
//                if (auto search = idfs.find(term); search != idfs.end())
//                    idf = search->second;
            score += idf * (qFreq * (K1 + 1) / (qFreq + K1 * (1 - B + B * docData.len / avgdl)));
        }

        topResult.push(pair(docData.url, score));
    }

    return topResult;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: ./bm25_util --index \"/path/to/dir\" --search \"search query\"" << endl;
        return 1;
    }

    string mode = argv[1];

    if (mode == "--index") {
        string folderPath = argv[2];
        vector<DocumentFreqData> termInDocFreqs;
        unordered_map<string, double> idfs;
        auto avgDocLength = indexDocuments(folderPath, termInDocFreqs, idfs);

        saveIdfs(idfs);

        saveDocFreq(termInDocFreqs, avgDocLength);

        cout << "Indexing complete. Result saved to " << IDFS_RESULT_PATH << ", " << DOC_FREQ_RESULT_PATH << endl;
    } else if (mode == "--search") {
        string query = argv[2];
        auto idfs = readIdf();
        vector<DocumentFreqData> termInDocFreqs;
        double avgDocLength = readTermInDocFreq(termInDocFreqs);
        cout << "Done" << endl;
        auto top = searchDocuments(idfs, termInDocFreqs, query, avgDocLength);

        stack<pair<string, double>> reverseStack;

        while (!top.empty()) {
            reverseStack.push(top.top());
            top.pop();
        }

        while (!reverseStack.empty()) {
            auto item = reverseStack.top();
            reverseStack.pop();
            std::cout << "Url: " << item.first << " with score " << item.second << std::endl;
        }
        while (!top.empty()) {
            auto item = top.top();
            cout << item.first << " - " << item.second << endl;
            top.pop();
        }
    } else {
        cout << "Invalid mode specified." << endl;
        cout << "Usage: ./bm25_util --index \"/path/to/dir\" --search \"search query\"" << endl;
        return 1;
    }

    return 0;
}

tuple<string, double> splitDocString(const string &input) {
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

double readTermInDocFreq(vector<DocumentFreqData> &termInDocFreqs) {

    ifstream file(DOC_FREQ_RESULT_PATH);
    string line;
    double avgdl = 0;

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
        cerr << "Unable to open file: " << DOC_FREQ_RESULT_PATH << endl;
    }

    return avgdl;
}

unordered_map<string, double> readIdf() {
    ifstream indexFile(IDFS_RESULT_PATH);
    unordered_map<string, double> idfs;
    string line;

    while (getline(indexFile, line)) {
        auto [url, idf] = splitDocString(line);
        idfs[url] = idf;
    }

    indexFile.close();
    return idfs;
}

void saveIdfs(unordered_map<string, double> &idfs) {
    ofstream indexFile(IDFS_RESULT_PATH);
    if (indexFile.is_open()) {
        for (const auto &pair: idfs) {
            indexFile << pair.first << "-" << pair.second << endl;
        }
        indexFile.close();
    } else {
        cerr << "Unable to open file: " << DOC_FREQ_RESULT_PATH << endl;
    }
}

void
saveDocFreq(vector<DocumentFreqData> &termInDocFreqs, double avgDocLength) {
    ofstream file(DOC_FREQ_RESULT_PATH);

    if (file.is_open()) {
        file << avgDocLength << endl;
        for (const auto &item: termInDocFreqs) {
            file << item.url << "-" << item.len << ":" << endl;
            for (const auto &innerPair: item.termFreqs) {
                file << "\t" << innerPair.first << ":" << innerPair.second << endl;
            }
            file << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file: " << DOC_FREQ_RESULT_PATH << endl;
    }
}