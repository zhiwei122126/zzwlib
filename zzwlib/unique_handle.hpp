
#pragma once

namespace zzwlib {

template<typename deletor>
class unique_handle final {
public:
    static const int INVALID_HANDLE_VALUE = -1;
    unique_handle(int handle_, deletor del_) :
            m_handle(handle_),
            m_deleter(del_) {
    }
    unique_handle(unique_handle&& other)
    {
        m_handle = other.m_handle;
        m_deleter = std::move(other.m_deleter);
        other.m_handle = INVALID_HANDLE_VALUE;
    }
    unique_handle& operator=(unique_handle&& other) {
        if (this != &other) {
            clear();
            m_handle = other.m_handle;
            m_deleter = std::move(other.m_deleter);
            other.m_handle = INVALID_HANDLE_VALUE;
        }
        return *this;
    }
    void clear() {
        if (m_handle != INVALID_HANDLE_VALUE) {
            m_deleter(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
        }
    }
    ~unique_handle() {
        clear();
    }
    int get() const {
        return m_handle;
    }
    bool operator!() const {
        return m_handle == INVALID_HANDLE_VALUE;
    }
private:
    int m_handle = INVALID_HANDLE_VALUE;
    deletor m_deleter;
    // disable copy constructor and copy assignment
    unique_handle(const unique_handle&) = delete;
    unique_handle& operator=(const unique_handle&) = delete;
};

};
