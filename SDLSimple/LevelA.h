// LevelA.h
#include "Scene.h"

class LevelA : public Scene {
public:
    int ENEMY_COUNT = 5;

    bool level_win = false;

    ~LevelA();

    //getter
    bool      const get_level_win() const { return level_win; }

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

    
};