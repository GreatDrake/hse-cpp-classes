#include <cstddef>
#include <algorithm>
#include <tuple>
#include <memory>

template<typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
private:
    std::tuple<T*, Deleter> t;

public:
    UniquePtr() noexcept {
        std::get<0>(t) = nullptr;
    }

    UniquePtr(T* p) {
        std::get<0>(t) = p;
    }

    UniquePtr(T* p, Deleter d) : t(std::make_tuple(p, d)) { }

    UniquePtr(UniquePtr&& other) : UniquePtr() {
        std::swap(std::get<0>(t), std::get<0>(other.t));
    }

    UniquePtr& operator=(std::nullptr_t) {
        std::get<1>(t)(std::get<0>(t));
        std::get<0>(t) = nullptr;
        return *this;
    }

    UniquePtr& operator=(UniquePtr&& other) {
        if (std::get<0>(t) != std::get<0>(other.t))
            std::swap(std::get<0>(t), std::get<0>(other.t));
        return *this;
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    ~UniquePtr() noexcept {
        std::get<1>(t)(std::get<0>(t));
    }

    const Deleter& get_deleter() const {
        return std::get<1>(t);
    }

    Deleter& get_deleter() {
        return std::get<1>(t);
    }

    const T& operator*() const {
        return *std::get<0>(t);
    }

    T& operator*() {
        return *std::get<0>(t);
    }

    T* operator->() {
        return std::get<0>(t);
    }

    const T* operator->() const {
        return std::get<0>(t);
    }

    T* release() {
        T* ret = std::get<0>(t);
        std::get<0>(t) = nullptr;
        return ret;
    }

    void reset(T* p) {
        std::get<1>(t)(std::get<0>(t));
        std::get<0>(t) = p;
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(std::get<0>(t), std::get<0>(other.t));
    }

    T* get() const {
        return std::get<0>(t);
    }

    explicit operator bool() const noexcept {
        return std::get<0>(t) != nullptr;
    }
};