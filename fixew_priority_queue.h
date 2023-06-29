//
// Created by galya on 29.06.23.
//

#include <iostream>
#include <queue>
#include <array>
#include <utility> // For std::pair

template<typename T, size_t N>
class FixedPriorityQueue {
public:
    // Constructor
    FixedPriorityQueue() = default;

    // Function to add an element to the queue
    void push(const std::pair<T, double>& item) {
        if (queue.size() == N) {
            if (item.second > queue.top().second) {
                queue.pop();
                queue.push(item);
            }
        } else {
            queue.push(item);
        }
    }
    // Function to get the top element from the queue
    std::pair<T, double> top() const {
        return queue.top();
    }

    void pop() {
        queue.pop();
   }

    // Function to check if the queue is empty
    [[nodiscard]] bool empty() const {
        return queue.empty();
    }

    // Function to get the size of the queue
    [[nodiscard]] size_t size() const {
        return queue.size();
    }



private:

    struct Compare {
        bool operator()(const std::pair<T, double>& a, const std::pair<T, double>& b) const {
            return a.second > b.second; // Min-heap
        }
    };


    std::priority_queue<std::pair<T, double>, std::vector<std::pair<T, double>>, Compare> queue{};

};

