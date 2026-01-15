//
//� Objects.cpp
//� OpenGL Advances Lighting
//
#include "Objects.hpp"
#include <iostream>
#include <algorithm>�
#include <random>�
#include <ctime>��
#include <glm/gtc/matrix_inverse.hpp>�
#include <GLFW/glfw3.h>
namespace gps {

	Scene::Scene() {
		std::srand(static_cast<unsigned int>(std::time(0)));

		std::vector<glm::vec3> manualPositions = {
			glm::vec3(-7.0f, -1.0f, -8.0f),
			glm::vec3(10.0f, -1.0f, -12.0f),
			glm::vec3(-4.0f, -1.0f, -20.0f),
			glm::vec3(12.0f, -1.0f, -4.0f),
			glm::vec3(0.0f, -1.0f, -24.0f),
			glm::vec3(12.0f, -1.0f, -4.0f),
			glm::vec3(10.0f, -1.0f, 12.0f)
		};

		for (const auto& pos : manualPositions) {
			forestInstances.push_back({ pos, 0, 5.0f, 0.0f });
		}

		// This generates some random trees
		int numberOfTrees = 200;
		float range = 90.0f;

		for (int i = 0; i < numberOfTrees; i++) {
			float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (range * 2)) - range;
			float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (range * 2)) - range;

			// Clear the center spawn area
			if (abs(x) < 15.0f && abs(z) < 15.0f) continue;

