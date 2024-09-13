// Copyright mzoesch. All rights reserved.

#pragma once

#include <string>

class Shader
{
public:
	// the program ID
	unsigned int ID;

	Shader() = default;
	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void Use();
	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
};