// Scene.h
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    Map* map;
    Entity* player;
    Entity* enemies;
    Entity* bullet;

    Mix_Music* bgm;
    Mix_Chunk* shoot_sfx;
    Mix_Chunk* walking_sfx;
    Mix_Chunk* death_sfx;

    int next_scene_id;
};

class Scene {
public:
    //int m_number_of_enemies = 2;

    GameState m_game_state;

    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;

    GameState const get_state()             const { return m_game_state; }
    //int       const get_number_of_enemies() const { return m_number_of_enemies; }
};