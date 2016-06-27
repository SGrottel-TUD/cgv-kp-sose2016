#include "Gui.hpp"
#include "glfw/glfw3.h"
#include "font.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <math.h>

cgvkp::rendering::Gui::Gui()
	: width(0), height(0), fontSize(0), hoveredButton(nullptr), activeInput(nullptr)
{
}

cgvkp::rendering::Gui::~Gui()
{
}

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

	fontSize = 4;

	return true;
}

void cgvkp::rendering::Gui::deinit()
{
	font.deinit();
}

void cgvkp::rendering::Gui::clear()
{
	hoveredButton = nullptr;
	activeInput = nullptr;
	
	labels.clear();
	buttons.clear();
	inputs.clear();
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
	label.position = glm::vec2(-label.size.x / 2, -label.size.y / 2);

	if (label.anchor & left)
	{
		label.position.x = -static_cast<float>(width) / 2;
	}
	if (label.anchor & right)
	{
		label.position.x = static_cast<float>(width) / 2 - label.size.x;
	}
	if (label.anchor & top)						
	{
		label.position.y = static_cast<float>(height) / 2 - label.size.y;
	}
	if (label.anchor & bottom)					
	{
		label.position.y = -static_cast<float>(height) / 2;
	}

	label.position += label.offset;
}

void cgvkp::rendering::Gui::setSize(float fovy, GLsizei framebufferWidth, GLsizei framebufferHeight, float zPlane)
{
	height = 2 * tan(fovy / 2) * zPlane;
	internExternRatio = height / framebufferHeight;
	width = static_cast<float>(framebufferWidth)  * internExternRatio;

	this->framebufferHeight = framebufferHeight;
	this->framebufferWidth = framebufferWidth;

	setPositions();
}

void cgvkp::rendering::Gui::loadMenu()
{
	clear();

	createButton("Spielen", 10, std::bind(&Gui::loadScore, this), top);
	createButton("Highscore", 10, std::bind(&Gui::loadHighscore, this), center);
	//createButton("Beenden", ????, left | bottom, glm::vec2(0, 0));

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

	std::list<Score> scores = getHighscores();

	createLabel("Highscore", 12, top);

	float offsetX = 7;
	glm::vec2 offsetName = glm::vec2(offsetX, -3 * fontSize);
	glm::vec2 offsetNum = glm::vec2(-offsetX, -3 * fontSize);
	 
	for (auto const& s : scores)
	{
		Label& name = createLabel(s.name, 5, top | left, offsetName);
		Label& score = createLabel(std::to_string(s.score), 5,  top | right, offsetNum);
		
		// points
		float pointLeft = -width / 2 + offsetX + name.size.x;
		float pointRight = width / 2 - offsetX - score.size.x;
		std::string points = "";

		std::string str = " . ";
		float strWidth = font.getWidth(str, name.fontSize);
		for (float x = pointLeft; x < pointRight - strWidth; x += strWidth)
		{
			points += str;
		}
		createLabel(points, 5, top, glm::vec2((pointLeft + pointRight) / 2, offsetName.y));

		offsetName.y -= 5;
		offsetNum.y -= 5;
	}
	createButton("Back", 8, std::bind(&Gui::loadMenu, this), bottom);
	setPositions();
}

void cgvkp::rendering::Gui::loadEntry()
{
	clear();
	float x = createLabel("Erreichte Punktzahl: ", 5, left,glm::vec2(0, 10)).size.x;
	createLabel(std::to_string(getScore()), 5, left, glm::vec2(x, 10));
	Label& label = createLabel("Gib deinen Namen ein: ", 5, left);
	createInput(5, left, glm::vec2(label.size.x,0));
	createButton("Weiter", 5, std::bind(&Gui::loadHighscore, this), bottom);
	setPositions();
}

void cgvkp::rendering::Gui::render(glm::mat4 const& viewProjectionMatrix) const
{
	fontPass.use();

	for (auto& label : labels)
	{
		render(label, viewProjectionMatrix);
	}
	for (auto& button : buttons)
	{
		render(button, viewProjectionMatrix);
	}
	for (auto& input : inputs)
	{
		render(input, viewProjectionMatrix);
	}
}

glm::vec2 cgvkp::rendering::Gui::render(Label const& label, glm::mat4 const& viewProjectionMatrix) const
{
	glm::vec3 position = glm::vec3(label.position, -90);
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
		glm::mat4 world = glm::translate(position) * glm::rotate(angle, glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(label.fontSize, label.fontSize, label.fontSize));
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
	if (activeInput == &input && static_cast<int>(2 * glfwGetTime()) % 2)
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

cgvkp::rendering::Label&  cgvkp::rendering::Gui::createLabel(std::function<std::string()> getText, float fontSize, Anchor anchor /*= center */, glm::vec2 const& offset/*  = glm::vec2(0, 0)*/, glm::vec3 const& color /* = glm::vec3(1, 1, 1)*/)
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
	input.onClick = [&]() { activeInput = &input; };
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
	hoveredButton = nullptr;

	// Calculate intern position.
	glm::vec2 position;
	position.x = width / (framebufferWidth - 1) *  x - width / 2;
	position.y = - height / (framebufferHeight - 1) * y + height / 2;
	
	// Find the button the mouse hovers.
	for (auto& button : buttons)
	{
		if (button.within(position.x, position.y))
		{
			hoveredButton = &button;
			button.color = glm::vec3(0, 1, 0);
			return;
		}
	}

	for (auto& input : inputs)
	{
		if (input.within(position.x, position.y))
		{
			hoveredButton = &input;
			input.color = glm::vec3(0, 1, 0);
			return;
		}
	}
}

void cgvkp::rendering::Gui::click()
{
	activeInput = nullptr;
	if (hoveredButton && hoveredButton->onClick)
	{
		auto f = hoveredButton->onClick; 
		f(); 
	} 
}

bool cgvkp::rendering::Gui::inputCodePoint(unsigned int codePoint)
{
	if (codePoint == 0)
	{
		return activeInput != nullptr;
	}

	if (activeInput)
	{
		if (codePoint == 0x08)
		{
			if(!activeInput->text.empty()) activeInput->text.pop_back();
		}
		else
		{
			activeInput->text += static_cast<char>(codePoint);
		} 

		return true;
	}

	return false;
}