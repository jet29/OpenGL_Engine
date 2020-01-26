#include "UserInterface.h"

int   UI::listbox_item_current = 0;
int   UI::listbox_sobel_gradient = 0;
int   UI::kernelheight = 3;
int   UI::kernelwidth = 3;
float UI::LoG_scale = 0.75f;
bool  UI::b_squareMatrix = 1;
int   UI::res[] = { 0,0 };
int   UI::bpp = 0;
int   UI::dpi = 0;
long  UI::uniqueColors = 0;
bool  UI::hardwareAcceleration = 0;
float UI::f_threshold = 0.5f;
int   UI::i_threshold = 122;

UI::UI() {
	histogram = 0;
}

bool UI::init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 330 core"))
		return false;
	ImGui::StyleColorsDark();

	return true;
}

void UI::draw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Set ImGui parameters
	// Techniques list
	ImGui::Begin("Image settings");
	const char* items[] = { "Regular","Negative","Grayscale", "Black and White","Sobel","Roberts","Prewitt",
							"Mean","Median","Laplace of Gaussian", "Toon Shading" };

	ImGui::Combo("Techniques\n", &listbox_item_current, items, IM_ARRAYSIZE(items));
	ImGui::Checkbox("Hardware Acceleration", &hardwareAcceleration);

	// Kernel size menu
	int w_min = 2, w_max = 7, h_min = 2, h_max = 7;
	if (listbox_item_current == 3) {
		ImGui::Separator();
		if (!hardwareAcceleration)
			ImGui::SliderInt("Threshold", &i_threshold, 0, 255);
		else
			ImGui::SliderFloat("Threshold", &f_threshold, 0.0f, 1.0f, "%.2f");
	}
	if (listbox_item_current > 3) {
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Kernel Size", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (b_squareMatrix == 0) {
				w_min = kernelheight == 1 ? w_min = 2 : w_min = 1;
				h_min = kernelwidth == 1 ? h_min = 2 : h_min = 1;
				ImGui::SliderInt("Width", &kernelwidth, w_min, w_max);
				ImGui::SliderInt("Height", &kernelheight, h_min, h_max);
			}
			else {
				w_min = h_min = 2;
				ImGui::SliderInt("Size", &kernelwidth, h_min, h_max);
				kernelheight = kernelwidth;
			}
			ImGui::Checkbox("Square Matrix", &b_squareMatrix);
			if (listbox_item_current == 9) {
				ImGui::SliderFloat("Scale", &LoG_scale, 0.5f, 1.1f, "%.3f");
			}
		}
	}

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Metadata", ImGuiTreeNodeFlags_CollapsingHeader))
	{
		if (ImGui::CollapsingHeader("Histogram", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Image((void*)histogram, ImVec2(312, 150));
		}
		ImGui::Text("Resolution: %ix%i", res[0], res[1]);
		ImGui::Text("Bits per pixel: %i bpp", bpp);
		ImGui::Text("Unique colours: %i", uniqueColors);
		ImGui::Text("Dots per inch: %i dpi", dpi);

	}
	ImGui::End();

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UI::setHistogram(GLuint histogram) {
	this->histogram = histogram;
}