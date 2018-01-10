#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include <SDL\SDL.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl_gl3.h>

#include "../utilities/Logger.h"

#include <iostream>
#include "../utilities/Settings.h"
#include "Entity.h"
#include "EntityManager.h"

class GUIManager {
	//Singleton class to hold all global consts across engine {width, height of window etc}
public:
	static GUIManager* Instance();
	static GUIManager* Instance(SDL_Window* window);

	//setters for render window flags 
	void renderEntityEditor(bool);
	void setEntityEditor(Entity*);
	void setEntityManager(EntityManager*);
	void renderSettingsGUI(bool);
	void renderSceneGraph(bool);
	

	void render();
private:
	//generate the gui frames
	void generateEntityEditor();
	void generateSettingsGUI();
	void generateSceneGraph();

	SDL_Window* m_window;
	bool m_renderEntityEditor;
	bool m_renderSettingsEditor;
	bool m_renderSceneGraph;

	EntityManager *m_entitymanager;
	Entity* m_entity;

	GUIManager(SDL_Window*);
	static GUIManager* m_Instance;
};
#endif



