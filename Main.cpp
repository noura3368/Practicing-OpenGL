#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//A SHADER IS NEEDED TO RENDER A SCENE, SHADERS ALTER VISUALS BY CHANGING TEXTRES and lightenening are displayed 

// graphics pipeline is a function that takes some data in beginning and at end, inputs a frame
// data is called vertex data which is a array of vertices
// not mathimatically vertices, as each vertex has other attributes such as colouror data
// first pahse of graphics piple is called the vertex shader
// the vertex shader keeps the position of all the vertices and transforms them (can change them or keeps them the same, up to you)
// the vertex shader connects the positions according to a primitive which is a shape
// each primitive interprets the data differently, for a triangle itwould take 3 points 
// a line would be two points with a line

// next we have geometry shader that can add vertices and then create new primitives out of already existing primitives 
//rasterization phase, so when all the perfect mathematical shapes get transformed into pixels, no colour
//fragment shade takes the output from vertex shader and associates colours, depth value of a pixel to it. After these operations the fragment is 
//sent to framebuffer for display on screen

// opengl doesnt provide defaults for vertex in fragments shaders

const char* screenVertexShaderSource = R"(#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uvs;
out vec2 UVs;
void main()
{
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	UVs = uvs;
})";
const char* screenFragmentShaderSource = R"(#version 460 core
out vec4 FragColor;
uniform sampler2D screen;
in vec2 UVs;
void main()
{
	FragColor = texture(screen, UVs);
})";

int main() {

	glfwInit();
	// glfw doesnt know what version of openGl we are using so we gotta tell it that
	// we can give it hints with special type of function that gives it that
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// hint is the first parameter 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// defines what profile aka packages of open gl we want to use
	// two packages: core which is the modern packages and 
	// compatible which are both modern and outdates pacakges
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//use glfloat instead of c++ floats as the glfloat might have different sizes than normal ones 
	GLfloat vertices[] =
	{
		//we want equilateral triangle
		// coordinate of left corner
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, // right corner
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f // top corner

	};

	// create window project size (length and width), name, if we want full screen
	GLFWwindow* window = glfwCreateWindow(800, 800, "Noura's Project", NULL, NULL);
	// error checking if window doesnt open properly
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// have to tell GLFW window that i would like to use it
	glfwMakeContextCurrent(window);
	// use glad to load the necessary packages that can change the colour of window
	gladLoadGL();

	//tell opengl the viewport of our window which is the area of window we want opengl rendering
	glad_glViewport(0, 0, 800, 800);

	// shaders are opengl object in memory, can only use them in references
	//gluint is an unsigned aka positive int, our value aka reference that stores out vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &screenVertexShaderSource, NULL); //feed in the shader code, parameters: reference value, using only one screen for whole shader, then point to source code
	glCompileShader(vertexShader); //gpu cant understand the reference to our shader. 

	//fragment shader 
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &screenFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//to use shaders, we need to wrap them in shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//now we need tell openGL how to interpret our vertices. Sending stuff between our CPU and GPU is kinda slow so we 
	// want to send in big batches called buffers (different than front and back buffer)

	// vertex array object stores pointers to one or more VBOs and tells openGL how to interpret them
	GLuint VAO, VBO; //an array of vertex references but we only have one object (triangle so we only need one)
	glGenVertexArrays(1, &VAO); // need to generate VAO before vbo 

	//vertex buffer object to store our vertex
	glGenBuffers(1, &VBO); // 1 3d object 
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // makes vbo the current object
	//this actually stores it 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// buffer from which the information is read is called the 'front buffer'
	// buffer from which the info is being written is called back buffer
	// while the frame is loading the pixels from the current frame and displaying them the next frame
	// is being written in the background away from our eyes. 
	// while the screen is finished with the current frame it starts display the next frame that was written in the background
	// while the previous frame is being overwritten with new info. these frames are called buffers
	// storage in memory for pixels

	//tell openGL to prepare the colour of a buffer and give it another colour
	glClearColor(0.07f, 0.13f, 0.0f, 1.0f); // last number is the trasnparency 1 = opaque, 0 = completely transparent. 
	// now we tell openGL to execute the command we've told it to prepare for 
	glClear(GL_COLOR_BUFFER_BIT); // at this point we have back buffer with the colour we want and front buffer with default
	// back buffer is colour opengl has prepared for
	// to see our colour we need to swap the buffers, since only the front buffer is displayed
	glfwSwapBuffers(window);

	float prev_time = float(glfwGetTime());
	float angle = 0.0f;
	while (!glfwWindowShouldClose(window)) { // dont close the window unless user manually closes it 
		// tell glfw to process all the polled events such as window resizing, appearing
		float time = float(glfwGetTime());
		if (time - prev_time >= 0.1) {
			angle += 0.1f;
			prev_time = time;
		};
		glClearColor(float(sin(angle)), float(cos(angle)), float(tan(angle)), angle);
		//glClearColor(1.0f, 0.0f, 0.05f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	// once done with window, we want to delete it
	glfwDestroyWindow(window);

	glfwTerminate();

	return 0; 

}