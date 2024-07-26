#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include <cstdlib>
#include "Entity.h"
#include "Map.h"
#include "Effects.h"

// ––––– STRUCTS AND ENUMS ––––– //

struct GameState
{
    Entity* player;
    Entity* walker;
    Entity* jumper;
    Entity* shooter;
    Entity* enemy;
    Map* map;
};

Effects* g_effects;
bool g_is_colliding_bottom = false;
bool succeed = false;
bool failed = false;
bool a_defeated = false;
bool b_defeated = false;
bool c_defeated = false;

// ––––– CONSTANTS ––––– //
constexpr int WINDOW_WIDTH = 500 * 2,
WINDOW_HEIGHT = 400 * 2;
const int FONTBANK_SIZE = 16;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char SPRITESHEET_FILEPATH[] = "sprites/doom.png";
constexpr char PLATFORM_FILEPATH[] = "sprites/platformPack_tile027.png";
constexpr char MAP_TILESET_FILEPATH[] = "sprites/tileset2.png";
constexpr char ENEMY_FILEPATH[] = "sprites/enemy.png";
constexpr char FONT_SPRITE_FILEPATH[] = "sprites/font1.png";

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr int CD_QUAL_FREQ = 44100,
AUDIO_CHAN_AMT = 2,     // stereo
AUDIO_BUFF_SIZE = 4096;

constexpr char BGM_FILEPATH[] = "sprites/kehlani.mp3",
SFX_FILEPATH[] = "sprites/bounce.wav";

constexpr int PLAY_ONCE = 0,    // play once, loop never
NEXT_CHNL = -1,   // next available channel
ALL_SFX_CHNL = -1;


Mix_Music* g_music;
Mix_Chunk* g_jump_sfx;

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 1, 2, 2, 2, 2, 0, 0, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 1, 1, 1, 
    0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 1, 1, 1, 1, 
    2, 2, 2, 2, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2
};




// ––––– GLOBAL VARIABLES ––––– //
GameState g_state;


SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

GLuint g_font_texture_id;


