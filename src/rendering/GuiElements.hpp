#pragma once
#include "glm/glm.hpp"
#include <vector>
#include <functional>

namespace cgvkp
{
	namespace rendering
	{
		enum Anchor
		{
			center	= 0x00,
			left	= 0x01,
			right	= 0x02,
			top		= 0x04,
			bottom	= 0x08
		};
		inline Anchor operator|(Anchor const& lhs, Anchor const& rhs) { return static_cast<Anchor>(static_cast<int>(lhs) | static_cast<int>(rhs)); }

		struct Label
		{
		public:
			glm::vec2 position;
			glm::vec2 offset;
			glm::vec3 color;
			glm::vec2 size;
			std::string text;
			float fontSize;
			Anchor anchor; // left middle right, top middle bottom
			std::function<std::string()> getText;

			inline bool within(float x, float y) { return x >= position.x && x <= position.x + size.x && y >= position.y && y <= position.y + size.y; }
		};

		struct Button : public Label
		{
		public:
			std::function<void()> onClick;
		};

		struct Input : public Button
		{

		};

	}
}
