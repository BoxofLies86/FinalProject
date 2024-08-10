// LevelA.h
#include "Scene.h"

class MenuScreen : public Scene {
public:
    int ENEMY_COUNT = 0;

    ~MenuScreen();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};