////
//// Created by galya on 29.06.23.
////
//
//#include "fixew_priority_queue.h"
//#include <iostream>
//#include <queue>
//#include <array>
//#include <utility> // For std::pair
//
//template<typename T, size_t N>
//class FixedPriorityQueue {
//public:
//    // Constructor
//    FixedPriorityQueue() {}
//
//    // Function to add an element to the queue
//    void push(const std::pair<T, int>& item) {
//        if (queue.size() == N) {
//            if (item.second > queue.top().second) {
//                queue.pop();
//                queue.push(item);
//            }
//        } else {
//            queue.push(item);
//        }
//    }
//
//    // Function to get the top element from the queue
//    std::pair<T, int> top() const {
//        return queue.top();
//    }
//
//    // Function to check if the queue is empty
//    bool empty() const {
//        return queue.empty();
//    }
//
//    // Function to get the size of the queue
//    size_t size() const {
//        return queue.size();
//    }
//
//private:
//    std::priority_queue<std::pair<T, int>, std::vector<std::pair<T, int>>, Compare> queue;
//
//    struct Compare {
//        bool operator()(const std::pair<T, int>& a, const std::pair<T, int>& b) const {
//            return a.second > b.second; // Min-heap
//        }
//    };
//};
//
