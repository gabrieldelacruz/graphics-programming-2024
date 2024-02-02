#include <ituGL/geometry/VertexArrayObject.h>

#include <ituGL/geometry/VertexAttribute.h>
#include <cstdio>

// Create the object initially null, get object handle and generate 1 vertex array
VertexArrayObject::VertexArrayObject() : Object(NullHandle)
{
	Handle& handle = GetHandle();
	// (todo) 00.1: Generate 1 vertex array
	glGenVertexArrays(1, &handle);
}

// Get object handle and delete 1 vertex array
VertexArrayObject::~VertexArrayObject()
{
	Handle& handle = GetHandle();
	// (todo) 00.1: Delete 1 vertex array
	if (handle != 0) {
		glDeleteVertexArrays(1, &handle);
		handle = 0;
	}
	printf("HELLO\n");
}

// Bind the vertex array handle to the specific target
void VertexArrayObject::Bind() const
{
	Handle handle = GetHandle();
	// (todo) 00.1: Bind vertex array
	glBindVertexArray(handle);
}

// Bind the null handle to the specific target
void VertexArrayObject::Unbind()
{
	Handle handle = NullHandle;
	// (todo) 00.1: Bind null vertex array
	glBindVertexArray(0);
}

// Sets the VertexAttribute pointer and enables the VertexAttribute in that location
void VertexArrayObject::SetAttribute(GLuint location, const VertexAttribute& attribute, GLint offset, GLsizei stride)
{
	// Get the attribute properties in OpenGL expected format
	GLint components = attribute.GetComponents();
	GLenum type = static_cast<GLenum>(attribute.GetType());
	GLboolean normalized = attribute.IsNormalized() ? GL_TRUE : GL_FALSE;

	// Compute the attribute pointer
	const unsigned char* pointer = nullptr; // Actual base pointer is in VBO
	glVertexAttribPointer(location, components, type, normalized, stride, pointer);

	glEnableVertexAttribArray(location);
}
