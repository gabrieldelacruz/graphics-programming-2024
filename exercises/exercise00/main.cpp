#include <ituGL/core/DeviceGL.h>
#include <ituGL/application/Window.h>
#include <iostream>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/geometry/VertexBufferObject.h>
#include <ituGL/geometry/VertexAttribute.h>
#include <ituGL/geometry/ElementBufferObject.h>

int buildShaderProgram();
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	DeviceGL deviceGL;

	// glfw window creation
	// --------------------
	Window window(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL");


	if (!window.IsValid())
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	deviceGL.SetCurrentWindow(window);
	if (!deviceGL.IsReady())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader program
	// ------------------------------------
	int shaderProgram = buildShaderProgram();

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.5f,  0.5f, 0.0f,  // top   
		-0.5f,  0.5f, 0.0f  // bottom
	};

	VertexBufferObject vbo;
	VertexArrayObject vao;
	ElementBufferObject ebo;
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	vao.Bind();
	vbo.Bind();
	ebo.Bind();
	// Create a std::span from the vertices array
	//std::cout << sizeof(vertices) << std::endl;

	unsigned int indices[] = { 0, 1, 2, 2, 0, 3 };
	size_t indicesCount = sizeof(indices) / sizeof(unsigned int);
	vbo.AllocateData({ reinterpret_cast<const std::byte*>(vertices), sizeof(vertices) });
	ebo.AllocateData<unsigned int>({ indices, indicesCount });
	VertexAttribute positionAttribute(Data::Type::Float, 3, false);
	vao.SetAttribute(0, positionAttribute, 0, 3 * sizeof(float));

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	vao.Unbind();
	ebo.Unbind();

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float time = 0.0f;
	float rotationSpeed = 0.1f;
	// render loop
	// -----------
	while (!window.ShouldClose())
	{
		// input
		processInput(window.GetInternalWindow());
		// render
		deviceGL.Clear(0.2f, 0.3f, 0.3f, 1.0f);

		// draw our first triangle
		glUseProgram(shaderProgram);
		vao.Bind(); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		vbo.Bind();
		float angle = time * rotationSpeed;
		// Update the VBO with the rotated vertices
		int numVertices = sizeof(vertices) / sizeof(vertices[0]) / 3;
		for (int i = 0; i < numVertices; ++i) {
			// Compute the rotated position of each vertex
			float x = vertices[i * 3];
			float y = vertices[i * 3 + 1];
			float newX = x * cos(angle) - y * sin(angle);
			float newY = x * sin(angle) + y * cos(angle);

			// Update the vertex positions
			vertices[i * 3] = newX;
			vertices[i * 3 + 1] = newY;
		}

		// Update the VBO data with the rotated vertices
		vbo.UpdateData({ reinterpret_cast<const std::byte*>(vertices), sizeof(vertices) });
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0); // no need to unbind it every time 

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		window.SwapBuffers();
		deviceGL.PollEvents();
		time += 0.1;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	// This is now done in the destructor of DeviceGL
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// --------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// build the shader program
// ------------------------
int buildShaderProgram()
{
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(0.29f, 0.54f, 0.76f, 1.0f);\n"
		"}\n\0";

	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}
