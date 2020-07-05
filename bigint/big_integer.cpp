#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "big_integer.h"

typedef unsigned __int128 uint128_t;

class vector;

int flags(big_integer const &a, big_integer const &b) {
    // '<' = 1;  '>' = -1; '==' 0;
    if (a.sign && !b.sign)
        return 1;
    if (b.sign && !a.sign)
        return -1;
    int ans = 0;
    if (a.digits.size() < b.digits.size()) {
        ans = 1;
    } else if (a.digits.size() > b.digits.size()) {
        ans = -1;
    } else {
        for(int i = a.digits.size() - 1; i >= 0; i--) {
            if (a.digits[i] < b.digits[i]) {
                ans = 1;
                break;
            }
            if (a.digits[i] > b.digits[i]) {
                ans = -1;
                break;
            }
        }
    }
    if (ans == 0)
        return ans;
    if (a.sign && b.sign)
        return 0 - ans;
    return ans;
}

void big_integer::norm() {
    while(this->digits.size() && this->digits[this->digits.size() - 1] == 0)
        this->digits.pop_back();
    if (this->digits.size() == 0)
        this->sign = false;
}

big_integer big_integer::abs_(big_integer a) {
    a.sign = false;
    return a;
}

// переводит из бита под знак в дополнение до двух
void big_integer::bitToTwo() {
    if (!sign)
        return;
    for(size_t i = 0; i < digits.size(); i++) {
        digits[i] = ~digits[i];
    }
    sign = false;
    (*this)++;
}

// переводит из дополнения до двух в бит под знак
void big_integer::twoToBit() {
    sign = false;
    if ((digits.back() & 2147483648) != 2147483648)
        return;
    (*this)--;
    for(size_t i = 0; i < digits.size(); i++) {
        digits[i] = ~digits[i];
    }
    sign = true;
}


std::pair<big_integer, big_integer> big_integer::div_little(big_integer a, big_integer const &b) {
    unsigned int b1 = b.digits[0];
    unsigned long long x = 0;
    big_integer rez;
    for(int i = a.digits.size() - 1; i >= 0; i--) {
        x = x * radix + a.digits[i];
        rez.digits.push_back(x / b1);
        x %= b1;
    }
    if (a.sign != b.sign) {
        rez.sign = true;
    }
    reverse(rez.digits.begin(), rez.digits.end());
    rez.norm();
    unsigned int x1 = x;
    big_integer rem(x1);
    rem.sign = a.sign;
    rem.norm();
    return std::make_pair(rez, rem);
}

void big_integer::mul_little(big_integer const &b) {
    unsigned long long b1 = b.digits[0];
    digits.push_back(0);
    unsigned long long carry = 0;
    for(size_t i = 0; i < digits.size() - 1; i++) {
        unsigned long long x = (unsigned long long) digits[i] * b1 + carry;
        digits[i] = x % radix;
        carry = x / radix;
    }
    digits.back() = carry;
    sign = sign ^ b.sign;
    (*this).norm();
}

std::pair<big_integer, big_integer> big_integer::div_(big_integer const& aa, big_integer const& bb) {
    big_integer a(aa);
    big_integer b(bb);
    a.sign = false;
    b.sign = false;
    if (a < b) {
        a.sign = aa.sign;
        return std::make_pair(0, a);
    }
    if (b.digits.size() == 1) {
        b.sign = bb.sign;
        a.sign = aa.sign;
        return div_little(a, b);
    }
    unsigned int f = radix / (b.digits.back() + 1);
    a.mul_little(f);
    b.mul_little(f);
    a.digits.push_back(0);
    uint128_t b2 = b.digits.back() * radix + b.digits[b.digits.size() - 2];
    big_integer rez;
    size_t b_size = b.digits.size();
    b.digits.insert(b.digits.begin(), a.digits.size() - b_size - 1, 0);
    big_integer c;
    while(b.digits.size() >= b_size) {
        size_t x = a.digits.size() - 1;
        uint128_t a3 = (uint128_t) a.digits[x] * radix * radix + (uint128_t) a.digits[x - 1] * radix +
                       (uint128_t) a.digits[x - 2];
        unsigned int d = a3 / b2;
        unsigned int q = std::min((unsigned int) d, UINT32_MAX);
        c = b;
        c.mul_little(q);
        a.norm();
        if (a < c) {
            q--;
            c -= b;
        }
        rez.digits.push_back(q);
        b.digits.erase(b.digits.begin());
        a -= c;
        if (a.digits.size() < b.digits.size() + 1) {
            a.digits.insert(a.digits.end(), b.digits.size() - a.digits.size() + 1, 0);
        }
    }
    rez.sign = aa.sign ^ bb.sign;
    reverse(rez.digits.begin(), rez.digits.end());
    rez.norm();
    a.sign = aa.sign;
    a = div_little(a, f).first;
    return std::make_pair(rez, a);
}

