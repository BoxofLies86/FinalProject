#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 30
#define LEVEL_HEIGHT 15

constexpr char SPRITESHEET_FILEPATH[] = "sprites/doom.png",
PLATFORM_FILEPATH[] = "sprites/tileset1.png",
ENEMY_FILEPATH[] = "sprites/enemy.png",
BULLET_FILEPATH[] = "sprites/bullet.png",
FONT_SPRITE_FILEPATH[] = "sprites/font1.png";




unsigned int LEVEL_DATA[] =
{
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2,
    2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2,
    2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2,
    2, 0, 0, 2, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2,
    2, 0, 0, 2, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2,
    2, 0, 0, 2, 0, 0, 0, 2, 2, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 2,
    2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 2,
    2, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2,
    2, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2,
    2, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2,
    2, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelA::~LevelA()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    delete m_game_state.bullet;
    Mix_FreeChunk(m_game_state.shoot_sfx);
    Mix_FreeChunk(m_game_state.walking_sfx);
    //Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.death_sfx);
}

void LevelA::initialise()
{
    GLuint map_texture_id = Utility::load_texture("sprites/tileset1.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 4, 1);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);


    int player_walking_animation[4][4] =
    {
    { 1, 5, 9, 13 },  // for George to move to the left,
    { 3, 7, 11, 15 }, // for George to move to the right,
    { 2, 6, 10, 14 }, // for George to move upwards,
    { 0, 4, 8, 12 }   // for George to move downwards
    };

    //glm::vec3 acceleration = glm::vec3(0.0f, -4.905f, 0.0f);

    //Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType); // Simpler constructor

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        3.5f,                      // speed
        0.3f,                      // width
        0.3f,                       // height
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(2.0f, -3.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(0.7f, 0.7f, 0.0f));

    // Jumping
    //m_game_state.player->set_jumping_power(3.0f);

    //BULLET
    GLuint bullet_texture_id = Utility::load_texture(BULLET_FILEPATH);
    m_game_state.bullet = new Entity(bullet_texture_id, 10.0f, 0.4f, 0.125f, BULLET);


    m_game_state.bullet->set_position(m_game_state.player->get_position());
    m_game_state.bullet->set_scale(glm::vec3(0.4f, 0.125f, 0.0f));
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.5f, 0.7f, 0.7f, ENEMY, GUARD, IDLE);
    }


    m_game_state.enemies[0].set_position(glm::vec3(8.0f, -1.0f, 0.0f));
    //m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[1].set_position(glm::vec3(14.0f,-11.0f,0.0f));
    m_game_state.enemies[2].set_position(glm::vec3(20.0f, -5.0f, 0.0f));
    m_game_state.enemies[3].set_position(glm::vec3(23.0f, -11.0f, 0.0f));
    m_game_state.enemies[4].set_position(glm::vec3(26.0f, -3.0f, 0.0f));
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].set_scale(glm::vec3(0.8f, 0.8f, 0.0f));
    }
    //m_game_state.enemies[1].set_position(glm::vec3(3.0f, 0.0f, 0.0f));
    //m_game_state.enemies[1].set_movement(glm::vec3(0.0f));
    //m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_AllocateChannels(16);

   /* m_game_state.bgm = Mix_LoadMUS("sprites/kehlani.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(0.0f);*/

    m_game_state.shoot_sfx = Mix_LoadWAV("sprites/shooty.wav");
    m_game_state.walking_sfx = Mix_LoadWAV("sprites/footstep1.wav");
    m_game_state.death_sfx = Mix_LoadWAV("sprites/death.wav");
}

void LevelA::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    m_game_state.bullet->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, m_game_state.player, 1, m_game_state.map);
    }

    if (!m_game_state.enemies[0].get_is_active() && !m_game_state.enemies[1].get_is_active()
        && !m_game_state.enemies[2].get_is_active() && !m_game_state.enemies[3].get_is_active()
        && !m_game_state.enemies[4].get_is_active())
    {
        level_win = true;
    }

    //std::cout << "level win? : " << level_win << std::endl;

    

}


void LevelA::render(ShaderProgram* g_shader_program)
{
    if (m_game_state.player->get_position().x != m_game_state.bullet->get_position().x)
    {
        m_game_state.bullet->render(g_shader_program);
    }
    //m_game_state.bullet->render(g_shader_program);
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++)
        m_game_state.enemies[i].render(g_shader_program);
}
