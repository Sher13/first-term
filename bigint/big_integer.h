#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <cstddef>
#include <string>
#include <vector>

struct big_integer
{
private:

    std::vector <unsigned int> digits;
    bool sign;
    unsigned long long radix = 4294967296; // 2^32

public:
    big_integer();                                          // ok
    big_integer(big_integer const& other);                  // ok
    big_integer(int a);                                     // ok
    big_integer(unsigned int a);                            // just another one constructor ^^
    explicit big_integer(std::string const& str);           // ok

    big_integer& operator=(big_integer const& other);       // ok

    big_integer& operator+=(big_integer const& rhs);        // ok
    big_integer& operator-=(big_integer const& rhs);        // ok
    big_integer& operator*=(big_integer const& rhs);        // ok
    big_integer& operator/=(big_integer const& rhs);        // ok
    big_integer& operator%=(big_integer const& rhs);        // ok

    big_integer& operator&=(big_integer const& rhs);        // ok
    big_integer& operator|=(big_integer const& rhs);        // ok
    big_integer& operator^=(big_integer const& rhs);        // ok

    big_integer& operator<<=(int rhs);                      // ok
    big_integer& operator>>=(int rhs);                      // ok

    big_integer operator+() const;                          // ok
    big_integer operator-() const;                          // ok
    big_integer operator~() const;                          // ok

    big_integer& operator++();                              // ok
    big_integer operator++(int);                            // ok

    big_integer& operator--();                              // ok
    big_integer operator--(int);                            // ok

    friend std::string to_string(big_integer const& a);     // ok

private:
    friend int flags(big_integer const& a, big_integer const& b);
    void norm();
    void bitToTwo();
    void twoToBit();
    unsigned int div_little(unsigned int b);
    void mul_little(big_integer const& b);
    std::pair <big_integer, big_integer> div_(big_integer const& a, big_integer const& b);
    void swap(big_integer& a, big_integer& b);
};

big_integer operator+(big_integer a, big_integer const& b);     // ok
big_integer operator-(big_integer a, big_integer const& b);     // ok
big_integer operator*(big_integer a, big_integer const& b);     // ok
big_integer operator/(big_integer a, big_integer const& b);     // ok
big_integer operator%(big_integer a, big_integer const& b);     // ok

big_integer operator&(big_integer a, big_integer const& b);     // ok
big_integer operator|(big_integer a, big_integer const& b);     // ok
big_integer operator^(big_integer a, big_integer const& b);     // ok

big_integer operator<<(big_integer a, int b);                   // ok
big_integer operator>>(big_integer a, int b);                   // ok

bool operator==(big_integer const& a, big_integer const& b);    // ok
bool operator!=(big_integer const& a, big_integer const& b);    // ok
bool operator<(big_integer const& a, big_integer const& b);     // ok
bool operator>(big_integer const& a, big_integer const& b);     // ok
bool operator<=(big_integer const& a, big_integer const& b);    // ok
bool operator>=(big_integer const& a, big_integer const& b);    // ok

std::ostream& operator<<(std::ostream& s, big_integer const& a);

#endif // BIG_INTEGER_H