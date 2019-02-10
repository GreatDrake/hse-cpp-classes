#include <cstddef>
#include <algorithm>

template<typename T>
class Control {
public:
    T* ptr;
    size_t ref_cnt;

    Control(T* p) : ptr(p), ref_cnt(1) { }

    void dec() {
        --ref_cnt;
        if (!ref_cnt) {
            delete ptr;
            ptr = nullptr;
        }
    }

    void inc() {
        ++ref_cnt;
    }

    ~Control() { }
};

template<typename T>
class SharedPtr {
private:
    T* ptr;
    Control<T>* ctrl;

    void inc() {
        if (ctrl != nullptr)
            ctrl->inc();
    }

    void dec() {
        if (ctrl != nullptr) {
            ctrl->dec();
            if (!ctrl->ref_cnt) {
                delete ctrl;
                ctrl = nullptr;
                ptr = nullptr;
            }
        }
    }

public:
    SharedPtr() noexcept : ptr(nullptr), ctrl(nullptr) { }

    SharedPtr(T* p) : ptr(p) {
        if (p != nullptr)
            ctrl = new Control<T>(p);
        else
            ctrl = nullptr;
    }

    SharedPtr(const SharedPtr& other) noexcept : ptr(other.ptr), ctrl(other.ctrl) {
        inc();
    }

    SharedPtr(SharedPtr&& other) noexcept : SharedPtr() {
        std::swap(ptr, other.ptr);
        std::swap(ctrl, other.ctrl);
    }

    SharedPtr& operator=(T* p) {
        dec();
        ptr = p;
        if (p != nullptr)
            ctrl = new Control<T>(p);
        else
            ctrl = nullptr;
        return *this;
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this == &other)
            return *this;
        dec();
        ptr = other.ptr;
        ctrl = other.ctrl;
        inc();
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this == &other)
            return *this;
        dec();
        ptr = other.ptr;
        ctrl = other.ctrl;
        other.ptr = nullptr;
        other.ctrl = nullptr;
        return *this;
    }

    ~SharedPtr() noexcept {
        dec();
    }

    const T& operator*() const noexcept {
        return *ptr;
    }

    T& operator*() noexcept {
        return *ptr;
    }

    T* operator->() noexcept {
        return ptr;
    }

    const T* operator->() const noexcept {
        return ptr;
    }

    void reset(T* p) {
        dec();
        ptr = p;
        if (p != nullptr)
            ctrl = new Control<T>(p);
        else
            ctrl = nullptr;
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr, other.ptr);
        std::swap(ctrl, other.ctrl);
    }

    T* get() const noexcept {
        return ptr;
    }

    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }
};

