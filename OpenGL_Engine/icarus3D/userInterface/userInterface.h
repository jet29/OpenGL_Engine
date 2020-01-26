#pragma once
#include <imgui.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <string>


class UI {
public:

	//static int listbox_item_current, listbox_sobel_gradient;
	//static int kernelwidth, kernelheight;
	//static bool b_squareMatrix;
	//static float LoG_scale;
	//static int res[2];
	//static int bpp, dpi;
	//static long uniqueColors;
	//static bool hardwareAcceleration;
	//static float f_threshold;
	//static int   i_threshold;
private:
	//GLuint histogram;
public:
	UI();
	bool init(GLFWwindow* window);
	void draw();
	void terminate();
	//void setHistogram(GLuint histogram);
};
