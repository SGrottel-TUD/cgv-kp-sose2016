#pragma once
#include "GuiElements.hpp"
#include "Font.hpp"
#include <iostream>
#include <list>
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
			Gui();
			~Gui();
			bool init(/*float fontSize*/);
			void deinit();
			void setSize(float fovy, GLsizei framebufferWidth, GLsizei framebufferHeight, float zPlane);
			void updateMousePosition(float x, float y);
			inline void click() { if (hoveredButton && hoveredButton->onClick) { auto f = hoveredButton->onClick; f(); } }
			void render(glm::mat4 const& viewProjectionMatrix) const;
			void clear();
			void loadMenu();
			void loadScore();
			void loadHighscore();
			void loadEntry();
			inline void setScoreCallback(std::function<unsigned int()> callback) { getScore = callback; }
			inline void setHighscoresCallback(std::function<std::list<Score>()> callback) { getHighscores = callback; }

		private:
			Label& createLabel(char const* text, float fontSize, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), glm::vec3 const& color = glm::vec3(1, 1, 1));
			Label& createLabel(std::string const& text, float fontSize, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), glm::vec3 const& color = glm::vec3(1, 1, 1)) { return createLabel(text.c_str(), fontSize, anchor, offset, color); }
			Label& createLabel(std::function<std::string()> getText, float fontSize, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), glm::vec3 const& color = glm::vec3(1, 1, 1));
			void createInput(float fontSize, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), float width = 50, glm::vec3 const& color = glm::vec3(1, 1, 1));
			void createButton(std::string const& text, float fontSize, std::function<void()> onClick, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), glm::vec3 const& color = glm::vec3(1, 1, 1));
			void setPosition(Label& label);
			void setPositions();
			glm::vec2 render(Label const& label, glm::mat4 const& viewProjectionMatrix) const;
			void render(Button const& button, glm::mat4 const& viewProjectionMatrix) const;
			void render(Input const& input, glm::mat4 const& viewProjectionMatrix) const;

			Font font;
			float width;
			float height;
			GLsizei framebufferWidth;
			GLsizei framebufferHeight;
			float internExternRatio;
			float fontSize;
			std::list<Label> labels;
			std::list<Button> buttons;
			std::list<Input> inputs;
			Button* hoveredButton;

			cgvkp::rendering::FontTechnique fontPass;

			//callbacks
			std::function<unsigned int()> getScore;
			std::function<std::list<Score>()> getHighscores;
		};

	}
}