#pragma once
#include <functional>
#include <list>
#include "font.hpp"
#include "guiElements.hpp"
#include "technique/fontTechnique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		struct Score
		{
			std::string name;
			int score;
		};

		class Gui
		{
		public:
			inline Gui() : framebufferWidth(0), framebufferHeight(0), pHoveredButton(nullptr), pActiveInput(nullptr) {}
			inline ~Gui() { deinit(); }
			bool init();
			inline void deinit() { font.deinit(); fontPass.deinit(); }
			inline void setExitCallback(std::function<void()> const& callback) { exit = callback; }
			inline void setHighscoresCallback(std::function<std::list<Score>()> const& callback) { getHighscores = callback; }
			inline void setScoreCallback(std::function<unsigned int()> const& callback) { getScore = callback; }
			void setSize(GLsizei framebufferWidth, GLsizei framebufferHeight, float fovy, float zPlane);

			void render(glm::mat4 projectionMatrix) const;

			void clear();
			void loadMenu();
			void loadScore();
			void loadHighscore();
			void loadEntry();

			void updateMousePosition(float x, float y);
			void click();
			bool inputCodePoint(unsigned int codePoint);

		private:
			Label& createLabel(char const* text, float fontSize, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), glm::vec3 const& color = glm::vec3(1, 1, 1));
			Label& createLabel(std::string const& text, float fontSize, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), glm::vec3 const& color = glm::vec3(1, 1, 1)) { return createLabel(text.c_str(), fontSize, anchor, offset, color); }
			Label& createLabel(std::function<std::string()> getText, float fontSize, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), glm::vec3 const& color = glm::vec3(1, 1, 1));
			void createInput(float fontSize, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), float width = 50, glm::vec3 const& color = glm::vec3(1, 1, 1));
			void createButton(std::string const& text, float fontSize, std::function<void()> onClick, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), glm::vec3 const& color = glm::vec3(1, 1, 1));
			void setPosition(Label& label);
			void setPositions();
			glm::vec2 render(Label const& label, glm::mat4 const& projectionMatrix) const;
			void render(Button const& button, glm::mat4 const& projectionMatrix) const;
			void render(Input const& input, glm::mat4 const& projectionMatrix) const;

			Font font;
			float framebufferWidth;
			float framebufferHeight;
			glm::mat4 guiProjection;
			std::list<Label> labels;
			std::list<Button> buttons;
			std::list<Input> inputs;
			Button* pHoveredButton;
			Input* pActiveInput;

			FontTechnique fontPass;

			// Callbacks.
			std::function<unsigned int()> getScore;
			std::function<std::list<Score>()> getHighscores;
			std::function<void()> exit;

			static float const normalFontSize;
			static float const titleFontSize;
		};

	}
}