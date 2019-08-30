#include<glad\glad.h>
#include<GLFW\glfw3.h>

#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>

#include<Learnopengl\Shader.h>
#include<Learnopengl\Camera.h>
#include<stb_image.h>

#include<iostream>
#include<vector>

GLvoid processInput(GLFWwindow*);
GLvoid framebuffer_size_callback(GLFWwindow*, GLint, GLint);
GLvoid mouse_callback(GLFWwindow*, GLdouble, GLdouble);
GLvoid scroll_callback(GLFWwindow*, GLdouble, GLdouble);
GLuint loadTexture(const GLchar*, GLboolean);
GLuint loadHdrTexture(const GLchar*);
GLvoid setupCube();
GLvoid setupQuad();
GLvoid Sphere();

GLint SCR_WIDTH = 800, SCR_HEIGHT = 600, ENV_WIDTH = 512, ENV_HEIGHT = 512, IRR_WIDTH = 32, IRR_HEIGHT = 32, PRE_WIDTH = 128, PRE_HEIGHT = 128;
GLfloat deltatime = 0.0f, lastframe = 0.0f, currentframe = 0.0f;
GLfloat lastxPos = SCR_WIDTH / 2.0f, lastyPos = SCR_HEIGHT / 2.0f;
GLfloat aspect = (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT;
GLboolean firstMouse = GL_TRUE;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLuint sphereVAO, sphereVBO, sphereEBO, sphereIndex, cubeVAO, cubeVBO, quadVAO, quadVBO;

GLint main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Phsical Based Shading", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to Initialize Window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to Initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);

	Shader PBR("pbrVS.shader", "pbrFS.shader");
	Shader EquiToCubeMap("cubeHdrVS.shader", "cubeHdrFS.shader");
	Shader skyboxShader("skyboxVS.shader", "skyboxFS.shader");
	Shader IrradianceShader("irradianceVS.shader", "irradianceFS.shader");
	Shader preFilterShader("cubeHdrVS.shader", "prefilterFS.shader");
	Shader brdfShader("brdfVS.shader", "brdfFS.shader");

	GLuint equiMap = loadHdrTexture("D:/OpenGL/PBR/newport_loft.hdr");

	GLuint ironAlbedo = loadTexture("D:/OpenGL/PBR/RustedIron/albedo.png", GL_FALSE);
	GLuint ironNormal = loadTexture("D:/OpenGL/PBR/RustedIron/normal.png", GL_FALSE);
	GLuint ironMetallic = loadTexture("D:/OpenGL/PBR/RustedIron/metallic.png", GL_FALSE);
	GLuint ironRoughness = loadTexture("D:/OpenGL/PBR/RustedIron/roughness.png", GL_FALSE);
	GLuint ironAo = loadTexture("D:/OpenGL/PBR/RustedIron/ao.png", GL_FALSE);

	GLuint goldAlbedo = loadTexture("D:/OpenGL/PBR/Gold/albedo.png", GL_FALSE);
	GLuint goldNormal = loadTexture("D:/OpenGL/PBR/Gold/normal.png", GL_FALSE);
	GLuint goldMetallic = loadTexture("D:/OpenGL/PBR/Gold/metallic.png", GL_FALSE);
	GLuint goldRoughness = loadTexture("D:/OpenGL/PBR/Gold/roughness.png", GL_FALSE);
	GLuint goldAo = loadTexture("D:/OpenGL/PBR/Gold/ao.png", GL_FALSE);

	GLuint grassAlbedo = loadTexture("D:/OpenGL/PBR/Grass/albedo.png", GL_FALSE);
	GLuint grassNormal = loadTexture("D:/OpenGL/PBR/Grass/normal.png", GL_FALSE);
	GLuint grassMetallic = loadTexture("D:/OpenGL/PBR/Grass/metallic.png", GL_FALSE);
	GLuint grassRoughness = loadTexture("D:/OpenGL/PBR/Grass/roughness.png", GL_FALSE);
	GLuint grassAo = loadTexture("D:/OpenGL/PBR/Grass/ao.png", GL_FALSE);

	GLuint plasticAlbedo = loadTexture("D:/OpenGL/PBR/Plastic/albedo.png", GL_FALSE);
	GLuint plasticNormal = loadTexture("D:/OpenGL/PBR/Plastic/normal.png", GL_FALSE);
	GLuint plasticMetallic = loadTexture("D:/OpenGL/PBR/Plastic/metallic.png", GL_FALSE);
	GLuint plasticRoughness = loadTexture("D:/OpenGL/PBR/Plastic/roughness.png", GL_FALSE);
	GLuint plasticAo = loadTexture("D:/OpenGL/PBR/Plastic/ao.png", GL_FALSE);

	GLuint wallAlbedo = loadTexture("D:/OpenGL/PBR/Wall/albedo.png", GL_FALSE);
	GLuint wallNormal = loadTexture("D:/OpenGL/PBR/Wall/normal.png", GL_FALSE);
	GLuint wallMetallic = loadTexture("D:/OpenGL/PBR/Wall/metallic.png", GL_FALSE);
	GLuint wallRoughness = loadTexture("D:/OpenGL/PBR/Wall/roughness.png", GL_FALSE);
	GLuint wallAo = loadTexture("D:/OpenGL/PBR/Wall/ao.png", GL_FALSE);

	Sphere();
	setupCube();
	setupQuad();

	GLint nrRows = 7, nrColumns = 7;
	GLfloat spacing = 2.5f;
	GLuint maxMipLevels = 5;

	glm::vec3 lightPositions[] =
	{
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3( 10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3( 10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] =
	{
		glm::vec3(300.0f),
		glm::vec3(300.0f),
		glm::vec3(300.0f),
		glm::vec3(300.0f)
	};

	glm::mat4 captureProj = glm::mat4(1.0f); 
	captureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

	GLuint captureFBO, captureRBO, captureCubeMap, irradianceMap, prefilterMap, brdfLUTTexture;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, ENV_WIDTH, ENV_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glGenTextures(1, &captureCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubeMap);
	for (GLuint i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, ENV_WIDTH, ENV_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (GLuint i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, IRR_WIDTH, IRR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (GLuint i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, PRE_WIDTH, PRE_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glGenTextures(1, &brdfLUTTexture);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, ENV_WIDTH, ENV_HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Capture Frame Buffer Incomplete " << std::endl;

	EquiToCubeMap.Use();
	EquiToCubeMap.setInt("equilateralMap", 0);
	EquiToCubeMap.setMat4("projection", captureProj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, equiMap);

	glViewport(0, 0, ENV_WIDTH, ENV_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindVertexArray(cubeVAO);
	for (GLuint i = 0; i < 6; i++)
	{
		EquiToCubeMap.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, captureCubeMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubeMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, IRR_WIDTH, IRR_HEIGHT);

	IrradianceShader.Use();
	IrradianceShader.setInt("environmentMap", 0);
	IrradianceShader.setMat4("projection", captureProj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubeMap);

	glViewport(0, 0, IRR_WIDTH, IRR_HEIGHT);
	glBindVertexArray(cubeVAO);
	for (GLuint i = 0; i < 6; i++)
	{
		IrradianceShader.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);

	preFilterShader.Use();
	preFilterShader.setInt("environmentMap", 0);
	preFilterShader.setMat4("projection", captureProj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubeMap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO); 
	glBindVertexArray(cubeVAO);
	for (GLuint mip = 0; mip < maxMipLevels; mip++)
	{
		GLint mipWidth = PRE_WIDTH * std::pow(0.5, mip);
		GLint mipHeight = PRE_HEIGHT * std::pow(0.5, mip);
		
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		GLfloat roughness = (GLfloat)mip / (GLfloat)(maxMipLevels - 1);
		preFilterShader.setFloat("roughness", roughness);
		
		for (GLuint i = 0; i < 6; i++)
		{
			preFilterShader.setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, ENV_WIDTH, ENV_HEIGHT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, ENV_WIDTH, ENV_HEIGHT);
	brdfShader.Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	PBR.Use();
	PBR.setInt("IrradianceMap", 0);
	PBR.setInt("PrefilterMap", 1);
	PBR.setInt("brdfLUT", 2);
	PBR.setInt("albedoMap", 3);
	PBR.setInt("normalMap", 4);
	PBR.setInt("metallicMap", 5);
	PBR.setInt("roughnessMap", 6);
	PBR.setInt("aoMap", 7);
	PBR.setMat4("projection", projection);

	skyboxShader.Use();
	skyboxShader.setInt("environmentMap", 0);
	skyboxShader.setMat4("projection", projection);
	
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	while (!glfwWindowShouldClose(window))
	{
		currentframe = (GLfloat)glfwGetTime();
		deltatime = currentframe - lastframe;
		lastframe = currentframe;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		PBR.Use();
		glm::mat4 model = glm::mat4(1.0f);
		PBR.setMat4("view", camera.GetViewMatrix());
		PBR.setVec3("camPos", camera.Position);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

		//Rusted Iron
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ironAlbedo);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ironNormal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ironMetallic);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, ironRoughness);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, ironAo);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
		PBR.setMat4("model", model);
		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, sphereIndex, GL_UNSIGNED_INT, 0);

		//Gold
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, goldAlbedo);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, goldNormal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, goldMetallic);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, goldRoughness);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, goldAo);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
		PBR.setMat4("model", model);
		glDrawElements(GL_TRIANGLE_STRIP, sphereIndex, GL_UNSIGNED_INT, 0);

		//Grass
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, grassAlbedo);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, grassNormal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, grassMetallic);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, grassRoughness);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, grassAo);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
		PBR.setMat4("model", model);
		glDrawElements(GL_TRIANGLE_STRIP, sphereIndex, GL_UNSIGNED_INT, 0);

		//Plastic
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, plasticAlbedo);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, plasticNormal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, plasticMetallic);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, plasticRoughness);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, plasticAo);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
		PBR.setMat4("model", model);
		glDrawElements(GL_TRIANGLE_STRIP, sphereIndex, GL_UNSIGNED_INT, 0);

		//Wall
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, wallAlbedo);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, wallNormal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, wallMetallic);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, wallRoughness);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, wallAo);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
		PBR.setMat4("model", model);
		glDrawElements(GL_TRIANGLE_STRIP, sphereIndex, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		/*for (GLuint i = 0; i < 4; i++)
		{
			//lightPositions[i] = lightPositions[i] + glm::vec3(glm::sin(glfwGetTime() * 5.0f) * 5.0f, 0.0f, 0.0f);
			PBR.setVec3(("lightPos[" + std::to_string(i) + "]").c_str(), lightPositions[i]);
			PBR.setVec3(("lightCol[" + std::to_string(i) + "]").c_str(), lightColors[i]);

			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPositions[i]);
			model = glm::scale(model, glm::vec3(0.5f));
			PBR.setMat4("model", model);

			glDrawElements(GL_TRIANGLE_STRIP, sphereIndex, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);*/

		skyboxShader.Use();
		skyboxShader.setMat4("view", camera.GetViewMatrix());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, captureCubeMap);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &sphereVBO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &sphereEBO);
	glDeleteFramebuffers(1, &captureFBO);
	glDeleteRenderbuffers(1, &captureRBO);
	glDeleteTextures(1, &equiMap);
	glDeleteTextures(1, &captureCubeMap);
	glDeleteTextures(1, &irradianceMap);
	glDeleteTextures(1, &prefilterMap);
	glDeleteTextures(1, &brdfLUTTexture);
	glDeleteTextures(1, &ironAlbedo);
	glDeleteTextures(1, &ironNormal);
	glDeleteTextures(1, &ironMetallic);
	glDeleteTextures(1, &ironRoughness);
	glDeleteTextures(1, &ironAo);
	glDeleteTextures(1, &goldAlbedo);
	glDeleteTextures(1, &goldNormal);
	glDeleteTextures(1, &goldMetallic);
	glDeleteTextures(1, &goldRoughness);
	glDeleteTextures(1, &goldAo);
	glDeleteTextures(1, &grassAlbedo);
	glDeleteTextures(1, &grassNormal);
	glDeleteTextures(1, &grassMetallic);
	glDeleteTextures(1, &grassRoughness);
	glDeleteTextures(1, &grassAo);
	glDeleteTextures(1, &plasticAlbedo);
	glDeleteTextures(1, &plasticNormal);
	glDeleteTextures(1, &plasticMetallic);
	glDeleteTextures(1, &plasticRoughness);
	glDeleteTextures(1, &plasticAo);
	glDeleteTextures(1, &wallAlbedo);
	glDeleteTextures(1, &wallNormal);
	glDeleteTextures(1, &wallMetallic);
	glDeleteTextures(1, &wallRoughness);
	glDeleteTextures(1, &wallAo);
	PBR.Delete();
	EquiToCubeMap.Delete();
	skyboxShader.Delete();
	IrradianceShader.Delete();
	preFilterShader.Delete();
	brdfShader.Delete();
	glfwTerminate();
	return 0;
}

