#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;

    Optional(const T& value) {        
        m_value = new(&m_data) T(value);
        m_is_initialized = true;
    }

    Optional(T&& value) {        
        m_value = new(&m_data) T(std::move(value));
        m_is_initialized = true;
    }

    Optional(const Optional& other) { // 1
        if (this == &other) return;
        if (!other.m_is_initialized) return;

        m_value = new(&m_data) T(*other.m_value);
        m_is_initialized = true;
    }

    Optional(Optional&& other) {
        if (this == &other) return;
        if (!other.m_is_initialized) return;
        
        m_value = new(&m_data) T(std::move(*other.m_value));
        m_is_initialized = true;
    }

    Optional& operator=(const T& value) {
        if (m_is_initialized) {
            *m_value = value;
        } else {
            m_value = new(&m_data) T(value);
            m_is_initialized = true;    
        }
        
        return *this;
    }

    Optional& operator=(T&& rhs) {
        if (m_is_initialized) {
            *m_value = std::move(rhs);
        } else {
            m_value = new(&m_data) T(std::move(rhs));
            m_is_initialized = true;
        }

        return *this;
    }

    Optional& operator=(const Optional& rhs) {  //2
        if (this == &rhs) return *this;

        if (m_is_initialized && rhs.m_is_initialized) {            
            *m_value = *rhs.m_value;
            m_is_initialized = true;
        } else if (!m_is_initialized && rhs.m_is_initialized) {
            m_value = new(&m_data) T(*rhs.m_value);
            m_is_initialized = true;
        } else if (m_is_initialized && !rhs.m_is_initialized) {
            m_value->~T();            
            m_is_initialized = false;
        } else if (!m_is_initialized && !rhs.m_is_initialized) {            
            m_is_initialized = false;
        }

        return *this;
    }

    Optional& operator=(Optional&& rhs) {
        if (this == &rhs) return *this;
        
        if (m_is_initialized && rhs.m_is_initialized) {            
            *m_value = std::move(*rhs.m_value);
            m_is_initialized = true;
        } else if (!m_is_initialized && rhs.m_is_initialized) {
            m_value = new(&m_data) T(std::move(*rhs.m_value));
            m_is_initialized = true;
        } else if (m_is_initialized && !rhs.m_is_initialized) {
            m_value->~T();            
            m_is_initialized = false;
        }
        
        return *this;
    }

    ~Optional() {
        if (m_is_initialized) m_value->~T();
    }

    bool HasValue() const {
        return m_is_initialized;
    }

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*() { 
        return *this->m_value; 
    }

    const T& operator*() const { 
        return *this->m_value; 
    }

    T* operator->() { 
        return this->m_value; 
    }

    const T* operator->() const { 
        return this->m_value; 
    }

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value() {
        if (!m_is_initialized) throw BadOptionalAccess();
        return *m_value;
    }
    const T& Value() const {
        if (!m_is_initialized) throw BadOptionalAccess();
        return *m_value;
    }

    template<typename... Args>
    Optional& Emplace(Args&&... args) {
        
        if (m_is_initialized) m_value->~T();
        else m_is_initialized = true;

        m_value = new(&m_data) T(std::forward<Args>(args)...);
        
        return *this;
    }

    void Reset() {
        if (!m_is_initialized) return;
        m_value->~T();
        m_is_initialized = false;
    }

private:
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char m_data[sizeof(T)];
    bool m_is_initialized = false;
    T* m_value = nullptr;
};