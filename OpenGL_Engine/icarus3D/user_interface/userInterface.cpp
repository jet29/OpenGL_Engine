#include "UserInterface.h"
#include "../icarus3D.h"
#include <ImGuizmo.h>


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
static const char* current_item_scene = NULL;
static const char* current_item_light = NULL;
static const char* current_item_light_selector = NULL;
int radio_button_trans_type = 0;
int radio_button_loc_w = 0;
bool fps_bool_checkbox = true;
bool closedModelWindowBool = true;
int stereo_eye_opt = 0;

vector<string> lights = { "Directional", "Pointlight" };

icarus3D* instance;

UI::UI() {
}

bool UI::init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true) || !ImGui_ImplOpenGL3_Init("#version 330 core"))
		return false;
	ImGui::StyleColorsDark();

	//Get singleton instance;
	instance = icarus3D::Instance();
	return true;
}

void UI::particleSystemWindow() {
	if (particleSystemFlag && instance->particlesystemBool) {
		ImGui::Begin("Particle System",&particleSystemFlag);

		ImGui::Text("Load Particle System");
		static char buffer_load[1024] = "./rain.json";
		ImGui::InputText("##load_particle_system", buffer_load, IM_ARRAYSIZE(buffer_load));
		ImGui::SameLine();
		if (ImGui::Button("Load##particle_system", ImVec2(60, 20))) {
			instance->particleSystem->load(buffer_load);
		}

		ImGui::Text("Save Particle System");
		static char buffer_save[1024] = "clouds";
		ImGui::InputText("##save_particle_system", buffer_save, IM_ARRAYSIZE(buffer_save));
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(60, 20))) {
			instance->particleSystem->save(buffer_save);
		}

		ImGui::Text("Change Texture");
		static char buffer_texture[1024] = "rain.png";
		ImGui::InputText("##texture_particle_system", buffer_texture, IM_ARRAYSIZE(buffer_texture));
		ImGui::SameLine();
		if (ImGui::Button("Load##texture", ImVec2(60, 20))) {
			instance->setParticleSystemTexture(buffer_texture);
		}

		ImGui::Text("Max Particles");
		ImGui::DragInt("##Max_Particles", &instance->particleSystem->max_particles, 1, 1, 1000);
		ImGui::Text("Particles per Spawn");
		ImGui::DragInt("##Particles_per_Spawn", &instance->particleSystem->particles_per_spawn, 1, 1, 100);
		ImGui::Text("Spawn Radius");
		ImGui::DragFloat("##Spawn_Radius", &instance->particleSystem->spawn_radius, 0.1, 1, 1000);
		ImGui::Text("Time to Live");
		ImGui::DragFloat("##Time_to_live", &instance->particleSystem->particle_ttl, 0.01, 0.01, 10);
		ImGui::Text("Time Between Spawn");
		ImGui::DragFloat("##Time_Between_Spawn", &instance->particleSystem->time_between_spawn, 0.01, 0.01, 10); 
		ImGui::Text("Particle Speed");
		ImGui::DragFloat("##Particle_Speed", &instance->particleSystem->particle_speed, 0.1, 1, 100);
		ImGui::Text("Particle Scale");
		//ImGui::Checkbox("##Particle_switch", &instance->particleSystem->particle_direction);
		ImGui::RadioButton("Width", &particle_system_radioButtons_opt_scale, 0); ImGui::SameLine();
		ImGui::RadioButton("height", &particle_system_radioButtons_opt_scale, 1); ImGui::SameLine();
		switch (particle_system_radioButtons_opt_scale) {
		case 0:
			ImGui::DragFloat("##particle_scale_width", &instance->particleSystem->particle_scale.x, 0.005f, 0.0f, 1.0f, "%.3f");
			break;
		case 1:
			ImGui::DragFloat("##particle_scale_height", &instance->particleSystem->particle_scale.y, 0.005f, 0.0f, 1.0f, "%.3f");
			break;
		}

		ImGui::Text("Particle Direction");
		//ImGui::Checkbox("##Particle_switch", &instance->particleSystem->particle_direction);
		ImGui::RadioButton("X", &particle_system_radioButtons_opt_dir, 0); ImGui::SameLine();
		ImGui::RadioButton("Y", &particle_system_radioButtons_opt_dir, 1); ImGui::SameLine();
		ImGui::RadioButton("Z", &particle_system_radioButtons_opt_dir, 2);
		switch (particle_system_radioButtons_opt_dir) {
		case 0:
			ImGui::DragFloat("##particle_dir_x", &instance->particleSystem->particle_direction.x, 0.005f, -1.0f, 1.0f, "%.3f");
			break;
		case 1:
			ImGui::DragFloat("##particle_dir_y", &instance->particleSystem->particle_direction.y, 0.005f, -1.0f, 1.0f, "%.3f");
			break;
		case 2:
			ImGui::DragFloat("##particle_dir_z", &instance->particleSystem->particle_direction.z, 0.005f, -1.0f, 1.0f, "%.3f");
			break;
		}

		ImGui::End();
	}

}

