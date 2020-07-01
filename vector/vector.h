#ifndef VECTOR_H
#define VECTOR_H
#include <iostream>

template <typename T>
struct vector
{
    typedef T* iterator;
    typedef T const* const_iterator;

    vector(){
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    };                                   // O(1) nothrow
    vector(vector const &other)
            : data_(nullptr), size_(0), capacity_(0)
    {
        change_capacity(other.size_);
        copy(other.data_, data_, other.size_);
        size_ = other.size_;
    };                                 // O(N) strong
    vector& operator=(vector const& other) {
        vector <T> nw(other);
        swap(nw);
        return *this;
    };                                 // O(N) strong

    ~vector() {
        clear();
        operator delete(data_);
    };                              // O(N) nothrow

    T& operator[](size_t x) {
        return data_[x];
    };                              // O(1) nothrow
    T const& operator[](size_t x) const {
        return data_[x];
    };                              // O(1) nothrow

    T* data() {
        return data_;
    };                              // O(1) nothrow
    T const* data() const {
        return data_;
    };                              // O(1) nothrow
    size_t size() const {
        return size_;
    };                              // O(1) nothrow

    T& front() {
        return data_[0];
    };                              // O(1) nothrow
    T const& front() const {
        return data_[0];
    };                              // O(1) nothrow

    T& back() {
        return data_[size_-1];
    };                              // O(1) nothrow
    T const& back() const {
        return data_[size_-1];
    };                              // O(1) nothrow
    void push_back(T const& val) {
        T x = val;
        if (size_ == capacity_)
            change_capacity(2 * (size_ + 1));
        new(data_ + size_) T(x);
        size_++;
    };                              // O(1)* strong
    void pop_back() {
        data_[size_ - 1].~T();
        size_--;
    };                              // O(1) nothrow

    bool empty() const {
        return size_ == 0;
    };                              // O(1) nothrow

    size_t capacity() const {
        return capacity_;
    };                              // O(1) nothrow
    void reserve(size_t new_cap) {
        if (new_cap > capacity_)
            change_capacity(new_cap);
    };                              // O(N) strong
    void shrink_to_fit() {
        change_capacity(size_);
    };                              // O(N) strong

    void clear() {
        while(size_ != 0)
            pop_back();
    };                              // O(N) nothrow

    void swap(vector& other) {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    };                              // O(1) nothrow

    iterator begin() {
        return data_;
    };                              // O(1) nothrow
    iterator end() {
        return data_ + size_;
    };                              // O(1) nothrow

    const_iterator begin() const {
        return data_;
    };                              // O(1) nothrow
    const_iterator end() const {
        return data_ + size_;
    };                              // O(1) nothrow

    iterator insert(iterator pos, T const& val) {
        size_t ind = pos - data_;
        if (size_ == capacity_) {
            change_capacity(2 * (size_ + 1));
        }
        new(data_+size_) T(val);
        for (size_t i = size_; i > ind; i--) {
            data_[i] = data_[i-1];
        }
        data_[ind] = val;
        size_++;
        return ind+data_;
    };                              // O(N) weak
    iterator insert(const_iterator pos, T const& val) {
        return insert(pos-data_+data_, val);
    };                              // O(N) weak

    iterator erase(iterator pos) {
        return erase(pos, pos + 1);
    };                              // O(N) weak
    iterator erase(const_iterator pos) {
        return erase(pos-data_+data_);
    };                              // O(N) weak

    iterator erase(iterator first, iterator last) {
        size_t ind = first-data_;
        size_t n = last - first;
        for (size_t i = ind; i < size_-n; i++) {
            data_[i] = data_[i+n];
        }
        while(n != 0) {
            pop_back();
            n--;
        }
        return ind+data_;
    };                              // O(N) weak
    iterator erase(const_iterator first, const_iterator last) {
        return erase(first-data_+data_, last-data_+data_);
    };                              // O(N) weak

private:
    void change_capacity(size_t new_cap) {
        if (new_cap == capacity_)
            return;
        T* ptr;
        if (new_cap == 0) {
            ptr = nullptr;
        } else {
            ptr = static_cast<T *>(operator new(new_cap * sizeof(T)));
        }
        copy(data_, ptr, size_);
        size_t sz = size_;
        clear();
        operator delete(data_);
        size_ = sz;
        data_ = ptr;
        capacity_ = new_cap;
    };
    void copy(T* from, T* to, size_t n) {
        size_t size = 0;
        try {
            for(size_t i = 0; i < n; i++) {
                T x = from[i];
                new(to+i) T(x);
                size++;
            }
        } catch(...) {
            while(size) {
                to[size - 1].~T();
                size--;
            }
            operator delete(to);
            throw;
        }
    }
private:
    T* data_;
    size_t size_;
    size_t capacity_;
};

#endif // VECTOR_H