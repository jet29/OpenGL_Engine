#pragma once
#include <imgui.hpp>
#include <string.h>
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
private:
	void showMainMenuBar();
	void showMenuFile();
	void drawModals();
	void settingsWindow();
	void particleSystemWindow();
	void directionalLightProperties();
	void pointLightProperties();
	void pickedModelWindow();
	void helpWindow();
	void directionalLightWindow();
	void fpsWindow();
	void collisionAlertWindow();
	// UI Control variables
	int dirLight_dir_radioButtons_opt;
	int particle_system_radioButtons_opt_dir;
	int particle_system_radioButtons_opt_scale;
	std::string activateModal = "";
	bool settingFlag = false;
	bool particleSystemFlag = false;
};
