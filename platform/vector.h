#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <memory>
#include <algorithm>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory& rhs) = delete;

    RawMemory(RawMemory&& other) noexcept {
        Swap(other);
    }

    RawMemory& operator=(RawMemory&& rhs) noexcept { 
        Swap(rhs);
        return *this;
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector {
public:

    using iterator = T*;
    using const_iterator = const T*;
    
    iterator begin() noexcept {
        return m_data.GetAddress();
    }

    iterator end() noexcept {
        return begin() + m_size;
    }

    const_iterator begin() const noexcept {
        return m_data.GetAddress();
    }

    const_iterator end() const noexcept {
        return begin() + m_size;
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    Vector() = default;

    explicit Vector(size_t size)
        : m_data(size)
        , m_size(size)
    {
        std::uninitialized_value_construct_n(begin(), size);
    }

    Vector(const Vector& other)
        : m_data(other.m_size)
        , m_size(other.m_size) 
    {   
        std::uninitialized_copy_n(other.begin(), other.m_size, begin());
    }

    Vector(Vector&& other) noexcept {        
        Swap(other);
    }

    Vector& operator=(const Vector& rhs) {
        if (this == &rhs) return *this;
        
        if (rhs.m_size > m_data.Capacity()) {
            Vector rhs_copy(rhs);
            Swap(rhs_copy);
        } else {            
            if (m_size > rhs.m_size) {
                DestroyN(begin() + rhs.m_size, m_size - rhs.m_size);
                for (size_t i = 0; i < rhs.m_size; ++i) {
                    m_data[i] = rhs.m_data[i];
                }
            } else {
                for (size_t i = 0; i < m_size; ++i) {
                    m_data[i] = rhs.m_data[i];
                }
                std::uninitialized_copy_n(rhs.begin() + m_size, rhs.m_size - m_size, begin() + m_size);
            }
                        
            m_size = rhs.m_size;
        }
        
        return *this;       
    }

    Vector& operator=(Vector&& rhs) noexcept {
        if (this == &rhs) return *this;
        Swap(rhs);
        return *this;
    }

    void Swap(Vector& other) noexcept {        
        m_data.Swap(other.m_data);
        std::swap(m_size, other.m_size);
    }

    ~Vector() {
        DestroyN(begin(), m_size);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= m_data.Capacity()) return;
        
        RawMemory<T> new_data(new_capacity);
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(begin(), m_size, new_data.GetAddress());
        } else {
            std::uninitialized_copy_n(begin(), m_size, new_data.GetAddress());
        }

        DestroyN(begin(), m_size);

        m_data.Swap(new_data);
    }

    void Resize(size_t new_size) {
        if (m_size == new_size) return;
        if(new_size < m_size) {
            DestroyN(begin() + new_size, m_size - new_size);
        } else {
            Reserve(new_size);
            std::uninitialized_value_construct_n(begin() + m_size, new_size - m_size);
        }
        m_size = new_size;
    }

    void PushBack(const T& value) {
        EmplaceBack(value);
    }
    
    void PushBack(T&& value) {
        EmplaceBack(std::move(value));
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (m_size == Capacity()) {
            RawMemory<T> new_data(m_size == 0 ? 1 : m_size * 2);
            new(new_data + m_size) T(std::forward<Args>(args)...);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(begin(), m_size, new_data.GetAddress());
            } else {
                std::uninitialized_copy_n(begin(), m_size, new_data.GetAddress());
            }

            DestroyN(begin(), m_size);

            m_data.Swap(new_data);

        } else {
            new(begin() + m_size) T(std::forward<Args>(args)...);
        }
        return m_data[m_size++];
    }

    void PopBack() {
        Destroy(begin() + m_size);
        --m_size;
    }

    iterator Insert(const_iterator pos, const T& value) {
        return Emplace(pos, value);
    }

    iterator Insert(const_iterator pos, T&& value) {
        return Emplace(pos, std::move(value));
    }

    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args) {
        size_t index = pos - begin();
        if (m_size == Capacity()) {
            RawMemory<T> new_data(m_size == 0 ? 1 : m_size * 2);
            new(new_data + index) T(std::forward<Args>(args)...);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(begin(), index, new_data.GetAddress());
                std::uninitialized_move_n(begin() + index, m_size - index, new_data + index + 1);
            } else {
                std::uninitialized_copy_n(begin(), index, new_data.GetAddress());
                std::uninitialized_copy_n(begin() + index, m_size - index, new_data + index + 1);
            }
            
            DestroyN(begin(), m_size);

            m_data.Swap(new_data);
        } else {
            T t{std::forward<Args>(args)...};
            if (pos == end()) {
                if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                    new(end()) T(std::move(t));                    
                } else {
                    new(end()) T(t);
                }
            } else {
                new(end()) T(std::move(*(end() - 1)));
                std::move_backward(begin() + index, end() - 1, end());
                *(begin() + index) = std::move(t);
            }
        }
        
        ++m_size;

        return begin() + index;
    }

    iterator Erase(const_iterator pos) {
        size_t index = pos - begin();
        Destroy(begin() + index);
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::move(begin() + index + 1, end(), begin() + index);
        } else {
            std::copy(begin() + index + 1, end(), begin() + index);
        }
        
        --m_size;
        return begin() + index;
    }

    size_t Size() const noexcept {
        return m_size;
    }

    size_t Capacity() const noexcept {
        return m_data.Capacity();
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {        
        return m_data[index];
    }

private:    

    // Вызывает деструкторы n объектов массива по адресу buf
    static void DestroyN(T* buf, size_t n) noexcept {
        for (size_t i = 0; i != n; ++i) {
            Destroy(buf + i);
        }
    }

    // Создаёт копию объекта elem в сырой памяти по адресу buf
    static void CopyConstruct(T* buf, const T& elem) {
        new (buf) T(elem);
    }

    // Вызывает деструктор объекта по адресу buf
    static void Destroy(T* buf) noexcept {
        buf->~T();
    }

    RawMemory<T> m_data;
    size_t m_size = 0;
};