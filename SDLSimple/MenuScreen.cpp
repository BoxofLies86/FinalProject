#include "MenuScreen.h"
#include "Utility.h"

#define LEVEL_WIDTH 2
#define LEVEL_HEIGHT 2

constexpr char SPRITESHEET_FILEPATH[] = "sprites/doom.png",
PLATFORM_FILEPATH[] = "sprites/tileset1.png",
ENEMY_FILEPATH[] = "sprites/enemy.png",
FONT_SPRITE_FILEPATH[] = "sprites/font1.png";

GLuint g_font_texture_id;

unsigned int LEVEL_DATA1[] =
{
    0, 0,
    0, 0
};

MenuScreen::~MenuScreen()
{
    //delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.shoot_sfx);
    //Mix_FreeMusic(m_game_state.bgm);
}

void MenuScreen::initialise()
{
    GLuint map_texture_id = Utility::load_texture("sprites/tileset1.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA1, map_texture_id, 1.0f, 4, 1);

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    g_font_texture_id = Utility::load_texture(FONT_SPRITE_FILEPATH);

    int player_walking_animation[4][4] =
    {
    { 1, 5, 9, 13 },  // for George to move to the left,
    { 3, 7, 11, 15 }, // for George to move to the right,
    { 2, 6, 10, 14 }, // for George to move upwards,
    { 0, 4, 8, 12 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.905f, 0.0f);


    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        0.9f,                      // width
        0.9f,                       // height
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(5.0f, -3.0f, 0.0f));

    // Jumping
    //m_game_state.player->set_jumping_power(3.0f);

    /**
     Enemies' stuff */
   /* GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    }


    m_game_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_position(glm::vec3(3.0f, 0.0f, 0.0f));*/
    //m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    //m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    /**
     BGM and SFX
     */
    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    //m_game_state.bgm = Mix_LoadMUS("assets/dooblydoo.mp3");
    //Mix_PlayMusic(m_game_state.bgm, -1);
    //Mix_VolumeMusic(0.0f);

    //m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
}

void MenuScreen::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    //for (int i = 0; i < ENEMY_COUNT; i++)
    //{
    //    m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    //}
}


void MenuScreen::render(ShaderProgram* g_shader_program)
{
    Utility::draw_text(g_shader_program, g_font_texture_id, "The Case of The Horizontally", 0.32, 0.00001f, glm::vec3(0.5f, -2.0f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "Challenged Doomguy", 0.32, 0.03f, glm::vec3(2.0f, -3.0f, 0.0f));
    Utility::draw_text(g_shader_program, g_font_texture_id, "Press Enter to Start", 0.3, 0.03f, glm::vec3(2.0f, -5.0f, 0.0f));
    //m_game_state.map->render(g_shader_program);
    //m_game_state.player->render(g_shader_program);
    /*for (int i = 0; i < m_number_of_enemies; i++)
        m_game_state.enemies[i].render(g_shader_program);*/
}
