#include "vector.h"
#include "iostream"

#define PRACTIKUM_ASSERT_EQUAL_OBJ(obj, def_ctor_, copy_ctor_, move_ctor_, copy_assign_, move_assign_, dtor_) \
    assert(obj::def_ctor == def_ctor_);                                                         \
    assert(obj::copy_ctor == copy_ctor_);                                                       \
    assert(obj::move_ctor == move_ctor_);                                                       \
    assert(obj::copy_assign == copy_assign_);                                                   \
    assert(obj::move_assign == move_assign_);                                                   \
    assert(obj::dtor == dtor_);

int DEFAULT_COOKIE = 1234;
int MAGIC = 123;
int SIZE = 10;

template<bool MoveNoexcept>
struct WithCopy {
    WithCopy() noexcept {
        ++def_ctor;
    }

    WithCopy(const int&) noexcept {
        ++copy_with_val;
    }

    WithCopy(int&&) noexcept {
        ++move_with_val;
    }

    WithCopy(const WithCopy & /*other*/) noexcept {
        ++copy_ctor;
    }

    WithCopy(WithCopy && /*other*/) noexcept(MoveNoexcept) {
        ++move_ctor;
    }

    WithCopy &operator=(const WithCopy &other) noexcept {
        if (this != &other) {
            ++copy_assign;
        }
        return *this;
    }

    WithCopy &operator=(WithCopy && /*other*/) noexcept {
        ++move_assign;
        return *this;
    }

    ~WithCopy() {
        ++dtor;
    }

    static size_t InstanceCount() {
        return def_ctor + copy_ctor + move_ctor - dtor;
    }

    static void Reset() {
        def_ctor = 0;
        copy_ctor = 0;
        move_ctor = 0;
        copy_assign = 0;
        move_assign = 0;
        dtor = 0;
        copy_with_val = 0;
        move_with_val = 0;
    }

    inline static size_t def_ctor = 0;
    inline static size_t copy_ctor = 0;
    inline static size_t move_ctor = 0;
    inline static size_t copy_assign = 0;
    inline static size_t move_assign = 0;
    inline static size_t dtor = 0;
    inline static size_t copy_with_val = 0;
    inline static size_t move_with_val = 0;

};

using move_without_noexcept = WithCopy<false>;


template<typename OBJ>
void TestInsertAdditionalCopyImpl(size_t copy, size_t move) {
    {
        OBJ a;
        Vector <OBJ> v(SIZE);
        OBJ::Reset();
        v.Insert(v.begin(), a);

        PRACTIKUM_ASSERT_EQUAL_OBJ(OBJ, 0u, copy + 1, move, 0u, 0u, SIZE)
    }
    {
        OBJ a;
        Vector <OBJ> v(SIZE);
        v.Reserve(2 * SIZE);
        OBJ::Reset();
        v.Insert(v.begin(), a);

        PRACTIKUM_ASSERT_EQUAL_OBJ(OBJ, 0u, 1u, 1u, 0u, SIZE, 1u)
    }
}

template <typename T>
void PrintVector(const T& t) {
    for (auto it = t.begin(); it != t.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

int main() {
    TestInsertAdditionalCopyImpl<move_without_noexcept>(SIZE, 0u);
    TestInsertAdditionalCopyImpl<move_without_noexcept>(SIZE, 0u);

    Vector<int> v;
    v.Reserve(10);
    PrintVector(v);
    v.Insert(v.begin(), 0);
    PrintVector(v);
    v.Insert(v.begin(), 1);
    PrintVector(v);
    v.Insert(v.begin(), 2);
    PrintVector(v);
    v.Insert(v.begin(), 3);
    PrintVector(v);
    v.Insert(v.begin(), 4);
    PrintVector(v);
    v.Insert(v.begin(), 5);
    PrintVector(v);

    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }    
    
    return 0;
}
