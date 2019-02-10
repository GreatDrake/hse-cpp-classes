#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <cmath>

template<typename T>
class Matrix {
private:
    size_t rows, cols;
    std::vector<std::vector<T>> m;
    class iterator;
    class const_iterator;

public:
    Matrix(const std::vector<std::vector<T>>& v) : m(v) {
        rows = v.size();
        cols = (v.empty() ? 0 : v[0].size());
    }

    iterator begin() {
        return iterator(0, 0, *this);
    }

    iterator end() {
        return iterator(rows, 0, *this);
    }

    const_iterator begin() const {
        return const_iterator(0, 0, *this);
    }

    const_iterator end() const {
        return const_iterator(rows, 0, *this);
    }

    std::pair<size_t, size_t> size() const;

    const T& operator()(size_t i, size_t j) const;

    T& operator()(size_t i, size_t j);

    Matrix& operator+=(const Matrix& other);

    template<typename TI>
    Matrix& operator*=(const TI& other);

    Matrix& operator*=(const Matrix& other);

    Matrix& transpose();

    Matrix transposed() const;

    template<typename U>
    std::vector<U> solve(const std::vector<U>& b) const;
};

template<typename T>
class Matrix<T>::iterator {
private:
    size_t i, j;
    Matrix<T>& ref;

public:
    iterator(size_t a, size_t b, Matrix<T>& r) : i(a), j(b), ref(r) { }

    iterator operator++() {
        ++j;
        if (j >= ref.size().second) {
            ++i;
            j = 0;
        }
        return *this;
    }

    iterator operator++(int) {
        iterator prev = *this;
        ++j;
        if (j >= ref.size().second) {
            ++i;
            j = 0;
        }
        return prev;
    }

    T& operator*() {
        return ref(i, j);
    }

    bool operator==(const iterator& other) {
        return (i == other.i) && (j == other.j);
    }

    bool operator!=(const iterator& other) {
        return !(*this == other);
    }
};

template<typename T>
class Matrix<T>::const_iterator {
private:
    size_t i, j;
    const Matrix<T>& ref;

public:
    const_iterator(size_t a, size_t b, const Matrix<T>& r) : i(a), j(b), ref(r) { }

    const_iterator operator++() {
        ++j;
        if (j >= ref.size().second) {
            ++i;
            j = 0;
        }
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator prev = *this;
        ++j;
        if (j >= ref.size().second) {
            ++i;
            j = 0;
        }
        return prev;
    }

    const T& operator*() const {
        return ref(i, j);
    }

    bool operator==(const const_iterator& other) {
        return (i == other.i) && (j == other.j);
    }

    bool operator!=(const const_iterator& other) {
        return !(*this == other);
    }
};

template<typename T>
std::pair<size_t, size_t> Matrix<T>::size() const {
     return {rows, cols};
}

template<typename T>
const T& Matrix<T>::operator()(size_t i, size_t j) const {
    return m[i][j];
}

template<typename T>
T& Matrix<T>::operator()(size_t i, size_t j) {
    return m[i][j];
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& m) {
    for (size_t i = 0; i < m.size().first; ++i) {
        if (i)
            out << '\n';
        out << m(i, 0);
        for (size_t j = 1; j < m.size().second; ++j)
            out << '\t' << m(i, j);
    }
    return out;
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& other) {
    for (size_t i = 0; i != rows; ++i)
        for (size_t j = 0; j != cols; ++j)
            m[i][j] += other(i, j);
    return *this;
}

template<typename T>
template<typename TI>
Matrix<T>& Matrix<T>::operator*=(const TI& other) {
    for (size_t i = 0; i != rows; ++i)
        for (size_t j = 0; j != cols; ++j)
            m[i][j] *= other;
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const Matrix<T>& other) {
    std::vector<std::vector<T>> res(rows, std::vector<T>(other.cols, T()));
    for (size_t i = 0; i != rows; ++i)
        for (size_t j = 0; j != cols; ++j)
            for (size_t k = 0; k != other.cols; ++k)
                res[i][k] += m[i][j] * other.m[j][k];
    m = res;
    cols = other.cols;
    return *this;
}

template<typename T>
Matrix<T> operator+(const Matrix<T>& l, const Matrix<T>& r) {
    Matrix<T> res = l;
    res += r;
    return res;
}

template<typename T, typename TI>
Matrix<T> operator*(const Matrix<T>& l, const TI& r) {
    Matrix<T> res = l;
    res *= r;
    return res;
}

template<typename T, typename TI>
Matrix<T> operator*(const TI& l, const Matrix<T>& r) {
    Matrix<T> res = r;
    res *= l;
    return res;
}

template<typename T>
Matrix<T> operator*(const Matrix<T>& l, const Matrix<T>& r) {
    Matrix<T> res = l;
    res *= r;
    return res;
}

template<typename T>
Matrix<T>& Matrix<T>::transpose() {
    std::vector<std::vector<T>> res(cols, std::vector<T>(rows));
    for (size_t i = 0; i != rows; ++i)
        for (size_t j = 0; j != cols; ++j)
            res[j][i] = m[i][j];
    m = res;
    std::swap(rows, cols);
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::transposed() const {
    Matrix<T> res = *this;
    res.transpose();
    return res;
}

template<typename T>
template<typename U>
std::vector<U> Matrix<T>::solve(const std::vector<U>& b) const {
    std::vector<std::vector<U>> s(rows, std::vector<U>(cols));
    for (size_t i = 0; i < s.size(); ++i)
        for (size_t j = 0; j < s.size(); ++j)
            s[i][j] = static_cast<U>(m[i][j]);
    for (size_t i = 0; i < b.size(); ++i)
        s[i].push_back(b[i]);

    for (size_t j = 0; j < s.size(); ++j) {
        size_t maxi = j;
        for (size_t i = j + 1; i < s.size(); ++i)
            if (std::abs(s[i][j]) > std::abs(s[maxi][j]))
                maxi = i;
        std::swap(s[j], s[maxi]);
        for (size_t i = 0; i < s.size(); ++i) {
            if (i == j)
                continue;
            U d = s[i][j] / s[j][j];
            for (size_t k = j; k <= s.size(); ++k)
                s[i][k] -= s[j][k] * d;
        }
    }

    std::vector<U> ans(s.size());
    for (size_t i = 0; i < s.size(); ++i)
        ans[i] = s[i][s.size()] / s[i][i];
    return ans;
}