#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <map>

template<typename T>
class Polynomial {
private:
    std::map<size_t, T> p;

    T get(size_t i) const {
        return (p.find(i) == p.end() ? T() : p.at(i));
    }

    void set(size_t i, const T& v) {
        if (v == T() && p.find(i) != p.end())
            p.erase(i);
        else if (v != T())
            p[i] = v;
    }

    static T b_pow(T a, size_t b) {
        T res = T(1);
        while (true) {
            if (b & 1)
                res *= a;
            b >>= 1;
            if (!b)
                break;
            a *= a;
        }
        return res;
    }

    Polynomial<T> pow(size_t b) const {
        Polynomial<T> res = T(1), cur = *this;
        while (true) {
            if (b & 1)
                res *= cur;
            b >>= 1;
            if (!b)
                break;
            cur *= cur;
        }
        return res;
    }

public:
    Polynomial() { }

    Polynomial(const std::vector<T>& v) {
        for (size_t i = 0; i < v.size(); ++i)
            set(i, v[i]);
    }

    Polynomial(const T& k) {
        set(0, k);
    }

    template<typename Iter>
    Polynomial(Iter begin, Iter end) {
        for (size_t i = 0; begin != end; ++begin, ++i)
            set(i, *begin);
    }

    typename std::map<size_t, T>::const_iterator begin() const {
        return p.cbegin();
    }

    typename std::map<size_t, T>::const_iterator end() const {
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
    if (p.begin() == p.end())
        return -1;
    auto it = p.end();
    --it;
    return it->first;
}

template<typename T>
T Polynomial<T>::operator[](size_t i) const {
    return get(i);
}

template<typename T>
bool Polynomial<T>::operator==(const Polynomial<T>& other) const {
    return (Degree() == other.Degree()) &&
           (std::equal(std::begin(p), std::end(p), std::begin(other.p)));
}

template<typename T>
bool Polynomial<T>::operator!=(const Polynomial<T>& other) const {
    return !(*this == other);
 }

template<typename T>
Polynomial<T>& Polynomial<T>::operator+=(const Polynomial<T>& other) {
    Polynomial<T> tmp = other;
    for (auto it = tmp.p.begin(); it != tmp.p.end(); ++it)
        set(it->first, get(it->first) + it->second);
    return *this;
}

template<typename T>
Polynomial<T>& Polynomial<T>::operator-=(const Polynomial<T>& other) {
    Polynomial<T> tmp = other;
    for (auto it = tmp.p.begin(); it != tmp.p.end(); ++it)
        set(it->first, get(it->first) - it->second);
    return *this;
}

template<typename T>
Polynomial<T>& Polynomial<T>::operator*=(const Polynomial<T>& other) {
    Polynomial<T> tmp = other, copy = *this;
    p.clear();
    for (auto it1 = copy.p.begin(); it1 != copy.p.end(); ++it1)
        for (auto it2 = tmp.p.begin(); it2 != tmp.p.end(); ++it2)
            set(it1->first + it2->first, get(it1->first + it2->first) + it1->second * it2->second);
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
    Polynomial<T> res = T();
    for (auto it = p.begin(); it != p.end(); ++it)
        res += r.pow(it->first) * it->second;
    return res;
}

template<typename T>
Polynomial<T> Polynomial<T>::operator/(const Polynomial<T>& r) const {
    Polynomial<T> res = T(), cur = *this, other = r;
    while (cur.Degree() >= other.Degree()) {
        size_t dif = cur.Degree() - other.Degree();
        Polynomial<T> t = T();
        t.set(dif, cur[cur.Degree()] / other[other.Degree()]);
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
    T res = T();
    for (auto it = p.begin(); it != p.end(); ++it)
        res += it->second * b_pow(v, it->first);
    return res;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Polynomial<T>& p) {
    if (p.Degree() == -1) {
        out << T();
    } else if (p.Degree() == 0) {
        out << p[0];
    } else if (p.Degree() > 0) {
        auto it = p.end();
        --it;
        if (it->second == T(-1))
            out << '-';
        else if (it->second != T(1))
            out << it->second << '*';
        out << 'x';
        if (it->first > 1)
            out << '^' << it->first;
        if (it == p.begin())
            return out;
        while (true) {
            --it;
            if (it->first == 0) {
                if (it->second > T(0))
                    out << '+';
                if (it->second != T())
                    out << it->second;
            } else {
                if (it->second > T(0))
                    out << '+';
                if (it->second == T(-1))
                    out << '-';
                if (it->second != T(1) && it->second != T(-1))
                    out << it->second << '*';
                out << 'x';
                if (it->first > 1)
                    out << '^' << it->first;
            }
            if (it == p.begin())
                break;
        }
    }
    return out;
}