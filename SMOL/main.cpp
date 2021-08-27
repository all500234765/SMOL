#include "pc.h"

#include "Engine.h"
#include "Platform/Pipeline.h"

#include <pspdebug.h>

PSP_MODULE_INFO("smol_test", PSP_MODULE_USER, 1, 0);

class MyGame final: public Game {
private:
    Buffer buff;
    VertexBuffer vb;
    Mesh mesh;

public:
    virtual void GameStart() override {
        /*pspDebugScreenInit();
        pspDebugScreenClear();       // Clears screen pixels
        pspDebugScreenSetXY(0, 0);   // Reset where we draw

        pspDebugScreenPrintf("Hello world!");*/
        
        struct V {
            ColorLowNoAlpha c; // 16
            short x, y, z;     // 48
        };

        V* SMOL_ALIGN(16) b = new V[3];
        b[0].x = 0;
        b[0].y = 0;
        b[0].z = 0;
        b[0].c.SetColor(SMOL_COLOR(255, 0, 0, 255));
        
        b[1].x = 128;
        b[1].y = 0;
        b[1].z = 0;
        b[1].c.SetColor(SMOL_COLOR(0, 255, 0, 255));
        
        b[2].x = 128;
        b[2].y = 0;
        b[2].z = 0;
        b[2].c.SetColor(SMOL_COLOR(0, 0, 255, 255));

        buff = Buffer(b, 3);
        vb.Create(3, VertexFormat(VFVertex16 | VFColorLowNoAlpha), buff);

        
        pspDebugScreenPrintf("AWAWAWA???\n");
        mesh.Create(&vb);
        pspDebugScreenPrintf("AWAWAWA!!!\n");
    }

    virtual void Step(f32 delta_time) override {
        //mesh.Draw(false);
        pspDebugScreenPrintf("FPS: %f\n", delta_time);


    }

    virtual void GameEnd() override {

    }


};

int main(int argc, char** argv) {
    MyGame game;

    EngineInit(argc, argv);
    EngineRun(&game);

    return 0;
}