			TreeInstance tree;
			tree.position = glm::vec3(x, -1.0f, z);
			tree.typeIndex = rand() % 3; // We have 3 model trees
			tree.scale = 4.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 3.0f));
			tree.rotationY = static_cast<float>(rand() % 360);

			forestInstances.push_back(tree);
		}
	}

	void Scene::loadModels() {
		nanosuit.LoadModel("objects/nanosuit/nanosuit.obj");
		mario.LoadModel("objects/mario/Mario.obj");
		creeper.LoadModel("objects/creeper/creeper.obj");
		zombie.LoadModel("objects/zombie/Zombie.obj");
		ground.LoadModel("objects/ground/ground.obj");
		lightCube.LoadModel("objects/cube/cube.obj");
		bonfire.LoadModel("objects/bonfire/Bonfire.obj");
		tent.LoadModel("objects/tent/Tent.obj");

		Model3D* t0 = new Model3D();
		t0->LoadModel("objects/tree_quad/tree0.obj");
		treeModels.push_back(t0);

		Model3D* t1 = new Model3D();
		t1->LoadModel("objects/tree_quad/tree1.obj");
		treeModels.push_back(t1);

		Model3D* t2 = new Model3D();
		t2->LoadModel("objects/tree_quad/tree2.obj");
		treeModels.push_back(t2);

		initCollision();
	}

	// Helper to calculate AABB from model matrix
	BoundingBox Scene::calculateAABB(const glm::mat4& modelMatrix, glm::vec3 localMin, glm::vec3 localMax) {
		std::vector<glm::vec3> corners = {
			{localMin.x, localMin.y, localMin.z}, {localMin.x, localMin.y, localMax.z},
			{localMin.x, localMax.y, localMin.z}, {localMin.x, localMax.y, localMax.z},
			{localMax.x, localMin.y, localMin.z}, {localMax.x, localMin.y, localMax.z},
			{localMax.x, localMax.y, localMin.z}, {localMax.x, localMax.y, localMax.z}
		};

		glm::vec3 newMin(1e9);
		glm::vec3 newMax(-1e9);

		for (const auto& corner : corners) {
			glm::vec4 worldPos = modelMatrix * glm::vec4(corner, 1.0f);
			newMin = glm::min(newMin, glm::vec3(worldPos));
			newMax = glm::max(newMax, glm::vec3(worldPos));
		}

		newMax.y = 20.0f;
		return { newMin, newMax };
	}

	void Scene::initCollision() {
		glm::mat4 model;
		glm::vec3 localMin(-1.0f, 0.0f, -1.0f);
		glm::vec3 localMax(1.0f, 2.0f, 1.0f);

		//TREES
		for (const auto& tree : forestInstances) {
			model = glm::translate(glm::mat4(1.0f), tree.position);
			model = glm::scale(model, glm::vec3(tree.scale));
			collisionBoxes.push_back(calculateAABB(model, glm::vec3(-0.2f, 0.0f, -0.2f), glm::vec3(0.2f, 2.0f, 0.2f)));
		}

		// ZOMBIE
		model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::translate(model, glm::vec3(15.0f, -2.0f, 5.0f));
		collisionBoxes.push_back(calculateAABB(model, localMin, localMax));

		//Bonfire
		model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f));
		model = glm::translate(model, glm::vec3(-5.0f, -1.5f, 0.0f));
		collisionBoxes.push_back(calculateAABB(model, localMin, localMax));

		//MARIO
		model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::translate(model, glm::vec3(-6.0f, -2.0f, 4.0f));
		collisionBoxes.push_back(calculateAABB(model, localMin, localMax));

		// tent
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-6.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f));
		collisionBoxes.push_back(calculateAABB(model, glm::vec3(-12.f, -2.f, -12.f), glm::vec3(12.f, 22.f, 12.f)));

		// CREEPER
		model = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f));
		model = glm::translate(model, glm::vec3(-100.0f, -20.0f, -100.0f));
		collisionBoxes.push_back(calculateAABB(model, glm::vec3(-10.f, 0.f, -10.f), glm::vec3(10.f, 40.f, 10.f)));

		// NANOSUIT
		model = glm::mat4(1.0f);
		collisionBoxes.push_back(calculateAABB(model, localMin, localMax));
	}

	bool Scene::checkCollision(const glm::vec3& position) {
		for (const auto& box : collisionBoxes) {
			if (box.contains(position)) return true;
		}
		if (position.y < -0.5f) return true; // Ground check
		return false;
	}

	void Scene::setNormalMatrix(Shader& shader, const glm::mat4& model, const glm::mat4& view) {
		glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	void Scene::draw(Shader& shader, const glm::mat4& view, float angleY) {
		shader.useShaderProgram();
		GLuint repeatLoc = glGetUniformLocation(shader.shaderProgram, "textureRepeat");
		GLuint hasAlphaLoc = glGetUniformLocation(shader.shaderProgram, "hasAlpha");
		GLuint modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
		glm::mat4 model;

		// STANDARD OBJECTS

		// CREEPER
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f));
		model = glm::translate(model, glm::vec3(-100.0f, -20.0f, -100.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		setNormalMatrix(shader, model, view);
		glUniform1f(repeatLoc, 1.0f);
		glUniform1f(hasAlphaLoc, 0.0f);
		creeper.Draw(shader);

		// NANOSUIT
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(180.0f + angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		setNormalMatrix(shader, model, view); glUniform1f(repeatLoc, 1.0f);
		glUniform1f(hasAlphaLoc, 0.0f);
		nanosuit.Draw(shader);

		// ZOMBIE
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::translate(model, glm::vec3(15.0f, -2.0f, 5.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		setNormalMatrix(shader, model, view); glUniform1f(repeatLoc, 1.0f);
		glUniform1f(hasAlphaLoc, 1.0f);
		zombie.Draw(shader);

		// Bonfire
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f));
		model = glm::translate(model, glm::vec3(-5.0f, -0.9f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		setNormalMatrix(shader, model, view); glUniform1f(repeatLoc, 1.0f);
		glUniform1f(hasAlphaLoc, 0.0f);
		bonfire.Draw(shader);

		// Tent
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-6.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.08f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		setNormalMatrix(shader, model, view); glUniform1f(repeatLoc, 1.0f);
		glUniform1f(hasAlphaLoc, 0.0f);
		tent.Draw(shader);

		// MARIO with time-based animation
		float timeValue = (float)glfwGetTime();

		model = glm::mat4(1.0f);

		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::translate(model, glm::vec3(-6.0f, -2.0f, 4.0f));
		model = glm::rotate(model, glm::radians(90.0f) + timeValue, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		setNormalMatrix(shader, model, view);
		glUniform1f(repeatLoc, 1.0f);
		glUniform1f(hasAlphaLoc, 0.0f);
		mario.Draw(shader);

		// GROUND
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); setNormalMatrix(shader, model, view);
		glUniform1f(repeatLoc, 500.0f); glUniform1f(hasAlphaLoc, 0.0f);
		ground.Draw(shader);


		// DRAW TREES USING BILLBOARING, with 3 faces, each rotated by 60 degrees
		glUniform1f(repeatLoc, 1.0f);
		glUniform1f(hasAlphaLoc, 1.0f);
		glDisable(GL_CULL_FACE); // Important for plane visibility

		if (!treeModels.empty()) {
			for (const auto& tree : forestInstances) {
				int idx = tree.typeIndex;
				if (idx < 0 || idx >= treeModels.size()) idx = 0;

				// Draw 3 planes, rotated 60 degrees apart
				for (int i = 0; i < 3; i++) {
					model = glm::translate(glm::mat4(1.0f), tree.position);

					// Add base rotation + 60 degree increments
					float finalRot = tree.rotationY + (i * 60.0f);

					model = glm::rotate(model, glm::radians(finalRot), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(tree.scale));

					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
					setNormalMatrix(shader, model, view);

					treeModels[idx]->Draw(shader);
				}
			}
		}
		glEnable(GL_CULL_FACE);
	}

	void Scene::drawLightCube(Shader& shader, const glm::mat4& view, const glm::mat4& lightRotation, const glm::vec3& lightDir) {
		shader.useShaderProgram();
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		lightCube.Draw(shader);
	}
}