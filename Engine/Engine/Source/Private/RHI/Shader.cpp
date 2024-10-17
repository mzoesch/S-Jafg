// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "RHI/Shader.h"
#include <filesystem>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#if PLATFORM_WINDOWS
	#include <Windows.h>
#endif /* PLATFORM_WINDOWS */

Jafg::Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::string ExecPath(PLATFORM_MAX_PATH, '\0');
		GetModuleFileNameA(NULL, &ExecPath[0], ExecPath.size());
		ExecPath = ExecPath.substr(0, ExecPath.find_last_of('\\'));

		std::cout << "Opening vertex shader file: " << ExecPath + '/' + vertexPath << '\n';
		std::cout << "Opening fragment shader file: " << ExecPath + '/' + fragmentPath << '\n';

		// open files
		vShaderFile.open(ExecPath + '/' + vertexPath);
		fShaderFile.open(ExecPath + '/' + fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (const std::ifstream::failure& E)
	{
		std::cout << "Error reading shader source files.\n"
			<< "Vertex path: " << vertexPath << '\n'
			<< "Fragment path: " << fragmentPath << '\n'
			<< E.what() << '\n'
			<< std::filesystem::current_path() << '\n';
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Error compiling vertex shader!\n" << infoLog << '\n';
	}

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Error compiling fragment shader!\n" << infoLog << '\n';
	}

	// shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Error linking shader program!\n" << infoLog << '\n';
	}

	// delete the shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Jafg::Shader::Use()
{
	glUseProgram(ID);
}

void Jafg::Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Jafg::Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Jafg::Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}