// ––––– GENERAL FUNCTIONS ––––– //
void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
    // Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (screen_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Physics (again)!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ––––– VIDEO ––––– //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);


    //EFFECTS
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    //g_effects->start(FADEOUT, 2.0f);

    g_font_texture_id = load_texture(FONT_SPRITE_FILEPATH);

    // ––––– MAP SETUP ––––– //
    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 4, 1);

    // ––––– AI SETUP ––––– //
    GLuint enemy_texture_id = load_texture(ENEMY_FILEPATH);

    g_state.enemy = new Entity[3];
    for (int i = 0; i < 3; i++)
    {
        g_state.enemy[i].set_entity_type(ENEMY);
        g_state.enemy[i].set_texture_id(enemy_texture_id);
        g_state.enemy[i].set_speed(1.0f);
        g_state.enemy[i].set_height(0.8f);
        g_state.enemy[i].set_width(0.8f);

        if (i == 0)
        {
            g_state.enemy[i].set_ai_type(WALKER);
            g_state.enemy[i].set_ai_state(IDLE);
            g_state.enemy[i].set_position(glm::vec3(6.0f, 0.0f, 0.0f));
            g_state.enemy[i].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
            
        }
        else if (i == 1)
        {
            g_state.enemy[i].set_ai_type(GUARD);
            g_state.enemy[i].set_ai_state(IDLE);
            g_state.enemy[i].set_position(glm::vec3(10.0f, 0.0f, 0.0f));
            g_state.enemy[i].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
        }
        else
        {
            g_state.enemy[i].set_ai_type(SHOOTER);
            g_state.enemy[i].set_ai_state(IDLE);
            //g_state.enemy[i].set_velocity(glm::vec3(-1.0f, 0.0f, 0.0f));
            g_state.enemy[i].set_position(glm::vec3(4.0f, -5.0f, 0.0f));
            g_state.enemy[i].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
        }

    }


    // ––––– BGM ––––– //
    Mix_OpenAudio(CD_QUAL_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHAN_AMT, AUDIO_BUFF_SIZE);
    
    // STEP 1: Have openGL generate a pointer to your music file
    g_music = Mix_LoadMUS(BGM_FILEPATH); // works only with mp3 files

    // STEP 2: Play music
    Mix_PlayMusic(
        g_music,  // music file
        -1        // -1 means loop forever; 0 means play once, look never
    );

    // STEP 3: Set initial volume
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2.0);

    // ––––– SFX ––––– //
    g_jump_sfx = Mix_LoadWAV(SFX_FILEPATH);

    // ––––– PLATFORMS ––––– //
    GLuint platform_texture_id = load_texture(PLATFORM_FILEPATH);

    // ––––– PLAYER (GEORGE) ––––– //
    GLuint player_texture_id = load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.905f, 0.0f);

    g_state.player = new Entity(player_texture_id, 5.0f, 0.7f, 1.0f, PLAYER);
    g_state.player->set_acceleration(glm::vec3(0.0f, -9.8f, 0.0f));


    // Jumping
    g_state.player->set_jumping_power(5.8f);

    // ––––– GENERAL ––––– //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                // Jump
                if (g_state.player->get_collided_bottom())
                {
                    g_state.player->jump();
                    Mix_PlayChannel(NEXT_CHNL, g_jump_sfx, 0);

                    /**
                     Mix_FadeInChannel(channel_id, sound_chunk, loops, fade_in_time);


                     */
                }
                break;

            case SDLK_h:
                // Stop music
                Mix_HaltMusic();
                break;

            case SDLK_p:
                Mix_PlayMusic(g_music, -1);

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {
        if (!succeed && !failed)
        {
            g_state.player->move_left();
        }
        
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        if (!succeed && !failed)
        {
            g_state.player->move_right();
        }
    }

    if (glm::length(g_state.player->get_movement()) > 1.0f)
    {
        g_state.player->normalise_movement();
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }


    
    while (delta_time >= FIXED_TIMESTEP)
    {


        g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.enemy, 3, g_state.map);
        
        for (int i = 0; i < 3; i++)
        {
            g_state.enemy[i].update(FIXED_TIMESTEP, g_state.player, g_state.player, 1, g_state.map);

        }
        


        delta_time -= FIXED_TIMESTEP;

        g_effects->update(FIXED_TIMESTEP);

       
    }

    // i know this is very not optimal and i shouldn't do this but my brain is dying
    if (g_state.enemy[0].get_is_defeated() || g_state.enemy[0].get_position().y < -10)
    {
        a_defeated = true;
    }
    if (g_state.enemy[1].get_is_defeated() || g_state.enemy[1].get_position().y < -10)
    {
        b_defeated = true;
    }
    if (g_state.enemy[2].get_is_defeated() || g_state.enemy[2].get_position().y < -10)
    {
        c_defeated = true;
    }

    if ((g_state.player->get_position().y < -10) || g_state.player->get_is_defeated() == true)
    {
        g_state.player->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
        g_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
        failed = true;
    }
    else if (a_defeated && b_defeated && c_defeated)
    {
        g_state.player->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
        g_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
        succeed = true;
    }


    g_accumulator = delta_time;
    g_view_matrix = glm::mat4(1.0f);
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_state.player->get_position().x, -g_state.player->get_position().y, 0.0f));


    //THIS
    //g_view_matrix = glm::translate(g_view_matrix, g_effects->get_view_offset());
}

void render()
{



    glClear(GL_COLOR_BUFFER_BIT);

    g_program.set_view_matrix(g_view_matrix);
    g_state.player->render(&g_program);
    g_state.map->render(&g_program);

    for (int i = 0; i < 3; i++)
    {
        g_state.enemy[i].render(&g_program);
    }
   
    if (succeed)
    {
        draw_text(&g_program, g_font_texture_id, "you win!", 0.3f, 0.005f, glm::vec3(g_state.player->get_position().x - 1.3f, g_state.player->get_position().y, 0.0f));

    }
    if (failed)
    {
        draw_text(&g_program, g_font_texture_id, "you lose!", 0.3f, 0.005f, glm::vec3(g_state.player->get_position().x - 1.3f, g_state.player->get_position().y, 0.0f));
    }

    SDL_GL_SwapWindow(g_display_window);


}

void shutdown()
{
    SDL_Quit();

    delete g_state.walker;
    delete g_state.jumper;
    delete g_state.shooter;
    delete g_state.player;
    delete g_state.map;
}

// ––––– GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}