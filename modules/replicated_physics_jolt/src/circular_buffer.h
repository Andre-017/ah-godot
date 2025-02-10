#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <core/math/math_funcs.h> // For ceil_power_of_two
#include <vector>
#include <cstdint>

template <typename ElementType>
class CircularBuffer {
public:
    /**
     * Constructor: Initializes a circular buffer with the specified capacity.
     * The capacity is rounded up to the next power of two for efficient indexing.
     *
     * @param capacity The number of elements the buffer can store.
     */
    explicit CircularBuffer(uint32_t capacity) {
        ERR_FAIL_COND_MSG(capacity == 0, "Capacity must be greater than 0.");
        capacity = next_power_of_2(capacity);
        elements.resize(capacity);
        index_mask = capacity - 1;
    }

    /**
     * Constructor: Initializes a circular buffer with the specified capacity and initial value.
     * The capacity is rounded up to the next power of two for efficient indexing.
     *
     * @param capacity The number of elements the buffer can store.
     * @param initial_value The initial value for all elements in the buffer.
     */
    CircularBuffer(uint32_t capacity, const ElementType &initial_value) {
        ERR_FAIL_COND_MSG(capacity == 0, "Capacity must be greater than 0.");
        capacity = Math::ceil_power_of_two(capacity); // Round up to the next power of 2
        elements.resize(capacity, initial_value);
        index_mask = capacity - 1;
    }

    /**
     * Access operator: Returns the mutable element at the specified index.
     *
     * @param index The index of the element to return.
     * @return Reference to the element at the specified index.
     */
    ElementType &operator[](uint32_t index) {
        return elements[index & index_mask];
    }

    /**
     * Access operator (const): Returns the immutable element at the specified index.
     *
     * @param index The index of the element to return.
     * @return Const reference to the element at the specified index.
     */
    const ElementType &operator[](uint32_t index) const {
        return elements[index & index_mask];
    }

    /**
     * Returns the number of elements that the buffer can hold.
     *
     * @return The buffer's capacity.
     */
    uint32_t capacity() const {
        return elements.size();
    }

    /**
     * Calculates the next index in the buffer after the given index.
     *
     * @param current_index The current index.
     * @return The next index.
     */
    uint32_t get_next_index(uint32_t current_index) const {
        return (current_index + 1) & index_mask;
    }

    /**
     * Calculates the previous index in the buffer before the given index.
     *
     * @param current_index The current index.
     * @return The previous index.
     */
    uint32_t get_previous_index(uint32_t current_index) const {
        return (current_index - 1) & index_mask;
    }

private:
    uint32_t index_mask;                 // Mask for efficient indexing
    std::vector<ElementType> elements;  // Underlying storage
};

#endif // CIRCULAR_BUFFER_H
