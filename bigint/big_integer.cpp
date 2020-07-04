#include <vector>
#include <algorithm>
#include <cmath>
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


std::pair<big_integer, big_integer> big_integer::div_little(big_integer a, big_integer b) {
    unsigned int b1 = b.digits[0];
    unsigned long long x = 0;
    big_integer rez;
    for(int i = a.digits.size()-1; i >= 0; i--) {
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

std::pair<big_integer, big_integer> big_integer::div_(big_integer a, big_integer b) {
    //  a < b
    if (flags(abs_(a), abs_(b)) == 1) {
        return std::make_pair(0, a);
    }
    if (b.digits.size() == 1) {
        return div_little(a, b); // I'm write it later....
    }
    unsigned int f = radix / (b.digits.back() + 1);
    a *= f;
    b *= f;
    a.digits.push_back(0);
    big_integer rez;
    size_t x = b.digits.size()-1;
    uint128_t b2 = (uint128_t)b.digits[x] * radix + (uint128_t)b.digits[x-1];
    bool a_sign = a.sign;
    bool b_sign = b.sign;
    size_t b_size = b.digits.size();
    b.sign = false;
    a.sign = false;
    b.digits.insert(b.digits.begin(), a.digits.size()-b_size-1, 0);
    while(b.digits.size() >= b_size) {
        x = a.digits.size()-1;
        uint128_t a3 = (uint128_t)a.digits[x] * radix * radix + (uint128_t)a.digits[x-1] * radix + (uint128_t)a.digits[x-2];
        unsigned int d = a3 / b2;
        unsigned int d1 = std::min(d, UINT32_MAX);
        big_integer c = b * d1;
        a.norm();
        if (flags(a, c) == 1) {
            d1--;
            c -= b;
        }
        rez.digits.push_back(d1);
        b.digits.erase(b.digits.begin());
        a -= c;
        if(a.digits.size() < b.digits.size()+1) {
            a.digits.insert(a.digits.end(), b.digits.size()-a.digits.size()+1, 0);
        }
    }
    if (a_sign != b_sign) {
        rez.sign = true;
    }
    reverse(rez.digits.begin(), rez.digits.end());
    rez.norm();
    a = div_little(a, f).first;
    return std::make_pair(rez, a);
}

big_integer::big_integer() {
    sign = false;
};

big_integer::big_integer(big_integer const &other) {
    for(size_t i = 0; i < other.digits.size(); i++)
        digits.push_back(other.digits[i]);
    sign = other.sign;
};

big_integer::big_integer(int a) {
    sign = false;
    if (a < 0)
        sign = true;
    long b = a;
    b = fabs(b);
    digits.push_back(b);
    (*this).norm();
};

big_integer::big_integer(unsigned int a) {
    sign = false;
    digits.push_back(a);
};

big_integer::big_integer(const std::string &str) {
    big_integer a;
    int st = 0;
    bool fl = false;
    if (str[0] == '-') {
        fl = true;
        st++;
    }
    for(size_t i = st; i < str.size(); i++) {
        a *= 10;
        a += (str[i] - '0');
    }
    a.sign = fl;
    a.norm();
    *this = a;
}

big_integer &big_integer::operator=(big_integer const &other) {
    big_integer res = big_integer(other);
    this->digits = res.digits;
    this->sign = res.sign;
    return *this;
};

big_integer &big_integer::operator+=(big_integer const &rhs) {
    this->digits.resize(std::max(this->digits.size(), rhs.digits.size()) + 1);
    big_integer a;
    a.digits.resize(this->digits.size());
    big_integer b(rhs);
    b.digits.resize(this->digits.size());
    if (this->sign != rhs.sign) {
        if (abs_(*this) < abs_(b)) {
            big_integer c(*this);
            *this = b;
            b = c;
        }
        a.sign = this->sign;
        for(size_t i = 0; i < a.digits.size() - 1; i++) {
            if (this->digits[i] >= b.digits[i])
                a.digits[i] += this->digits[i] - b.digits[i];
            else {
                long long x = radix - b.digits[i] + this->digits[i];
                b.digits[i+1]++;
                a.digits[i] = x;
            }
        }
        a.norm();
        *this = a;
        return *this;
    } else {
        a.digits.resize(a.digits.size() + 1);
        for(size_t i = 0; i < this->digits.size(); i++) {
            long long c = (long long) this->digits[i] + (long long) b.digits[i] + (long long) a.digits[i];
            a.digits[i] = c % radix;
            a.digits[i + 1] += c / radix;
        }
        a.sign = sign;
        a.norm();
        *this = a;
        return *this;
    }
};


big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this += (-rhs);
};

big_integer &big_integer::operator*=(big_integer const &rhs) {
    if (rhs.digits.empty()) {
        *this = rhs;
        return *this;
    }
    big_integer a;
    a.digits.resize(this->digits.size() + rhs.digits.size() + 2);
    if (this->sign != rhs.sign) {
        a.sign = true;
    } else {
        a.sign = false;
    }
    for(size_t i = 0; i < this->digits.size(); i++) {
        for(size_t j = 0; j < rhs.digits.size(); j++) {
            uint128_t x = (uint128_t) (this->digits[i]) * (uint128_t) (rhs.digits[j]) + (uint128_t)(a.digits[i+j]);
            a.digits[i + j] = x % radix;
            x = x / radix + a.digits[i + j + 1];
            a.digits[i + j + 1] = x % radix;
            x = x/radix + a.digits[i + j + 2];
            a.digits[i + j + 2] = x % radix;
        }
    }
    a.norm();
    *this = a;
    return *this;
};

big_integer &big_integer::operator/=(big_integer const &rhs) {
    auto rez = div_((*this), rhs);
    *this = rez.first;
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    auto rez = div_((*this), rhs);
    *this = rez.second;
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
    big_integer a(*this);
    a.bitToTwo();
    for(size_t i = 0; i < a.digits.size(); i++) {
        a.digits[i] = ~a.digits[i];
    }
    a.twoToBit();
    a.norm();
    return a;
}

big_integer &big_integer::operator++() {
    return *this += 1;
};

big_integer big_integer::operator++(int) {
    return *this += 1;
};

big_integer &big_integer::operator--() {
    return *this -= 1;
};

big_integer big_integer::operator--(int) {
    return *this -= 1;
};

std::string to_string(big_integer const &a) {
    std::string s;
    big_integer b(a);
    b.digits.push_back(0);
    b.sign = false;
    while(!b.digits.empty()) {
        auto x = b.div_(b, 10);
        b = x.first;
        if (x.second.digits.empty()) {
            x.second.digits.push_back(0);
        }
        s += (char) (x.second.digits[0] + '0');
    }
    while(s.size() != 1 && s.back() == '0') {
        s.pop_back();
    }
    reverse(s.begin(), s.end());
    if (a.sign) {
        s = '-' + s;
    }
    return s;
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
    return !(f == 0);
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