#pragma once

#include <iostream>
#include <queue>
#include <array>
#include <utility>
#include <stack>

using namespace std;

template<typename T, size_t N>
class FixedPriorityQueue {
public:
    FixedPriorityQueue() = default;

    void push(const std::pair<T, double> &item) {
        if (queue.size() == N) {
            if (item.second > queue.top().second) {
                queue.pop();
                queue.push(item);
            }
        } else {
            queue.push(item);
        }
    }

    std::pair<T, double> top() const {
        return queue.top();
    }

    void pop() {
        queue.pop();
    }

    [[nodiscard]] bool empty() const {
        return queue.empty();
    }

    [[nodiscard]] size_t size() const {
        return queue.size();
    }


    void print() {
        stack <pair<string, double>> reverseStack;

        while (!this->empty()) {
            reverseStack.push(top());
            pop();
        }

        while (!reverseStack.empty()) {
            auto item = reverseStack.top();
            reverseStack.pop();
            cout << "Url: " << item.first << " with score " << item.second << endl;
        }
        while (!empty()) {
            auto item = top();
            cout << item.first << " - " << item.second << endl;
            pop();
        }
    }

private:

    struct Compare {
        bool operator()(const std::pair<T, double> &a, const std::pair<T, double> &b) const {
            return a.second > b.second; // Min-heap
        }
    };


    std::priority_queue<std::pair<T, double>, std::vector<std::pair<T, double>>, Compare> queue{};

};

