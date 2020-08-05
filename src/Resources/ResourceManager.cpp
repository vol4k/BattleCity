#include "ResourceManager.hpp"
#include "../Renderer/ShaderProgram.hpp"
#include "../Renderer/Texture2D.hpp"
#include "../Renderer/Sprite.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBI_ONLY_PNG

ResourceManager::ResourceManager(const std::string executablePath) {
	size_t found = executablePath.find_last_of("/\\");
	m_path = executablePath.substr(0, found);
}

std::string ResourceManager::getFileString(const std::string& relativeFilePath) const {
	std::ifstream f;
	f.open(m_path + "/" + relativeFilePath.c_str(), std::ios::in | std::ios::binary);
	if (!f.is_open()) {
		std::cerr << "Faild to open file: " << relativeFilePath << std::endl;
		return std::string{};
	}

	std::stringstream buffer;
	buffer << f.rdbuf();
	return buffer.str();
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::loadShaders(
	const std::string& shaderName, 
	const std::string& vertexPatch, 
	const std::string& fragmentPatch
){
	std::string vertexString = getFileString(vertexPatch);
	if (vertexString.empty()) {
		std::cerr << "No vertex shader!" << std::endl;
		return nullptr;
	}

	std::string fragmentString = getFileString(fragmentPatch);
	if (fragmentString.empty()) {
		std::cerr << "No fragment shader!" << std::endl;
		return nullptr;
	}

	std::shared_ptr<Renderer::ShaderProgram>& newShader = m_shaderPrograms.emplace(shaderName, 
	std::make_shared<Renderer::ShaderProgram>(vertexString, 																									fragmentString)).first->second;
	if (newShader->isCompiled()) {
		return newShader;
	}
	
	std::cerr << "Can't load shader program:\n"
		<< "Vertex: " << vertexPatch << "\n"
		<< "Fragment: " << fragmentPatch << std::endl;

	return nullptr;
}

std::shared_ptr<Renderer::ShaderProgram> ResourceManager::getShaderProgram(const std::string& shaderName) {
	ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName);
	if (it != m_shaderPrograms.end()) {
		return it->second;
	}
	std::cerr << "Can't find the shader program: " << shaderName << std::endl;
	return nullptr;
}

std::shared_ptr<Renderer::Texture2D> ResourceManager::loadTexture(const std::string& textureName, const std::string& texturePath) {
	int channels	= NULL;
	int width		= NULL;
	int height		= NULL;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* pixels = stbi_load(std::string(m_path + "/" + texturePath).c_str(), &width, &height, &channels, NULL);

	if (!pixels) {
		std::cerr << "Can't load image: " << texturePath << std::endl;
		return nullptr;
	}

	std::shared_ptr<Renderer::Texture2D> newTexture = m_textures.emplace(textureName, 
																		 std::make_shared<Renderer::Texture2D>(width, 
																											   height, 
																											   pixels, 
																											   channels, 
																											   GL_NEAREST,
																											   GL_CLAMP_TO_EDGE)).first->second;

	stbi_image_free(pixels);

	return newTexture;
}

std::shared_ptr<Renderer::Texture2D> ResourceManager::getTexture(const std::string& textureName) {
	TexturesMap::const_iterator it = m_textures.find(textureName);
	if (it != m_textures.end()) {
		return it->second;
	}
	std::cerr << "Can't find the texture: " << textureName << std::endl;
	return nullptr;
}

std::shared_ptr<Renderer::Sprite> ResourceManager::loadSprite(const std::string& spriteName,
															  const std::string& textureName,
															  const std::string& shaderName,
															  const unsigned int spriteWidth,
															  const unsigned int spriteHeight,
															  const std::string subTextureName)
{
	auto pTexture = getTexture(textureName);
	if (!pTexture)
	{
		std::cerr << "Can't find the texture: " << textureName << " for the sprite: " << spriteName << std::endl;
		return nullptr;
	}

	auto pShader = getShaderProgram(shaderName);
	if (!pShader)
	{
		std::cerr << "Can't find the shader: " << shaderName << " for the sprite: " << spriteName << std::endl;
		return nullptr;
	}

	std::shared_ptr<Renderer::Sprite> newSprite = m_sprites.emplace(textureName,
																	std::make_shared<Renderer::Sprite>(pTexture,
																									   subTextureName,
																									   pShader,
																									   glm::vec2(0.f, 0.f),
																									   glm::vec2(spriteWidth,
																												 spriteHeight))).first->second;

	return newSprite;
}

std::shared_ptr<Renderer::Sprite> ResourceManager::getSprite(const std::string& spriteName)
{
	SpritesMap::const_iterator it = m_sprites.find(spriteName);
	if (it != m_sprites.end()) {
		return it->second;
	}
	std::cerr << "Can't find the sprite: " << spriteName << std::endl;
	return nullptr;
}


std::shared_ptr<Renderer::Texture2D> ResourceManager::loadTextureAtlas(const std::string textureName,
																	   const std::string texturePath,
																	   const std::vector<std::string> subTextures,
																	   const unsigned int subTextureWidth,
																	   const unsigned int subTextureHeight)
{
	auto pTexture = loadTexture(std::move(textureName), std::move(texturePath));
	if (pTexture)
	{
		const unsigned int textureWidth = pTexture->width();
		const unsigned int textureHeight = pTexture->height();

		unsigned int currentTextureOffsetX = 0;
		unsigned int currentTextureOffsetY = textureHeight;

		for (const auto& currentSubTextureName : subTextures)
		{
			glm::vec2 leftBottomUV(	static_cast<float>(currentTextureOffsetX) / textureWidth, 
									static_cast<float>(currentTextureOffsetY - subTextureHeight) / textureHeight);
			
			glm::vec2 rightTopUV(	static_cast<float>(currentTextureOffsetX + subTextureWidth) / textureWidth, 
									static_cast<float>(currentTextureOffsetY) / textureHeight);

			pTexture->addSubTexture(std::move(currentSubTextureName), leftBottomUV, rightTopUV);

			currentTextureOffsetX += subTextureWidth;
			if (currentTextureOffsetX >= textureWidth) 
			{
				currentTextureOffsetY -= subTextureHeight;
				currentTextureOffsetX = 0;
			}
		}
	}
	return pTexture;
}