#pragma once

#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <stdexcept>

using namespace std::string_literals;

struct ReserveProxyObj {
    ReserveProxyObj(size_t capacity_) : capacity(capacity_) {

    }
    size_t capacity = 0;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    SimpleVector(ReserveProxyObj r) {
        Reserve(r.capacity);
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : array_(size), size_(size), capacity_(size) {            
        std::fill(begin(), end(), Type{});
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : array_(size), size_(size), capacity_(size) {        
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : array_(init.size()), size_(init.size()), capacity_(init.size()) {        
        std::move(init.begin(), init.end(), begin());
    }

    SimpleVector(SimpleVector&& other) {
        array_ = std::move(other.array_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    SimpleVector& operator=(SimpleVector&& other) {
        array_ = std::move(other.array_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
        return *this;
    }

    SimpleVector(const SimpleVector& other) {
        ArrayPtr<Type> arr(other.size_);
        std::copy(other.begin(), other.end(), arr.Get());
        array_.swap(arr);
        size_ = other.size_;
        capacity_ = other.capacity_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        SimpleVector simple_vector(rhs);
        swap(simple_vector);
        return *this;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {        
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return array_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return array_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) throw std::out_of_range("index >= size"s);
        return array_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) std::out_of_range("index >= size"s);
        return array_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {        
        if (new_size > size_) {
            if (new_size > capacity_) {
                capacity_ = new_size;
                ArrayPtr<Type> arr(capacity_);
                std::move(begin(), end(), arr.Get());
                array_.swap(arr);
            }   
            for (auto it = end(); it != (begin() + new_size); ++it) *it = std::move(Type{});                
        }
        size_ = new_size;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return array_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return array_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return array_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return array_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return array_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return array_.Get() + size_;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        CheckCapacitanceWhenPushBack();
        *end() = item;
        ++size_;        
    }

    // Добавляет по ссылке rvalue элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        CheckCapacitanceWhenPushBack();
        *end() = std::move(item);
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        Iterator res = ShiftAndMoveElementsWhenInsert(pos);
        *res = value;
        ++size_;
        return res;
    }

    // Вставляет по ссылке rvalue значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value) {        
        Iterator res = ShiftAndMoveElementsWhenInsert(pos);
        *res = std::move(value);
        ++size_;
        return res;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        size_t dist = std::distance(cbegin(), pos);
        std::move(begin() + dist + 1, end(), begin() + dist);
        --size_;
        return begin() + dist;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        array_.swap(other.array_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        ArrayPtr<Type> arr(new_capacity);
        std::move(begin(), end(), arr.Get());        
        array_.swap(arr);
        capacity_ = new_capacity;
    }

private:
    void CheckCapacitanceWhenPushBack() {
        if (size_ == capacity_) {
            capacity_ = std::max(capacity_ * 2, size_t(2));
            ArrayPtr<Type> arr(capacity_);
            std::move(begin(), end(), arr.Get());
            array_.swap(arr);            
        }        
    }

    Iterator ShiftAndMoveElementsWhenInsert(ConstIterator pos) {
        size_t dist = std::distance(cbegin(), pos);
        if (size_ == capacity_) {
            capacity_ = std::max(capacity_ * 2, size_t(2));
            ArrayPtr<Type> arr(capacity_);
            std::move(begin(), begin() + dist, arr.Get());            
            std::move(begin() + dist, end(), arr.Get() + dist + 1);            
            array_.swap(arr);
        } else {
            std::move_backward(begin() + dist, end(), end() + 1);            
        }
        return begin() + dist;
    }

    ArrayPtr<Type> array_ = ArrayPtr<Type>();
    size_t size_ = 0;
    size_t capacity_ = 0;
};


template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    } else if (lhs.begin() == rhs.begin()) {
        return true;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());;
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {    
    return !(lhs < rhs);
}

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}
