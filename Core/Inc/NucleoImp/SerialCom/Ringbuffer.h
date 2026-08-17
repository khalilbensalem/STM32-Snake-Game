#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <cstdint>
#include <cstddef>

namespace ELE3312 {

class RingBuffer {
public:
    RingBuffer(uint8_t* buffer, size_t size)
        : buffer(buffer), size(size), head(0), tail(0) {}

    bool push(uint8_t byte) {
        size_t next = (head + 1) % size;
        if (next == tail) return false; // Buffer plein
        buffer[head] = byte;
        head = next;
        return true;
    }

    bool pop(uint8_t& byte) {
        if (head == tail) return false; // Buffer vide
        byte = buffer[tail];
        tail = (tail + 1) % size;
        return true;
    }

    bool isEmpty() const { return head == tail; }

private:
    uint8_t* buffer;
    size_t size;
    volatile size_t head;
    volatile size_t tail;
};

} // namespace ELE3312

#endif
