// Module C: Queue

/*
- Store pending URLs extracted from Module A.
- Prioritize URLs based on importance & freshness.
- Avoid re-crawling the same URLs too often (use a Bloom filter or hash table).

- Priority Queue: std::priority_queue (can use min-heap for URL prioritization).
- Deduplication: Bloom filters (libbf or boost::dynamic_bitset).
- Message Queue (Optional, for distributed crawling): Apache Kafka or RabbitMQ

*/

#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

using namespace std;

template <typename T>
class Sec_Queue
{
private:
    std::queue<T> sc_queue;
    std::mutex mtx;
    condition_variable cv;

public:
    Sec_Queue(/* args */) = default;
    ~Sec_Queue() = default;

    void push(T value){ // is this safe
        lock_guard<mutex> lock(mtx);
        sc_queue.push(value);
        cv.notify_one();
    }

    bool pop(T& value){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !sc_queue.empty(); });  
        value = sc_queue.front();
        sc_queue.pop();
        return true;
    }

    bool empty(){
        lock_guard<mutex> lock(mtx);
        return sc_queue.empty();
    }

    std::optional<T> peek() {
        std::lock_guard<std::mutex> lock(mtx);
        if (sc_queue.empty()) return std::nullopt;  // ✅ Return empty state instead of throwing
        return sc_queue.front();
    }
};








#endif