GLvoid Sphere()
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<GLuint> indices;

	const GLuint X_SEGMENTS = 64;
	const GLuint Y_SEGMENTS = 64;
	const GLfloat PI = 3.14159265359f;
	for (GLuint y = 0; y <= Y_SEGMENTS; y++)
	{
		for (GLuint x = 0; x <= X_SEGMENTS; x++)
		{
			GLfloat xSegment = (GLfloat)x / (GLfloat)X_SEGMENTS;
			GLfloat ySegment = (GLfloat)y / (GLfloat)Y_SEGMENTS;
			GLfloat xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			GLfloat yPos = std::cos(ySegment * PI);
			GLfloat zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			uv.push_back(glm::vec2(xSegment, ySegment));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	GLboolean oddRow = GL_FALSE;
	for (GLint y = 0; y < Y_SEGMENTS; y++)
	{
		if (!oddRow) 
		{
			for (GLint x = 0; x <= X_SEGMENTS; x++)
			{
				indices.push_back(y       * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (GLint x = X_SEGMENTS; x >= 0; x--)
			{
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y       * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	sphereIndex = indices.size();

	std::vector<GLfloat> data;
	for (GLuint i = 0; i < positions.size(); i++)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);

		if (normals.size() > 0)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}

		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
	}

	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glGenBuffers(1, &sphereEBO);
	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	GLsizei stride = 8 * sizeof(GLfloat);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLvoid setupCube()
{
	GLfloat cubeVertices[] =
	{
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLvoid setupQuad()
{
	GLfloat quadVertices[] =
	{
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLvoid framebuffer_size_callback(GLFWwindow* window, GLint width, GLint height)
{
	glViewport(0, 0, width, height);
}

GLvoid processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltatime);
}

GLvoid mouse_callback(GLFWwindow* window, GLdouble xPos, GLdouble yPos)
{
	if (firstMouse)
	{
		xPos = lastxPos;
		yPos = lastyPos;
		firstMouse = GL_FALSE;
	}

	GLfloat xoffset = xPos - lastxPos;
	GLfloat yoffset = lastyPos - yPos;

	lastxPos = xPos;
	lastyPos = yPos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

GLvoid scroll_callback(GLFWwindow* window, GLdouble xoffset, GLdouble yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

GLuint loadTexture(const GLchar* path, GLboolean gammaCorrection)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	GLint width, height, nrChannels;
	GLubyte* data = stbi_load(path, &width, &height, &nrChannels, NULL);
	if (data)
	{
		GLenum dataFormat{}, internalFormat{};
		if (nrChannels == 1)
			internalFormat = dataFormat = GL_RED;
		else if (nrChannels == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrChannels == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, NULL, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindBuffer(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
	}
	else
	{
		cout << "Texture is Dead!!! " << endl;
		stbi_image_free(data);
	}
	return textureID;
}

GLuint loadHdrTexture(const GLchar* path)
{
	GLuint HdrtextureID{};

	stbi_set_flip_vertically_on_load(GL_TRUE);

	GLint width, height, nrComponents;
	GLfloat* data = stbi_loadf(path, &width, &height, &nrComponents, 0);
	
	if (data)
	{
		glGenTextures(1, &HdrtextureID);
		glBindTexture(GL_TEXTURE_2D, HdrtextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to Load HDR Image " << std::endl;
		stbi_image_free(data);
	}

	return HdrtextureID;
}