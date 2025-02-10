#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include "./circular_buffer.h"

template <typename T> class CircularQueue {
public:
    explicit CircularQueue(uint32_t capacity)
        : buffer(capacity), head(0), tail(0), count(0) {}

public:
    bool push(const T &element) {
        if (is_full()) { return false; }
        buffer[tail] = element;
        tail = buffer.get_next_index(tail);
        ++count;

        return true;
    }

    bool pop(T &out_element) {
        if (is_empty()) { return false; }
        out_element = buffer[head];
        head = buffer.get_next_index(head);
        --count;

        return true;
    }

    const T &front() const { return buffer[head]; }
    T &front() { return buffer[head]; }
    bool is_empty() const { return count == 0; }
    bool is_full() const { return count == capacity(); }
    uint32_t size() const { return count; }
    uint32_t capacity() const { return buffer.capacity(); }

    void clear () {
        head = 0;
        tail = 0;
        count = 0;
    }

private:
    CircularBuffer<T> buffer;
    uint32_t head;
    uint32_t tail;
    uint32_t count;
};

#endif // CIRCULAR_QUEUE_H
