#pragma once

#include <ituGL/core/Data.h>

// Represent an attribute stored in a vertex buffer
// This attributes can be used for things like: position, color, normal, etc.
class VertexAttribute
{
public:
    class Layout;

public:
    VertexAttribute(Data::Type type, int components, bool normalized = false);

    inline Data::Type GetType() const { return m_type; }
    inline int GetComponents() const { return m_components; }
    inline bool IsNormalized() const { return m_normalized; }

    // Gets the size of the attribute
    inline int GetSize() const { return Data::GetTypeSize(m_type) * m_components; }

    // Gets how many location indices the attribute needs (usually 1)
    int GetLocationSize() const;

private:
    // (C++) 6
    // Data type of the attribute. Usually an integer or floating point type
    Data::Type m_type : 16;
    // How many components does the attribute have. It can be 1 for scalars or 2, 3 or 4 for vectors
    unsigned int m_components : 3;
    // If an integer value is normalized, it is converted to a [0, 1] floating point on the GPU
    bool m_normalized : 1;
};

// Helper class that contains an attribute, the offset where it starts in a buffer, and the stride
class VertexAttribute::Layout
{
public:
    Layout(const VertexAttribute& attribute, GLint offset, GLsizei stride);

    inline const VertexAttribute& GetAttribute() const { return m_attribute; }
    inline GLint GetOffset() const { return m_offset; }
    inline GLsizei GetStride() const { return m_stride; }

private:
    VertexAttribute m_attribute;
    GLint m_offset;
    GLsizei m_stride;
};
