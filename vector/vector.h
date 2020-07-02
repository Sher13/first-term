#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm>
#include <cstdio>

template<typename T>
struct vector {
    typedef T* iterator;
    typedef T const* const_iterator;

    vector()
            : data_(nullptr), size_(0), capacity_(0) {};

    vector(vector const &other)
            : data_(nullptr), size_(0), capacity_(0) {
        if (other.size_ == 0)
            return;
        data_ = create_copy(other.data_, other.size_, other.size_);
        capacity_ = other.size_;
        size_ = other.size_;
    };

    vector &operator=(vector const &other) {
        vector nw(other);
        swap(nw);
        return *this;
    };

    ~vector() {
        clear();
        operator delete(data_);
    };

    T &operator[](size_t x) {
        return data_[x];
    };

    T const &operator[](size_t x) const {
        return data_[x];
    };

    T* data() {
        return data_;
    };

    T const* data() const {
        return data_;
    };

    size_t size() const {
        return size_;
    };

    T &front() {
        return data_[0];
    };

    T const &front() const {
        return data_[0];
    };

    T &back() {
        return data_[size_ - 1];
    };

    T const &back() const {
        return data_[size_ - 1];
    };

    void push_back(T const &val) {
        if (size_ == capacity_) {
            T x = val;
            change_capacity(size_ == 0 ? 1 : size_ * 2);
            new(data_ + size_) T(x);
            size_++;
            return;
        }
        new(data_ + size_) T(val);
        size_++;
    };

    void pop_back() {
        data_[size_ - 1].~T();
        size_--;
    };

    bool empty() const {
        return size_ == 0;
    };

    size_t capacity() const {
        return capacity_;
    };

    void reserve(size_t new_cap) {
        if (new_cap > capacity_) {
            change_capacity(new_cap);
        }
    };

    void shrink_to_fit() {
        if (size_ != capacity_) {
            change_capacity(size_);
        }
    };

    void clear() {
        delete_(size_);
        size_ = 0;
    };

    void swap(vector &other) {
        using std::swap;
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    };

    iterator begin() {
        return data_;
    };

    iterator end() {
        return data_ + size_;
    };

    const_iterator begin() const {
        return data_;
    };

    const_iterator end() const {
        return data_ + size_;
    };

    iterator insert(iterator pos, T const &val) {
        ptrdiff_t ind = pos - begin();
        push_back(val);
        for (size_t i = size_ - 1; i > ind; i--) {
            std::swap(data_[i], data_[i - 1]);
        }
        return ind + data_;
    };

    iterator insert(const_iterator pos, T const &val) {
        return insert(pos - begin() + begin(), val);
    };

    iterator erase(iterator pos) {
        return erase(pos, pos + 1);
    };

    iterator erase(const_iterator pos) {
        return erase(pos - begin() + begin());
    };

    iterator erase(iterator first, iterator last) {
        ptrdiff_t ind = first - begin();
        ptrdiff_t n = last - first;
        for (size_t i = ind; i < size_ - n; i++) {
            std::swap(data_[i], data_[i + n]);
        }
        delete_(n);
        size_ -= n;
        return ind + data_;
    };

    iterator erase(const_iterator first, const_iterator last) {
        return erase(first - begin() + begin(), last - begin() + begin());
    };

private:
    void change_capacity(size_t new_cap) {
        T* ptr = create_copy(data_, size_, new_cap);
        delete_(size_);
        operator delete(data_);
        data_ = ptr;
        capacity_ = new_cap;
    };

// pred: new_cap >= new_size
    T* create_copy(T* from, size_t new_size, size_t new_cap) {
        T* to;
        if (new_cap == 0) {
            return nullptr;
        } else {
            to = static_cast<T*>(operator new(new_cap * sizeof(T)));
        }
        size_t size = 0;
        try {
            for (size_t i = 0; i < new_size; i++) {
                T x = from[i];
                new(to + i) T(x);
                size++;
            }
        } catch (...) {
            while (size) {
                to[size - 1].~T();
                size--;
            }
            operator delete(to);
            throw;
        }
        return to;
    }

    void delete_(size_t n) {
        for (size_t i = 0; i < n; i++) {
            data_[size_ - i - 1].~T();
        }
    }

private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

#endif // VECTOR_H
