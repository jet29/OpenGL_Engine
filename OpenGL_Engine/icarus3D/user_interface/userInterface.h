#pragma once
#include <imgui.hpp>
//#include "glm/glm.hpp"
//#include <GLFW/glfw3.h>


#include <string>
//#include <stb_image.h>

class UI {
public:

	static int listbox_item_current, listbox_sobel_gradient;
	static int kernelwidth, kernelheight;
	static bool b_squareMatrix;
	static float LoG_scale;
	static int res[2];
	static int bpp, dpi;
	static long uniqueColors;
	static bool hardwareAcceleration;
	static float f_threshold;
	static int   i_threshold;
public:
	UI();
	bool init(GLFWwindow* window);
	void draw();
	void terminate();
};
