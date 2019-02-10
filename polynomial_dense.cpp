#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

template<typename T>
class Polynomial {
private:
    std::vector<T> p;

    void cut() {
        while (p.size() && p.back() == T())
            p.pop_back();
    }

public:
    Polynomial() { }

    Polynomial(std::vector<T> v) : p(v) {
        cut();
    }

    Polynomial(const T& k) {
        p = {k};
        cut();
    }

    template<typename Iter>
    Polynomial(Iter begin, Iter end) {
        std::copy(begin, end, std::back_inserter(p));
        cut();
    }

    typename std::vector<T>::const_iterator begin() const {
        return p.cbegin();
    }

    typename std::vector<T>::const_iterator end() const {
        return p.cend();
    }

    int Degree() const;

    T operator[](size_t i) const;

    bool operator==(const Polynomial& other) const;

    bool operator!=(const Polynomial& other) const;

    Polynomial& operator+=(const Polynomial& other);

    Polynomial& operator-=(const Polynomial& other);

    Polynomial& operator*=(const Polynomial& other);

    Polynomial operator+(const Polynomial& other) const;

    Polynomial operator-(const Polynomial& other) const;

    Polynomial operator*(const Polynomial& other) const;

    Polynomial operator&(const Polynomial& other) const;

    Polynomial operator/(const Polynomial& other) const;

    Polynomial operator%(const Polynomial& other) const;

    Polynomial operator,(const Polynomial& other) const;

    T operator()(T v) const;
};

template<typename T>
int Polynomial<T>::Degree() const {
    return static_cast<int>(p.size()) - 1;
}

template<typename T>
T Polynomial<T>::operator[](size_t i) const {
    return (i >= p.size() ? T() : p[i]);
}

template<typename T>
bool Polynomial<T>::operator==(const Polynomial<T>& other) const {
    return (p.size() == other.p.size()) &&
           (std::equal(std::begin(p), std::end(p), std::begin(other.p)));
}

template<typename T>
bool Polynomial<T>::operator!=(const Polynomial<T>& other) const {
    return !(*this == other);
 }

template<typename T>
Polynomial<T>& Polynomial<T>::operator+=(const Polynomial<T>& other) {
    if (p.size() < other.p.size())
        p.resize(other.p.size());
    for (size_t i = 0; i < std::min(p.size(), other.p.size()); ++i)
        p[i] += other.p[i];
    cut();
    return *this;
}

template<typename T>
Polynomial<T>& Polynomial<T>::operator-=(const Polynomial<T>& other) {
    if (p.size() < other.p.size())
        p.resize(other.p.size());
    for (size_t i = 0; i < std::min(p.size(), other.p.size()); ++i)
        p[i] -= other.p[i];
    cut();
    return *this;
}

template<typename T>
Polynomial<T>& Polynomial<T>::operator*=(const Polynomial<T>& other) {
    size_t new_size = p.size() * other.p.size();
    std::vector<T> res(new_size, T());
    for (size_t i = 0; i < p.size(); ++i)
        for (size_t j = 0; j < other.p.size(); ++j)
            res[i + j] += p[i] * other.p[j];
    p = res;
    cut();
    return *this;
}

template<typename T>
Polynomial<T> Polynomial<T>::operator+(const Polynomial<T>& r) const {
    Polynomial<T> res = *this;
    res += r;
    return res;
}

template<typename T>
Polynomial<T> Polynomial<T>::operator-(const Polynomial<T>& r) const {
    Polynomial<T> res = *this;
    res -= r;
    return res;
}

template<typename T>
Polynomial<T> Polynomial<T>::operator*(const Polynomial<T>& r) const {
    Polynomial<T> res = *this;
    res *= r;
    return res;
}

template<typename T>
Polynomial<T> Polynomial<T>::operator&(const Polynomial<T>& r) const {
    Polynomial<T> res(T(0));
    Polynomial<T> cur(T(1));
    for (size_t i = 0; i < p.size(); ++i) {
        res += cur * p[i];
        cur *= r;
    }
    return res;
}

template<typename T>
Polynomial<T> Polynomial<T>::operator/(const Polynomial<T>& r) const {
    Polynomial<T> res = T(), cur = *this, other = r;
    while (cur.Degree() >= other.Degree()) {
        size_t dif = cur.Degree() - other.Degree();
        std::vector<T> v(dif + 1, T(0));
        v.back() = cur[cur.Degree()] / other[other.Degree()];
        Polynomial<T> t(v);
        res += t;
        cur -= other * t;
    }
    return res;
}

template<typename T>
Polynomial<T> Polynomial<T>::operator%(const Polynomial<T>& r) const {
    return *this - ((*this / r) * r);
}

template<typename T>
Polynomial<T> Polynomial<T>::operator,(const Polynomial<T>& r) const {
    Polynomial<T> cur = *this, other = r;
    while (other != T(0)) {
        cur = cur % other;
        std::swap(cur, other);
    }
    return cur / cur[cur.Degree()];
}

template<typename T>
T Polynomial<T>::operator()(T v) const {
    if (p.empty())
        return T();
    T cur = T(1), res = T();
    for (size_t i = 0; i < p.size(); ++i) {
        res += cur * p[i];
        cur *= v;
    }
    return res;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Polynomial<T>& p) {
    if (p.Degree() == -1) {
        out << T();
    } else if (p.Degree() == 0) {
        out << p[0];
    } else if (p.Degree() > 0) {
        size_t i = p.Degree();
        if (p[i] == T(-1))
            out << '-';
        else if (p[i] != T(1))
            out << p[i] << '*';
        out << 'x';
        if (i > 1)
            out << '^' << i;
        while (--i) {
            if (p[i] == T())
                continue;
            if (p[i] > T(0))
                out << '+';
            if (p[i] == T(-1))
                out << '-';
            if (p[i] != T(1) && p[i] != T(-1))
                out << p[i] << '*';
            out << 'x';
            if (i > 1)
                out << '^' << i;
        }
        if (p[0] > T(0))
            out << '+';
        if (p[0] != T())
            out << p[0];
    }
    return out;
}