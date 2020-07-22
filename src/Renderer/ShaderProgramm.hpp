#pragma once

#include <glad/glad.h>
#include <string>

namespace Renderer {
	class ShaderProgramm {
	public:
		ShaderProgramm(const std::string& vertexShader, const std::string& fragmentShader);
		~ShaderProgramm();
		bool isCompiled() const { return m_isCompiled; }
		void use() const;

		ShaderProgramm() = delete;
		ShaderProgramm(ShaderProgramm&) = delete;
		ShaderProgramm& operator=(const ShaderProgramm&) = delete;
		ShaderProgramm& operator=(ShaderProgramm&& shaderProgram) noexcept;
		ShaderProgramm(ShaderProgramm&& shaderProgram) noexcept;

	private:
		bool createShader(const std::string& source, const GLenum shaderType, GLuint& shaderID);

		bool m_isCompiled = false;
		GLuint m_ID = NULL;
	};
}