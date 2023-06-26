//
// Created by galya on 25.06.23.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <sstream>
#include <iterator>
#include <boost/tokenizer.hpp>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;
using tokenizer = boost::tokenizer<boost::char_separator<char>>;

// переписать indexDocuments по аналогии с _initialize + _calc_idf
// переписать сохранение и считывание с диска, не забываем переменны магические числа (постоянные хардкодим)

//Добавить тупую нормализацию

// переписватьподсчет скоров как get_scores, выдать топчик с помощью очереди с приоритетом
// Функция для разделения текста на отдельные слова с использованием Boost tokenizer
vector<string> tokenize(const string& text) {
    vector<string> words;
    boost::char_separator<char> separator(" \t\n\r\f.,;:\"\'()[]{}<>!?-");
    tokenizer tokens(text, separator);

    for (const auto& token : tokens) {
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

// Функция для индексации документов в папке с использованием алгоритма BM25
unordered_map<string, double> indexDocuments(const string& folderPath) {

  // возращаем (сохранчем на диск) массив частот документов [{'Hello': 1, 'there': 1, 'good': 1, 'man!': 1}, {'It': 1, 'is': 1, 'quite': 1, 'windy': 1, 'in': 1, 'London': 1}, {'How': 1, 'is': 1, 'the': 1, 'weather': 1, 'today?': 1}]
   // и массив idf {'Hello': 0.5108256237659907, 'there': 0.5108256237659907, 'good': 0.5108256237659907, 'man!': 0.5108256237659907, 'It': 0.5108256237659907, 'is': 0.10946263366414084, 'quite': 0.5108256237659907, 'windy': 0.5108256237659907, 'in': 0.5108256237659907, 'London': 0.5108256237659907, 'How': 0.5108256237659907, 'the': 0.5108256237659907, 'weather': 0.5108256237659907, 'today?': 0.5108256237659907}
    unordered_map<string, double> bm25Scores;
    vector<Document> documents;

    // Чтение документов из папки
    for (const auto& entry : filesystem::directory_iterator(folderPath)) {
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

    // Индексация документов
    int N = documents.size();
    unordered_map<string, int> documentFreqs;
    unordered_map<string, unordered_map<string, int>> termFreqs;
    unordered_map<string, double> avgDocLengths;
    int totalDocLength = 0;

    for (const auto& document : documents) {
        vector<string> words = tokenize(document.descr);
        unordered_map<string, int> termFreq;

        for (const string& word : words) {
            termFreq[word]++;
        }

        termFreqs[document.url] = termFreq;

        for (const auto& pair : termFreq) {
            string term = pair.first;
            documentFreqs[term]++;
        }

        totalDocLength += words.size();
    }

    double avgDocLength = static_cast<double>(totalDocLength) / N;

    for (const auto& pair : termFreqs) {
        string url = pair.first;
        unordered_map<string, int> termFreq = pair.second;
        double docLength = termFreq.size();
        avgDocLengths[url] = docLength / avgDocLength;

        for (const auto& termFreqPair : termFreq) {
            string term = termFreqPair.first;
            int freq = termFreqPair.second;
            double tf = freq / docLength;
            double idf = log2(N / documentFreqs[term]);
            bm25Scores[url] += tf * idf;
        }
    }

    return bm25Scores;
}

// Функция для поиска документов по запросу с использованием индекса BM25
vector<pair<string, double>> searchDocuments(const unordered_map<string, double>& bm25Scores, const string& query) {
    // считаем скоры, кладе в очередь с приоритетом вощвращаем топчик
    vector<string> queryTerms = tokenize(query);
    unordered_map<string, double> documentScores;

    for (const auto& pair : bm25Scores) {
        string url = pair.first;
        double score = pair.second;

        for (const string& term : queryTerms) {
            if (term.length() > 1 && url.find(term) != string::npos) {
                documentScores[url] += score;
            }
        }
    }

    vector<pair<string, double>> sortedScores(documentScores.begin(), documentScores.end());
    sort(sortedScores.begin(), sortedScores.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second > rhs.second;
    });

    return sortedScores;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: ./bm25_util --index \"/path/to/dir\" --search \"search query\"" << endl;
        return 1;
    }

    string mode = argv[1];

    if (mode == "--index") {
        string folderPath = argv[2];
        unordered_map<string, double> bm25Scores = indexDocuments(folderPath);

        ofstream indexFile("bm_25_res.txt");
        for (const auto& pair : bm25Scores) {
            indexFile << pair.first << " - " << pair.second << endl;
        }

        indexFile.close();

        cout << "Indexing complete. Result saved to bm_25_res.txt" << endl;
    } else if (mode == "--search") {
        string query = argv[2];
        ifstream indexFile("bm_25_res.txt");
        unordered_map<string, double> bm25Scores;
        string line;

        while (getline(indexFile, line)) {
            istringstream iss(line);
            string url;
            double score;
            iss >> url >> score;
            bm25Scores[url] = score;
        }

        indexFile.close();

        vector<pair<string, double>> searchResults = searchDocuments(bm25Scores, query);

        int numResults = min(static_cast<int>(searchResults.size()), 5);

        for (int i = 0; i < numResults; i++) {
            cout << searchResults[i].first << " - " << searchResults[i].second << endl;
        }
    } else {
        cout << "Invalid mode specified." << endl;
        cout << "Usage: ./bm25_util --index \"/path/to/dir\" --search \"search query\"" << endl;
        return 1;
    }

    return 0;
}