void UI::settingsWindow() {
	if (settingFlag) {
		ImGui::Begin("Settings", &settingFlag);

		// Show fps settings
		ImGui::Checkbox("##fps_checkbox", &fps_bool_checkbox);
		ImGui::SameLine();
		ImGui::Text("Show FPS");

		ImGui::Separator();

		// Particle system checkbox
		ImGui::Checkbox("##particle_system", &instance->particlesystemBool);
		ImGui::SameLine();
		ImGui::Text("Particle system");

		

		// Particle system button
		if (instance->particlesystemBool) {
			ImGui::SameLine();
			if (particleSystemFlag) {
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
				if (ImGui::Button("Settings##particle_settings")) {
					particleSystemFlag = particleSystemFlag == true ? false : true;
				}
				ImGui::PopStyleColor(2);
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
				if (ImGui::Button("Settings##particle_settings")) {
					particleSystemFlag = particleSystemFlag == true ? false : true;
				}
				ImGui::PopStyleColor(3);
			}
		}

		ImGui::Separator();

		// Anaglyph stereoscopy checkbox and settings
		ImGui::Checkbox("##anaglyph_mode", &instance->stereoBool); ImGui::SameLine();
		ImGui::Text("Anaglyph stereoscopy");
		if (instance->stereoBool) {
			ImGui::Text("Convergence distance");
			if (ImGui::DragFloat("##convergence", &instance->stereoscopic.convergence, 0.1f, 1.0f, 20.0f, "%.1f"))
				instance->updateStereoPerspectiveMatrix();
			ImGui::Text("Intraocular distance");
			if (ImGui::DragFloat("##IOD", &instance->stereoscopic.IOD, 0.1f, 1.0f, 20.0f, "%.1f"))
				instance->updateStereoPerspectiveMatrix();
			ImGui::Text("Field of view");
			if (ImGui::DragFloat("##fov", &instance->stereoscopic.fov, 0.1f, 1.0f, 360.0f, "%.1f°"))
				instance->updateStereoPerspectiveMatrix();
			//if (ImGui::CollapsingHeader("Anaglyph colors")) {
			//	ImGui::RadioButton("Left eye", &stereo_eye_opt, 0); ImGui::SameLine();
			//	ImGui::RadioButton("Right eye", &stereo_eye_opt, 1);
			//	switch (stereo_eye_opt) {
			//	case 0:
			//		ImGui::ColorPicker4("Left eye", &instance->stereoscopic.le_color[0]);
			//		break;
			//	case 1:
			//		ImGui::ColorPicker4("right eye", &instance->stereoscopic.re_color[0]);
			//		break;
			//	}
			//}

		}

		ImGui::Separator();

		// Techniques 
		if (ImGui::CollapsingHeader("Techniques")) {
			// DoF technique selector
			ImGui::Checkbox("##dof_checkbox", &instance->depthOfFieldBool);
			ImGui::SameLine();
			ImGui::Text("Depth of Field");
			//ImGui::DragFloat("DOF Threshold", &instance->scene[instance->currentScene]->DOFThreshold, 0.005f, 0, 100);

			// SSAO  technique selector
			ImGui::Checkbox("##ssao_checkbox", &instance->ssaoBool);
			ImGui::SameLine();
			ImGui::Text("SSAO");
			if (instance->ssaoBool) {
				ImGui::Text("Radius");
				ImGui::DragFloat("##Radius", &instance->ssao.radius,0.1f,0.5f,15.0f);
				ImGui::Text("Bias");
				ImGui::DragFloat("##bias", &instance->ssao.bias,0.005f,0.0f,1.0f);
				ImGui::Text("Kernel Size");
				if (ImGui::DragInt("##kernel_size", &instance->ssao.kernelSize, 2,2,128))
					instance->computeSSAOKernel();
			}

		}

		ImGui::Separator();

		// Scene selector
		if (instance->currentScene != -1) {
			if (ImGui::CollapsingHeader("Scene settings")) {
				current_item_scene = instance->scene.size() == 0 ? NULL : instance->scene[instance->currentScene]->name.c_str();
				ImGui::Text("Current scene");
				if (ImGui::BeginCombo("##combo_scenes", current_item_scene)) {
					for (int n = 0; n < instance->scene.size(); n++)
					{
						bool is_selected = (current_item_scene == instance->scene[n]->name.c_str()); // You can store your selection however you want, outside or inside your objects
						if (ImGui::Selectable(instance->scene[n]->name.c_str(), is_selected)) {
							current_item_scene = instance->scene[n]->name.c_str();
							instance->currentScene = n;
							instance->setPickedIndex(-1);
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
					}
					ImGui::EndCombo();
				}
				if (ImGui::CollapsingHeader("Lighting")) {
					// Light selector
					if (ImGui::CollapsingHeader("Directional Light")) {
						//if (ImGui::BeginCombo("##combo_lights", current_item_light)) {
						//	for (int n = 0; n < 2; n++)
						//	{
						//		bool is_selected2 = (current_item_light == lights[n].c_str()); // You can store your selection however you want, outside or inside your objects
						//		if (ImGui::Selectable(lights[n].c_str(), is_selected2)) {
						//			current_item_light = lights[n].c_str();
						//		}
						//		if (is_selected2)
						//			ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						//	}
						//	ImGui::EndCombo();
						//}
						directionalLightProperties();
					}
					if (ImGui::CollapsingHeader("Point Lights")) {
						pointLightProperties();
					}
				}
			}

			ImGui::Separator();

			//if (current_item_light == "Pointlight")
		}

		ImGui::End();
	}
}

void UI::directionalLightProperties() {
	ImGui::Checkbox("##light_switch", &instance->light->lightSwitch); ImGui::SameLine();
	ImGui::Text("Light switch");
	ImGui::RadioButton("X", &dirLight_dir_radioButtons_opt, 0); ImGui::SameLine();
	ImGui::RadioButton("Y", &dirLight_dir_radioButtons_opt, 1); ImGui::SameLine();
	ImGui::RadioButton("Z", &dirLight_dir_radioButtons_opt, 2);
	switch (dirLight_dir_radioButtons_opt) {
	case 0:
		ImGui::DragFloat("", &instance->light->properties.direction.x, 0.005f, -1.0f, 1.0f, "%.3f");
		break;
	case 1:
		ImGui::DragFloat("", &instance->light->properties.direction.y, 0.005f, -1.0f, 1.0f, "%.3f");
		break;
	case 2:
		ImGui::DragFloat("", &instance->light->properties.direction.z, 0.005f, -1.0f, 1.0f, "%.3f");
		break;
	}

	if (ImGui::CollapsingHeader("Light Color")) 
{
		// Ambient color
		if (ImGui::CollapsingHeader("Ambient")) {
			ImGui::ColorPicker3("Ka", &instance->light->properties.color.ambient.x);
		}

		// Diffuse color
		if (ImGui::CollapsingHeader("Diffuse")) {
			ImGui::ColorPicker3("Kd", &instance->light->properties.color.diffuse.x);
		}

		// Specular color
		if (ImGui::CollapsingHeader("Specular")) {
			ImGui::ColorPicker3("Ks", &instance->light->properties.color.specular.x);
		}
	}
}

void UI::pointLightProperties() {
	// Scene selector
	ImGui::Text("Light selector");
	if (ImGui::BeginCombo("##combo_light_selector", current_item_light_selector)) {
		for (int n = 0; n < instance->scene.size(); n++)
		{
			//bool is_selected = (current_item_light_selector == instance->scene[n]->name.c_str()); // You can store your selection however you want, outside or inside your objects
/*			if (ImGui::Selectable(instance->scene[n]->name.c_str(), is_selected)) {
				current_item_light_selector = instance->scene[n]->name.c_str();
				instance->currentScene = n;
				instance->setPickedIndex(-1);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();*/   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	ImGui::Separator();
}

void UI::pickedModelWindow() {

	// If there's an element picked show its info
	if (instance->getPickedIndex() != -1) {
		Model* model = instance->scene[instance->currentScene]->models[instance->getPickedIndex()];
		bool flag = false;
		string frameTitle = "Picked: " + instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->name;
		if (closedModelWindowBool) {
			ImGui::Begin(frameTitle.c_str(), &closedModelWindowBool);

			ImGui::RadioButton("Rotate", &radio_button_trans_type, 0); ImGui::SameLine();
			ImGui::RadioButton("Translate", &radio_button_trans_type, 1); ImGui::SameLine();
			ImGui::RadioButton("Scale", &radio_button_trans_type, 2);

			// Set euler angles from quaternion
			ImGuizmo::DecomposeMatrixToComponents(&model->modelMatrix[0][0], &model->position[0], &model->rotationAngles[0], &model->scale[0]);
			if (ImGui::InputFloat3("Rt", &instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->rotationAngles.x, 2)) {
				flag = true;
				instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->
					setRotationQuaternion(instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->rotationAngles);
			}

			// Set translation matrix
			if (ImGui::InputFloat3("Tr", &instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->position.x, 2)) {
				flag = true;
				instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->setTranslationMatrix();
			}

			if (ImGui::InputFloat3("Sc", &instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->scale.x, 2)) {
				flag = true;
				instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->setScaleMatrix();
			}


			// Update model Matrix if any change has been made
			if (flag)
				instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->computeModelMatrix();

			if (instance->scene[instance->currentScene]->models[instance->getPickedIndex()]->type == POINTLIGHT) {
				PointLight* pointlight = (PointLight*)instance->scene[instance->currentScene]->models[instance->getPickedIndex()];
				ImGui::Text("Light switch");
				ImGui::Checkbox("##pointlight_light_switch", &pointlight->lightSwitch);
				if (ImGui::CollapsingHeader("Pointlight Color")) {
					// Ambient color
					if (ImGui::CollapsingHeader("Ambient")) {
						ImGui::ColorPicker3("Ka", &pointlight->properties.color.ambient[0]);
					}

					// Diffuse color
					if (ImGui::CollapsingHeader("Diffuse")) {
						ImGui::ColorPicker3("Kd", &pointlight->properties.color.diffuse[0]);
					}

					// Specular color
					if (ImGui::CollapsingHeader("Specular")) {
						ImGui::ColorPicker3("Ks", &pointlight->properties.color.specular[0]);
					}

					ImGui::DragFloat("Constant", &pointlight->properties.attenuation.constant, 0.005f, 0.0f, 3.0f);
					ImGui::DragFloat("Linear", &pointlight->properties.attenuation.linear, 0.005f, 0.0f, 3.0f);
					ImGui::DragFloat("Quadratic", &pointlight->properties.attenuation.quadratic, 0.005f, 0.0f, 3.0f);
				}
			}

			//if (ImGui::IsKeyPressed(GLFW_KEY_T)) {
			//	cout << "hola" << endl;
			//}

			ImGui::Text("Transformation space");
			ImGui::RadioButton("Local", &radio_button_loc_w, 0); ImGui::SameLine();
			ImGui::RadioButton("World", &radio_button_loc_w, 1); ImGui::SameLine();
			ImGuizmo::MODE space = radio_button_loc_w == 0 ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

			vector<float> bound = vector<float>(6);
			vector<float> boundSnap = vector<float>(3, 0.1);
			vector<float> snap = vector<float>(3, 1);

			bound[0] = model->mesh->min.x;
			bound[1] = model->mesh->min.y;
			bound[2] = model->mesh->min.z;
			bound[3] = model->mesh->max.x;
			bound[4] = model->mesh->max.y;
			bound[5] = model->mesh->max.z;

			ImGui::GetIO().WantCaptureMouse = true;

			ImGuizmo::RecomposeMatrixFromComponents(&model->position[0], &model->rotationAngles[0], &model->scale[0], &model->modelMatrix[0][0]);
			if (radio_button_trans_type == 0)
				ImGuizmo::Manipulate(&instance->camera.viewMatrix[0][0], &instance->camera.perspectiveMatrix[0][0], ImGuizmo::ROTATE, space, &model->modelMatrix[0][0], NULL, NULL);
			else if (radio_button_trans_type == 1)
				ImGuizmo::Manipulate(&instance->camera.viewMatrix[0][0], &instance->camera.perspectiveMatrix[0][0], ImGuizmo::TRANSLATE, space, &model->modelMatrix[0][0], NULL, NULL);
			else if (radio_button_trans_type == 2)
				ImGuizmo::Manipulate(&instance->camera.viewMatrix[0][0], &instance->camera.perspectiveMatrix[0][0], ImGuizmo::SCALE, space, &model->modelMatrix[0][0], NULL, &snap[0], &bound[0], &boundSnap[0]);

			ImGui::End();
		}
		else {
			instance->setPickedIndex(-1);
			closedModelWindowBool = true;
		}
	}
	
}

void UI::directionalLightWindow() {
	ImGui::Begin("Directional light properties");

	ImGui::End();

}

void UI::showMenuFile() {
	//ImGui::MenuItem("(dummy menu)", NULL, false, false);
	if (ImGui::MenuItem("New Scene")) {
		activateModal = "Create Scene##modal";
	}
	if (ImGui::MenuItem("Open Scene")) {
		activateModal = "Open scene##open_scene_modal";
	}

	if (ImGui::MenuItem("Save Scene",NULL,false,instance->currentScene != -1 ? true :false)) {
		activateModal = "Save scene##save_scene_modal";
	}

}

void UI::drawModals() {
	if (activateModal != "")
		ImGui::OpenPopup(activateModal.c_str());

	// Save new model  - modal
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Add a new model"))
	{
		ImGui::Text("Please write model's path");
		static char buffer[1024] = "assets/models/texturedcube/texturedcube.obj";
		ImGui::InputText("##model_path", buffer, IM_ARRAYSIZE(buffer));

		ImGui::Text("Name your model");
		char bufferName[1024];
		string defaultModelName = "Model " + to_string(instance->scene[instance->currentScene]->models.size());
		strcpy_s(bufferName, defaultModelName.c_str());
		ImGui::InputText("##model_name", bufferName, IM_ARRAYSIZE(bufferName));

		if (ImGui::Button("Add", ImVec2(80, 30))){
			instance->addModel(string(buffer), bufferName);
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::EndPopup();
	}

	// Create new scene  - modal
	ImGui::SetNextWindowSize(ImVec2(300, 130));
	if (ImGui::BeginPopupModal("Create Scene##modal",0))
	{
		ImGui::Text("Please name your scene");
		string defaultSceneName = "Scene " + to_string(instance->scene.size());
		char buffer[1024];
		strcpy_s(buffer, defaultSceneName.c_str());

		ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer));
		if (ImGui::Button("Create", ImVec2(80, 30))) {
			instance->createScene(string(buffer));
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::EndPopup();
	}

	// Add a new light - modal
	ImGui::SetNextWindowSize(ImVec2(300, 130));
	if (ImGui::BeginPopupModal("Add a new light"))
	{
		ImGui::Text("Please name your light");
		string defaultLightName = "Light " + to_string(instance->scene[instance->currentScene]->pointlight_index.size());
		char buffer[1024];
		strcpy_s(buffer, defaultLightName.c_str());
		ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer));
		if (ImGui::Button("Add", ImVec2(80, 30))) {
			instance->addLight(string(buffer));
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::EndPopup();
	}

	// Save new scene
	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Save scene##save_scene_modal"))
	{
		ImGui::Text("Please name your scene");
		static char buffer[1024] = "Scene";
		ImGui::InputText("##save_scene_path", buffer, IM_ARRAYSIZE(buffer));
		char scenePath[1024] = "./";
		strcat_s(scenePath, sizeof(scenePath), buffer);
		strcat_s(scenePath, sizeof(scenePath), ".json");
		if (ImGui::Button("Save", ImVec2(80, 30))) {
			// Save scene code HERE
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::EndPopup();
	}

	ImGui::SetNextWindowSize(ImVec2(300, 150));
	if (ImGui::BeginPopupModal("Open scene##open_scene_modal"))
	{
		ImGui::Text("Write your scene's path");
		static char buffer[1024] = "./Scene.json";
		ImGui::InputText("##load_scene_path", buffer, IM_ARRAYSIZE(buffer));

		if (ImGui::Button("Open", ImVec2(80, 30))) {
			// ---- Open scene code HERE
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(80, 30)))
		{
			ImGui::CloseCurrentPopup();
			activateModal = "";
		}
		ImGui::EndPopup();
	}

}

void UI::showMainMenuBar() {

	if (ImGui::BeginMainMenuBar())
	{
		// File Option
		if (ImGui::BeginMenu("File"))
		{
			showMenuFile();
			ImGui::EndMenu();
		}
		// Scene Option
		if (instance->currentScene != -1) {
			if (ImGui::BeginMenu("Scene")){
				if (ImGui::MenuItem("Add model")) {
					activateModal = "Add a new model";
				}
				if (ImGui::MenuItem("Add light")) {
					activateModal = "Add a new light";
				}
				ImGui::EndMenu();
			}
		}
		// Settings button
		if (settingFlag) {
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			if (ImGui::Button("Settings")) {
				settingFlag = settingFlag == true ? false : true;
			}
			ImGui::PopStyleColor(2);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(247 / 255.0f, 202 / 255.0f, 22 / 255.0f, 1.0f));
			if (ImGui::Button("Settings")) {
				settingFlag = settingFlag == true ? false : true;
			}
			ImGui::PopStyleColor(3);
		}
		ImGui::EndMainMenuBar();
	}
}

void UI::fpsWindow() {
	if (fps_bool_checkbox) {
		ImGui::SetNextWindowSize(ImVec2(100, 30));
		ImGui::SetNextWindowPos(ImVec2(instance->windowWidth-100-10, 10+128));
		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::Begin("##fps_modal", (bool*)0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
			ImGui::Text("FPS: %.1f", instance->getFPS());
		ImGui::End();
	}
}

void UI::collisionAlertWindow() {
	if (instance->collisionBool){
		ImGui::SetNextWindowSize(ImVec2(100, 30));
		ImGui::SetNextWindowPos(ImVec2(instance->windowWidth - 100 - 10, 10 + 128 + 40));
		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.8f, 0.0f, 0.0f, 0.7f));
		ImGui::Begin("##collision_modal", (bool*)0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::PopStyleColor();
		ImGui::Text("COLLISION!");
		ImGui::End();
	}
}

void UI::draw() {
	// Start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::SetOrthographic(true);
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	//ImGui::GetIO().WantCaptureMouse = true;
	//ImGuizmo::DrawGrid(&instance->camera.viewMatrix[0][0], &instance->camera.perspectiveMatrix[0][0], &glm::mat4(1.0f)[0][0], 100.0f);
	ImGuizmo::SetRect(0, 0, instance->windowWidth, instance->windowHeight);
	glm::mat4 matrix;
	//ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0],&instance->camera.position[0], &instance->camera.viewDirection[0], &glm::vec3(1.0f)[0]);
	if(!instance->cameraMode)
		ImGuizmo::ViewManipulate(&instance->camera.viewMatrix[0][0],8.0f,ImVec2(instance->windowWidth-128,0),ImVec2(128,128),0x10101010);
	//ImGuizmo::DecomposeMatrixToComponents(&instance->camera.viewMatrix[0][0], &instance->camera.position[0], &instance->camera.viewDirection[0],NULL);
	showMainMenuBar();

	// Draw fps window
	fpsWindow();

	// Draw alert window for collision
	collisionAlertWindow();

	// Main configuration window
	settingsWindow();

	// Particle System configuration window
	particleSystemWindow();

	// Directional light information
	//directionalLightWindow();
	
	// Picked model window
	pickedModelWindow();

	drawModals(); 

	// Render dear imgui into screen
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