big_integer::big_integer()
        : sign(false) {
};

big_integer::big_integer(big_integer const &other)
        : sign(other.sign), digits(other.digits) {
};

big_integer::big_integer(int a) {
    sign = false;
    if (a < 0)
        sign = true;
    long b = a;
    b = std::abs(b);
    digits.push_back(b);
    (*this).norm();
};

big_integer::big_integer(unsigned int a) {
    sign = false;
    digits.push_back(a);
};

big_integer::big_integer(const std::string &str)
        : big_integer() {
    int st = 0;
    bool fl = false;
    if (str[0] == '-') {
        fl = true;
        st++;
    }
    for(size_t i = st; i < str.size(); i++) {
        size_t j = i;
        unsigned int x = 0;
        unsigned int st10 = 1;
        while(j - i < 9 && j < str.size()) {
            x = x * 10 + (str[j] - '0');
            j++;
            st10 *= 10;
        }
        i = j - 1;
        (*this).mul_little(st10);
        (*this) += x;
    }
    sign = fl;
    (*this).norm();
}

big_integer &big_integer::operator=(big_integer const &other) {
    big_integer res(other);
    swap((*this), res);
    return *this;
};

big_integer &big_integer::operator+=(big_integer const &rhs) {
    size_t mx = std::max(rhs.digits.size(), digits.size()) + 1;
    digits.resize(mx);
    big_integer b(rhs);
    b.digits.resize(mx);
    if (sign != b.sign) {
        bool a_sign = sign;
        bool b_sign = b.sign;
        sign = false;
        b.sign = false;
        if (*this < b) {
            swap(*this, b);
            std::swap(a_sign, b_sign);
        }
        long long borrow = 0;
        for(size_t i = 0; i < mx; i++) {
            if ((long long) digits[i] >= (long long) b.digits[i] + borrow) {
                digits[i] = digits[i] - b.digits[i] - borrow;
                borrow = 0;
            } else {
                unsigned long long x = digits[i] + radix - b.digits[i] - borrow;
                digits[i] = x;
                borrow = 1;
            }
        }
        sign = a_sign;
    } else {
        long long carry = 0;
        for(size_t i = 0; i < mx; i++) {
            long long x = (long long) digits[i] + (long long) b.digits[i] + carry;
            digits[i] = x % radix;
            carry = x / radix;
        }
    }
    (*this).norm();
    return (*this);
};


big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this += (-rhs);
};

big_integer &big_integer::operator*=(big_integer const &rhs) {
    if (rhs.digits.empty()) {
        *this = rhs;
        return *this;
    }
    if (rhs.digits.size() == 1) {
        (*this).mul_little(rhs);
        return *this;
    }
    big_integer a;
    a.digits.resize(this->digits.size() + rhs.digits.size() + 2);
    for(size_t i = 0; i < digits.size(); i++) {
        big_integer b(rhs);
        b.sign = false;
        b.mul_little(digits[i]);
        b.digits.insert(b.digits.begin(), i, 0);
        a += b;
    }
    a.sign = sign ^ rhs.sign;
    a.norm();
    swap(a, *this);
    return *this;
};

