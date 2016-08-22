#pragma once

#include <functional>
#include <list>
#include "font.hpp"
#include "guiElements.hpp"
#include "technique/fontTechnique.hpp"
#include "data/world.hpp"

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
			inline Gui(::cgvkp::data::world & data, char const* pFontFilename) : data(data), font(pFontFilename), framebufferWidth(0), framebufferHeight(0), pHoveredButton(nullptr), pActiveInput(nullptr) {}
			inline ~Gui() { deinit(); }
			bool init();
			inline void deinit() { font.deinit(); fontPass.deinit(); }
			inline void setExitCallback(std::function<void()> const& callback) { exit = callback; }
			inline void setHighscoresCallback(std::function<std::list<Score>()> const& callback) { getHighscores = callback; }
			inline void setScoreCallback(std::function<unsigned int()> const& callback) { getScore = callback; }
			inline void setFPSCallback(std::function<int()> const& callback) { getFPS = callback; }
			void setSize(GLsizei framebufferWidth, GLsizei framebufferHeight, float fovy, float zPlane);

			void render(glm::mat4 projectionMatrix) const;

			void clear();
			void loadMenu();
			void loadScore();
			void loadHighscore();
			void loadEntry();
			void loadOptions();

			void updateMousePosition(float x, float y);
			void click();
			bool inputCodePoint(unsigned int codePoint);

		private:
			Label& createLabel(char const* pText, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), float fontSize = normalFontSize, glm::vec3 const& color = glm::vec3(1, 1, 1));
			Label& createLabel(std::string const& text, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), float fontSize = normalFontSize, glm::vec3 const& color = glm::vec3(1, 1, 1)) { return createLabel(text.c_str(), anchor, offset, fontSize, color); }
			Label& createLabel(std::function<std::string()> getText, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), float fontSize = normalFontSize, glm::vec3 const& color = glm::vec3(1, 1, 1));
			Input& createInput(Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), float fontSize = normalFontSize, float width = 50, glm::vec3 const& color = glm::vec3(1, 1, 1));
			Button& createButton(char const* pText, std::function<void()> onClick, Anchor anchor = center, glm::vec2 const& offset = glm::vec2(0, 0), float fontSize = normalFontSize, glm::vec3 const& color = glm::vec3(1, 1, 1));
			void setPosition(Label& label);
			void setPositions();
			glm::vec2 render(Label const& label, glm::mat4 const& projectionMatrix) const;
			void render(Button const& button, glm::mat4 const& projectionMatrix) const;
			void render(Input const& input, glm::mat4 const& projectionMatrix) const;

            ::cgvkp::data::world & data;

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
			std::function<int()> getFPS;
			std::function<unsigned int()> getScore;
			std::function<std::list<Score>()> getHighscores;
			std::function<void()> exit;

			static float const normalFontSize;
			static float const titleFontSize;
		};

	}
}