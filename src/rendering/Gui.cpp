#include "gui.hpp"
#include <glfw/glfw3.h>
#include <glm/gtx/transform.hpp>

float const cgvkp::rendering::Gui::normalFontSize = 20.0f;
float const cgvkp::rendering::Gui::titleFontSize = 30.0f;

bool cgvkp::rendering::Gui::init()
{
	if (!font.init("fonts/CartoonRegular.ttf", ' ', '~'))
	{
		return false;
	}

	if (!fontPass.init())
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::Gui::clear()
{
	pHoveredButton = nullptr;
	pActiveInput = nullptr;
	
	labels.clear();
	buttons.clear();
	inputs.clear();
}

void cgvkp::rendering::Gui::loadMenu()
{
	clear();

	createButton("Spielen", normalFontSize, std::bind(&Gui::loadScore, this), top);
	createButton("Highscore", normalFontSize, std::bind(&Gui::loadHighscore, this), center);
	createButton("Beenden", normalFontSize, exit, left | bottom);

	setPositions();
}

void cgvkp::rendering::Gui::loadScore()
{
	clear();
	//star before score
	createLabel([&]() { return std::to_string(getScore()); }, 10, right | top);

	setPositions();
}

void cgvkp::rendering::Gui::loadHighscore()
{
	clear();

	createLabel("Highscore", 12, top);

	float offsetX = 7;
	glm::vec2 offsetName = glm::vec2(offsetX, -3 * 12);
	glm::vec2 offsetNum = glm::vec2(-offsetX, -3 * 12);

	auto scores = getHighscores();
	for (auto const& s : scores)
	{
		Label& name = createLabel(s.name, normalFontSize, top | left, offsetName);
		Label& score = createLabel(std::to_string(s.score), normalFontSize,  top | right, offsetNum);
		
		// points
		float pointLeft = -framebufferWidth / 2 + offsetX + name.size.x;
		float pointRight = framebufferWidth / 2 - offsetX - score.size.x;
		std::string points = "";

		std::string str = " . ";
		float strWidth = font.getWidth(str, name.fontSize);
		for (float x = pointLeft; x < pointRight - strWidth; x += strWidth)
		{
			points += str;
		}
		createLabel(points, normalFontSize, top, glm::vec2((pointLeft + pointRight) / 2, offsetName.y));

		offsetName.y -= 5;
		offsetNum.y -= 5;
	}
	createButton("Back", 8, std::bind(&Gui::loadMenu, this), bottom);
	setPositions();
}

void cgvkp::rendering::Gui::loadEntry()
{
	clear();
	//float x = createLabel("Erreichte Punktzahl: ", normalFontSize, left, glm::vec2(0, 10)).size.x;
	//createLabel(std::to_string(getScore()), normalFontSize, left, glm::vec2(x, 10));
	//Label& label = createLabel("Gib deinen Namen ein: ", normalFontSize, left, glm::vec2(0, -10));
	//createInput(normalFontSize, left, glm::vec2(label.size.x, -10));
	//createButton("Weiter", normalFontSize, std::bind(&Gui::loadHighscore, this), bottom);
	createLabel("center", normalFontSize);
	createLabel("left", normalFontSize, left);
	setPositions();
}

void cgvkp::rendering::Gui::setSize(GLsizei width, GLsizei height, float fovy, float zPlane)
{
	framebufferWidth = static_cast<float>(width);
	framebufferHeight = static_cast<float>(height);

	float internExternRatio = 2 * tan(fovy / 2) * zPlane / framebufferHeight;
	guiProjection = glm::translate(glm::vec3(0, 0, -zPlane)) * glm::scale(glm::vec3(internExternRatio));

	setPositions();
}

void cgvkp::rendering::Gui::setPositions()
{
	for (auto& label : labels)
	{
		setPosition(label);
	}

	for (auto& input : inputs)
	{
		setPosition(input);
	}

	for (auto& button : buttons)
	{
		setPosition(button);
	}
}

void cgvkp::rendering::Gui::setPosition(Label& label)
{
	label.position = -0.5f * label.size;

	if (label.anchor & left)
	{
		label.position.x = -framebufferWidth / 2;
	}
	if (label.anchor & right)
	{
		label.position.x = framebufferWidth / 2 - label.size.x;
	}
	if (label.anchor & top)
	{
		label.position.y = framebufferHeight / 2 - label.size.y;
	}
	if (label.anchor & bottom)
	{
		label.position.y = -framebufferHeight / 2;
	}

	label.position += label.offset;
}

void cgvkp::rendering::Gui::render(glm::mat4 projectionMatrix) const
{
	fontPass.use();
	projectionMatrix *= guiProjection;
	for (auto& label : labels)
	{
		render(label, projectionMatrix);
	}
	for (auto& button : buttons)
	{
		render(button, projectionMatrix);
	}
	for (auto& input : inputs)
	{
		render(input, projectionMatrix);
	}
}

glm::vec2 cgvkp::rendering::Gui::render(Label const& label, glm::mat4 const& viewProjectionMatrix) const
{
	glm::vec3 position = glm::vec3(label.position, 0);
	std::string text;
	char const* str = label.text.c_str();

	if (label.getText)
	{
		text = label.getText();
		str = text.c_str();

		if (label.anchor & center)
		{
			position.x -= font.getWidth(str, label.fontSize) / 2;
		}
		else if(label.anchor & right)
		{
			position.x -= font.getWidth(str, label.fontSize);
		}
	}
	else
	{
		//Inputs
		while (font.getWidth(str, label.fontSize) > label.size.x && *str)
		{
			++str;
		}
	}

	for (; *str; ++str)
	{
		float angle = static_cast<float>(cos(glfwGetTime() * 3 + *str)) * 0.1f;
		glm::mat4 world = glm::translate(position) * glm::rotate(angle, glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(label.fontSize, label.fontSize, 1));
		fontPass.setWorldViewProjection(viewProjectionMatrix * world);
		position.x += font.render(*str) * label.fontSize;
	}

	return glm::vec2(position.x, position.y);
}

void cgvkp::rendering::Gui::render(Button const& button, glm::mat4 const& viewProjectionMatrix) const
{
	// set color on mouseover
	render(static_cast<Label>(button), viewProjectionMatrix);
}

void cgvkp::rendering::Gui::render(Input const& input, glm::mat4 const& viewProjectionMatrix) const
{	
	Label label;
	
	label.color = input.color;
	label.fontSize = input.fontSize;
	label.size.y = input.size.y;
	label.text = "|" ;
	label.offset = input.offset;
	label.size.x = font.getWidth(label.text, label.fontSize);

	label.position = render(static_cast<Label>(input), viewProjectionMatrix);
	if (pActiveInput == &input && static_cast<int>(2 * glfwGetTime()) % 2)
	{
		render(label, viewProjectionMatrix);
	}
}

cgvkp::rendering::Label& cgvkp::rendering::Gui::createLabel(char const* text, float fontSize, Anchor anchor /* = center */, glm::vec2 const& offset /* = glm::vec2(0, 0) */, glm::vec3 const& color /* = glm::vec3(1, 1, 1) */)
{
	labels.emplace_back();

	Label& label = labels.back();
	label.text = text;
	label.color = color;
	label.size.x = font.getWidth(text, fontSize);
	label.size.y = font.getHeight(fontSize);
	label.anchor = anchor;
	label.offset = offset;
	label.fontSize = fontSize;

	return label;
}

cgvkp::rendering::Label& cgvkp::rendering::Gui::createLabel(std::function<std::string()> getText, float fontSize, Anchor anchor /*= center */, glm::vec2 const& offset/*  = glm::vec2(0, 0)*/, glm::vec3 const& color /* = glm::vec3(1, 1, 1)*/)
{
	Label& label = createLabel("", fontSize, anchor, offset, color);
	label.getText = getText;
	return label;
}

void cgvkp::rendering::Gui::createInput(float fontSize, Anchor anchor /*= center*/, glm::vec2 const& offset /*= glm::vec2(0, 0) */, float width /* = 300 */, glm::vec3 const& color /* = glm::vec3(1, 1, 1) */)
{
	inputs.emplace_back();

	Input& input = inputs.back();
	input.color = color;
	input.size.x = width;
	input.size.y = font.getHeight(fontSize);
	input.anchor = anchor;
	input.offset = offset;
	input.fontSize = fontSize;
	input.text = "Alfred ist wieder da!";
	input.onClick = [&]() { pActiveInput = &input; };
}

void cgvkp::rendering::Gui::createButton(std::string const& text, float fontSize, std::function<void()> onClick, Anchor anchor /*= center*/, glm::vec2 const& offset /*= glm::vec2(0, 0)*/, glm::vec3 const& color /*= glm::vec3(1, 1, 1)*/)
{
	buttons.emplace_back();

	Button& button = buttons.back();
	button.text = text;
	button.color = color;
	button.size.x = font.getWidth(text, fontSize);
	button.size.y = font.getHeight(fontSize);
	button.anchor = anchor;
	button.offset = offset;
	button.onClick = onClick;
	button.fontSize = fontSize;
}

void cgvkp::rendering::Gui::updateMousePosition(float x, float y)
{
	pHoveredButton = nullptr;

	x -= framebufferWidth / 2;
	y = framebufferHeight / 2 - y;
	
	// Find the button the mouse hovers.
	for (auto& button : buttons)
	{
		if (button.within(x, y))
		{
			pHoveredButton = &button;
			button.color = glm::vec3(0, 1, 0);
			return;
		}
	}

	for (auto& input : inputs)
	{
		if (input.within(x, y))
		{
			pHoveredButton = &input;
			input.color = glm::vec3(0, 1, 0);
			return;
		}
	}
}

void cgvkp::rendering::Gui::click()
{
	pActiveInput = nullptr;
	if (pHoveredButton && pHoveredButton->onClick)
	{
		auto f = pHoveredButton->onClick; 
		f(); 
	} 
}

bool cgvkp::rendering::Gui::inputCodePoint(unsigned int codePoint)
{
	if (codePoint == 0)
	{
		return pActiveInput != nullptr;
	}

	if (pActiveInput)
	{
		if (codePoint == 0x08)
		{
			if(!pActiveInput->text.empty()) pActiveInput->text.pop_back();
		}
		else
		{
			pActiveInput->text += static_cast<char>(codePoint);
		} 

		return true;
	}

	return false;
}
