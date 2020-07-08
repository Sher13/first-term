#include <vector>
#include <cstdint>

struct cow_vector {
private:
    std::vector<uint32_t> data_;
    size_t counter;

public:

    cow_vector()
            : counter(1) {};

    cow_vector(size_t n)
            : counter(1) {
        data_ = std::vector<uint32_t>(n);
    };

    cow_vector(std::vector<uint32_t> const &b)
            : counter(1) {
        data_ = std::vector<uint32_t>(b);
    };

    ~cow_vector() = default;

    void inc() {
        counter++;
    }

    void dec() {
        counter--;
    }

    bool is_one() {
        return counter == 1;
    };

    std::vector<uint32_t> &data() {
        return data_;
    }
};