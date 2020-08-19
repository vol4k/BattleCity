#include "Game.hpp"

#include "../Renderer/ShaderProgram.hpp"
#include "../Resources/ResourceManager.hpp"
#include "../Renderer/Texture2D.hpp"
#include "../Renderer/Sprite.hpp"
#include "../Renderer/AnimatedSprite.hpp"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Game::Game(const glm::vec2& windowSize): 
	m_eCurrentGameState(EGameState::Active), 
	m_windowSize(windowSize)
{
	m_keys.fill(false);
}

Game::~Game() 
{

}

void Game::render() 
{
	ResourceManager::getAnimatedSprite("NewAnimatedSprite")->render();
}

void Game::update(const uint64_t delta) 
{
	ResourceManager::getAnimatedSprite("NewAnimatedSprite")->update(delta);
}

void Game::setKey(const int key, const int action) 
{
	m_keys[key] = action;
}

bool Game::init() 
{
	auto pDefaultShaderProgram = ResourceManager::loadShaders("DefaultShader", "res/Shaders/vertex.txt", "res/Shaders/fragment.txt");
	if (!pDefaultShaderProgram) {
		std::cerr << "Can't create shader program: " << "DefaultShader" << std::endl;
		return false;
	}

	auto pSpriteShaderProgram = ResourceManager::loadShaders("SpriteShader", "res/Shaders/vSprite.txt", "res/Shaders/fSprite.txt");
	if (!pSpriteShaderProgram) {
		std::cerr << "Can't create shader program: " << "SpriteShader" << std::endl;
		return false;
	}

	auto tex = ResourceManager::loadTexture("DefaultTexture", "res/Textures/map_16x16.png");

	std::vector<std::string> subTexturesNames = {
		"block",
		"topLeftBlock",
		"topRightBlock",
		"topBlock",
		"bottomLeftBlock",
		"leftBlock",

		"topRightBottomLeftBlock",
		"topBottomLeftBlock",
		"bottomRightBlock",
		"topLeftBottomRightBlock",
		"rightBlock",
		"topBottomRightBlock",

		"bottomBlock",
		"topLeftBottomBlock",
		"topRightBottomBlock",
		"water1",
		"water2",
		"water3",

		"rock",
		"leaf",
		"ice",
		"wall",
		"respawn",
		"nothing",
	};
	auto pTextureAtlas = ResourceManager::loadTextureAtlas("DefaultTextureAtlas", "res/Textures/map_8x8.png", std::move(subTexturesNames), 8, 8);

	auto pSprite = ResourceManager::loadSprite("NewSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "topBottomLeftBlock");
	pSprite->setPosition(glm::vec2(300, 100));

	auto pAnimatedSprite = ResourceManager::loadAnimatedSprite("NewAnimatedSprite", "DefaultTextureAtlas", "SpriteShader", 50, 50);

	std::vector<std::pair<std::string, uint64_t>> waterState;
	waterState.emplace_back(std::pair<std::string, uint64_t>("water1", 5e8));
	waterState.emplace_back(std::pair<std::string, uint64_t>("water2", 5e8));
	waterState.emplace_back(std::pair<std::string, uint64_t>("water3", 5e8));


	pAnimatedSprite->insertState("waterState", std::move(waterState));

	pAnimatedSprite->setState("waterState");
	pAnimatedSprite->setPosition(glm::vec2(300, 300));

	pDefaultShaderProgram->use();
	pDefaultShaderProgram->setInt("tex", 0);

	glm::mat4 modelMatrix_1 = glm::mat4(1.f);
	modelMatrix_1 = glm::translate(modelMatrix_1, glm::vec3(100.f, 200.f, 0.f));

	glm::mat4 modelMatrix_2 = glm::mat4(1.f);
	modelMatrix_2 = glm::translate(modelMatrix_2, glm::vec3(590.f, 200.f, 0.f));

	glm::mat4 projectionMatrix = glm::ortho(0.f,
											static_cast<float>(m_windowSize.x),
											0.f,
											static_cast<float>(m_windowSize.y),
											-100.f,
											100.f);

	pDefaultShaderProgram->setMatrix4("projectionMat", projectionMatrix);

	pSpriteShaderProgram->use();
	pSpriteShaderProgram->setInt("tex", 0);
	pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

	return true;
}