#pragma once

#include "buffer.hpp"

namespace tlw {
namespace gl {

template<typename Type>
class BufferMapping {
public:
    BufferMapping(const Buffer<Type>& b, GLenum access)
        : buffer(b)
        , data_(static_cast<Type*>(glMapNamedBuffer(b, access)))
    {}

    ~BufferMapping() {
        glUnmapNamedBuffer(buffer);
    }

    Type* data() const {
        return data_;
    }

    Type& operator[](int i) {
        return data_[i];
    }

private:
    const Buffer<Type>& buffer; 

    Type* data_ = nullptr;
};

}}