big_integer &big_integer::operator/=(big_integer const &rhs) {
    auto rez = div_((*this), rhs);
    swap(rez.first, *this);
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    auto rez = div_((*this), rhs);
    swap(rez.second, *this);
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    big_integer b(rhs);
    size_t mx = std::max(digits.size(), b.digits.size()) + 1;
    digits.resize(mx);
    b.digits.resize(mx);
    (*this).bitToTwo();
    b.bitToTwo();
    for(size_t i = 0; i < mx; i++) {
        digits[i] = digits[i] & b.digits[i];
    }
    (*this).twoToBit();
    (*this).norm();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    big_integer b(rhs);
    size_t mx = std::max(digits.size(), b.digits.size()) + 1;
    digits.resize(mx);
    b.digits.resize(mx);
    (*this).bitToTwo();
    b.bitToTwo();
    for(size_t i = 0; i < mx; i++) {
        digits[i] = digits[i] | b.digits[i];
    }
    (*this).twoToBit();
    (*this).norm();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    big_integer b(rhs);
    size_t mx = std::max(digits.size(), b.digits.size()) + 1;
    digits.resize(mx);
    b.digits.resize(mx);
    (*this).bitToTwo();
    b.bitToTwo();
    for(size_t i = 0; i < mx; i++) {
        digits[i] = digits[i] ^ b.digits[i];
    }
    (*this).twoToBit();
    (*this).norm();
    return *this;
}

// mul
big_integer &big_integer::operator<<=(int rhs) {
    int kl = rhs / 32;
    rhs %= 32;
    digits.insert(digits.begin(), kl, 0);
    unsigned int e = 1;
    e <<= rhs;
    return *this *= e;
}

// div
big_integer &big_integer::operator>>=(int rhs) {
    int kl = rhs / 32;
    rhs %= 32;
    digits.erase(digits.begin(), digits.begin() + kl);
    unsigned int e = 1;
    e <<= rhs;
    (*this) /= e;
    if (sign && (*this) % e != 0) {
        (*this)--;
    }
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
};

big_integer big_integer::operator-() const {
    big_integer a(*this);
    a.sign = !a.sign;
    a.norm();
    return a;
};

big_integer big_integer::operator~() const {
    return -(*this)-1;
}

big_integer &big_integer::operator++() {
    return *this += 1;
};

big_integer big_integer::operator++(int) {
    big_integer a(*this);
    *this += 1;
    return a;
};

big_integer &big_integer::operator--() {
    return *this -= 1;
};

big_integer big_integer::operator--(int) {
    big_integer a(*this);
    *this -= 1;
    return a;
};

std::string to_string(big_integer const &a) {
    std::string s;
    big_integer b(a);
    b.digits.push_back(0);
    b.sign = false;
    while(!b.digits.empty()) {
        auto x = b.div_little(b, 1000000000);
        b = x.first;
        if (x.second.digits.empty()) {
            x.second.digits.push_back(0);
        }
        std::string y = std::to_string(x.second.digits[0]);
        while(y.size() != 9) {
            y = "0" + y;
        }
        s = y + s;
    }
    while(s.size() != 1 && s[0] == '0') {
        s.erase(s.begin());
    }
    if (a.sign) {
        s = '-' + s;
    }
    return s;
}

void big_integer::swap(big_integer &a, big_integer &b) {
    using std::swap;
    swap(a.sign, b.sign);
    swap(a.digits, b.digits);
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator<(big_integer const &a, big_integer const &b) {
    int f = flags(a, b);
    return (f == 1);
}

bool operator==(big_integer const &a, big_integer const &b) {
    int f = flags(a, b);
    return (f == 0);
}

bool operator>(big_integer const &a, big_integer const &b) {
    int f = flags(a, b);
    return (f == -1);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    int f = flags(a, b);
    return f != 0;
};

bool operator<=(big_integer const &a, big_integer const &b) {
    int f = flags(a, b);
    return (f >= 0);
};

bool operator>=(big_integer const &a, big_integer const &b) {
    int f = flags(a, b);
    return (f <= 0);
}

std::ostream &operator<<(std::ostream &cout_, big_integer const &a) {
    cout_ << to_string(a);
    return cout_;
}
