#pragma once

#include "pc.h"

template<typename T>
class Property
{
private:
    T data = (T)0, prev_data = (T)0;
    bool changed = false;

public:
    Property() {}
    Property(Property&& rhs) { operator=(rhs); }
    Property(const Property& rhs) { operator=(rhs); }
    Property(T&& rhs) { operator=(rhs); }
    Property(const T& rhs) { operator=(rhs); }

    Property<T>& operator=(const T& rhs) {
        if( changed = (data != rhs) )
            data = rhs;

        return *this;
    }

    Property<T>& operator=(T&& rhs) {
        if( changed = (data != rhs) )
            data = rhs;

        rhs = (T)0;

        return *this;
    }

    Property<T>& operator=(const Property<T>& rhs) {
        if( changed = (data != rhs.data) ) 
            data = rhs.data;

        return *this;
    }

    Property<T>& operator=(Property<T>&& rhs) {
        if( this == rhs )
            return *this;

        changed = (data != rhs.data);
        data = std::move(rhs.data);

        rhs.changed = true;
        rhs.data = (T)0;

        return *this;
    }

    operator T() const { return data; }

    template<typename C>
    operator C() const { return static_cast<const C>(data); }

    bool IsChanged() const { return changed && (prev_data != data); }
    void Reset() { changed = false; prev_data = data; }

    T get() const { return data; }
    void set(const T& rhs)           { operator=(rhs); }
    void set(T&& rhs)                { operator=(rhs); }
    void set(const Property<T>& rhs) { operator=(rhs); }
    void set(Property<T>&& rhs)      { operator=(rhs); }

    typedef T type;
};
