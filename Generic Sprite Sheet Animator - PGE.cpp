// Generic Sprite Sheet animation for the PixelGameEngine (PGE)
//
// Joseph21
// January 30, 2022

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// let the screen dimensions be constant and vary the resolution by
// adapting the pixel size
#define SCREEN_X   1640
#define SCREEN_Y    840
#define PIXEL_X       1
#define PIXEL_Y       1

// PGE derived class
class AnimationEngine : public olc::PixelGameEngine {

struct sMyRect {
    int x = 0, y = 0;
    int w = 0, h = 0;
};

struct sAnimatedSequence {
    // static info
    olc::Sprite *spritePtr = nullptr;
    std::vector<sMyRect> vFrames;   // contains the subsprite adresses

    // sub sprite defining info
    int nNrOfFrames  = 0;
    int subSprWidth  = 0;
    int subSprHeight = 0;
    int tileOffsetX  = 0; // offset where animation starts
    int tileOffsetY  = 0;
    int tilesX = 0;
    int tilesY = 0;

    // dynamic info
    int nCurFrame    = 0;
    float fTickTime  = 0.0f;
    float fCumTime   = 0.0f;
};

void InitSequence( sAnimatedSequence &as, olc::Sprite *sprPtr, int nrFrames, int subSprWidth, int subSprHeight, int tileOffX, int tileOffY, int tileX, int tileY, float fTick ) {
    as.spritePtr    = sprPtr;
    as.vFrames.clear();   // not used yet
    as.nNrOfFrames  = nrFrames;
    as.subSprWidth  = subSprWidth;
    as.subSprHeight = subSprHeight;
    as.tileOffsetX  = tileOffX;
    as.tileOffsetY  = tileOffY;
    as.tilesX       = tileX;
    as.tilesY       = tileY;
    as.nCurFrame    = 0;
    as.fCumTime     = 0.0f;
    as.fTickTime    = fTick;
}

void UpdateSequence( sAnimatedSequence &as, float fElapsedTime ) {
    as.fCumTime += fElapsedTime;
    if (as.fCumTime > as.fTickTime) {
        as.fCumTime -= as.fTickTime;

        as.nCurFrame += 1;
        if (as.nCurFrame >= as.nNrOfFrames) {
            as.nCurFrame = 0;
        }
    }
}

void RenderSequence( sAnimatedSequence &as, int x, int y, int scale = 1, bool bBorder = false ) {
    // assumption that the sequence sub sprites are arranged horizontally
    olc::vi2d curTile = { as.nCurFrame % as.tilesX, as.nCurFrame / as.tilesX };
    DrawPartialSprite( x, y, as.spritePtr, (as.tileOffsetX + curTile.x) * as.subSprWidth, (as.tileOffsetY + curTile.y) * as.subSprHeight, as.subSprWidth, as.subSprHeight, scale );
    if (bBorder) {
        DrawRect( x - 1, y - 1, as.subSprWidth * scale + 2, as.subSprHeight * scale + 2, olc::RED );
    }
}

public:
    AnimationEngine() {
        sAppName = "AnimationEngine";
    }

private:
    sAnimatedSequence anSeq;
    olc::Sprite *glbSprite = nullptr;

public:
    bool OnUserCreate() override {
        bool bSucces = true;

        std::string sFileName;
        sFileName.append( "sprite sheet - animated" );
        sFileName.append( ".png" );

        glbSprite = new olc::Sprite( sFileName );
        if (glbSprite->width == 0) {
            std::cout << "ERROR: OnUserCreate() --> sprite loading failed for file: " << sFileName << std::endl;
        } else {
            InitSequence(
                anSeq, glbSprite,  // Initial values:
                 8,                //   * nr of frames
                64, 64,            //   * size of subsprites in spritesheet
                 0,  0,            //   * offset of sequence in tiles
                 4,  2,            //   * dimensions of tile grid where sequence can be found in sprite
                 0.2f              //   * initial tick time (in seconds)
            );
        }

        return bSucces;
    }

    // This function increases or decreases var by stepSize within the limits minVal and maxVal, depending on the
    // combination of keys. holdKey is associated with this var, kUp and kDown are for increasing or decreasing.
    // Keeping SHIFT hold down as well as holdKey enables fast changing.
    //   * var            - the reference variable to be modified if the correct key combination is pressed
    //   * kHold          - the key that is associated with this variable (must be held icw kDown/kUp)
    //   * kDn, kUp       - the keys to decrease, increase the variable
    //   * stepSize       - the amount of decrease, increase
    //   * minVal, maxVal - the result value of &var is clamped within these values
    void VarOnKeyCombi_int( int &var, olc::Key kHold, olc::Key kDn, olc::Key kUp, int stepSize, int minVal, int maxVal ) {
        if (GetKey( kHold ).bHeld) {
            if (GetKey( kUp ).bPressed || (GetKey( olc::SHIFT).bHeld && GetKey( kUp ).bHeld)) var = std::min( maxVal, var + stepSize );
            if (GetKey( kDn ).bPressed || (GetKey( olc::SHIFT).bHeld && GetKey( kDn ).bHeld)) var = std::max( minVal, var - stepSize );
        }
    }
    // the same as var_int_input(), but for a float type var, with float stepSize, minVal and maxVal
    void VarOnKeyCombi_float( float &var, olc::Key kHold, olc::Key kDn, olc::Key kUp, float stepSize, float minVal, float maxVal ) {
        if (GetKey( kHold ).bHeld) {
            if (GetKey( kUp ).bPressed || (GetKey( olc::SHIFT).bHeld && GetKey( kUp ).bHeld)) var = std::min( maxVal, var + stepSize );
            if (GetKey( kDn ).bPressed || (GetKey( olc::SHIFT).bHeld && GetKey( kDn ).bHeld)) var = std::max( minVal, var - stepSize );
        }
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // get updates for the control variables for the animation
        VarOnKeyCombi_int(   anSeq.nNrOfFrames , olc::F1, olc::NP_SUB, olc::NP_ADD, 1,      1,       64      );
        VarOnKeyCombi_int(   anSeq.tileOffsetX , olc::F2, olc::NP_SUB, olc::NP_ADD, 1,      0,       16      );
        VarOnKeyCombi_int(   anSeq.tileOffsetY , olc::F3, olc::NP_SUB, olc::NP_ADD, 1,      0,       12      );
        VarOnKeyCombi_int(   anSeq.subSprWidth , olc::F4, olc::NP_SUB, olc::NP_ADD, 1,      1,      384      );
        VarOnKeyCombi_int(   anSeq.subSprHeight, olc::F5, olc::NP_SUB, olc::NP_ADD, 1,      1,      384      );
        VarOnKeyCombi_float( anSeq.fTickTime   , olc::F6, olc::NP_SUB, olc::NP_ADD, 0.001f, 0.001f,   2.000f );
        VarOnKeyCombi_int(   m_scale           , olc::F7, olc::NP_SUB, olc::NP_ADD, 1,      1,        4      );
        VarOnKeyCombi_int(   anSeq.tilesX      , olc::F8, olc::NP_SUB, olc::NP_ADD, 1,      1,       16      );
        VarOnKeyCombi_int(   anSeq.tilesY      , olc::F9, olc::NP_SUB, olc::NP_ADD, 1,      1,       16      );

        Clear( olc::VERY_DARK_GREEN );
        SetPixelMode( olc::Pixel::MASK );

        // draw the sprite sheet with a border around it
        DrawSprite( 1, 1, glbSprite );
        DrawRect( 0, 0, glbSprite->width + 2, glbSprite->height + 2, olc::YELLOW );

        // update and render the animated sequence, and draw a border around the subsprite
        UpdateSequence( anSeq, fElapsedTime );
        RenderSequence( anSeq, SCREEN_X - 350, 200, m_scale, true );

        DrawRect( 1 + (anSeq.tileOffsetX + anSeq.nCurFrame % anSeq.tilesX) * anSeq.subSprWidth,
                  1 + (anSeq.tileOffsetY + anSeq.nCurFrame / anSeq.tilesX) * anSeq.subSprHeight, anSeq.subSprWidth, anSeq.subSprHeight, olc::RED );
        SetPixelMode( olc::Pixel::NORMAL );

        DrawString( SCREEN_X - 350,  10, "Press F-key in combination with NP_SUB/NP_ADD" );
        DrawString( SCREEN_X - 350,  20, "to change value  [Use left SHIFT to speed up]" );
        DrawString( SCREEN_X - 350,  40, "      vFrames.size() = " + std::to_string( anSeq.vFrames.size() ));
        DrawString( SCREEN_X - 350,  50, "F01 - nNrOfFrames    = " + std::to_string( anSeq.nNrOfFrames    ));
        DrawString( SCREEN_X - 350,  60, "F02 - tileOffsetX    = " + std::to_string( anSeq.tileOffsetX    ));
        DrawString( SCREEN_X - 350,  70, "F03 - tileOffsetY    = " + std::to_string( anSeq.tileOffsetY    ));
        DrawString( SCREEN_X - 350,  80, "F04 - subSprWidth    = " + std::to_string( anSeq.subSprWidth    ));
        DrawString( SCREEN_X - 350,  90, "F05 - subSprHeight   = " + std::to_string( anSeq.subSprHeight   ));
        DrawString( SCREEN_X - 350, 100, "      nCurFrame      = " + std::to_string( anSeq.nCurFrame      ));
        DrawString( SCREEN_X - 350, 110, "F06 - fTickTime      = " + std::to_string( anSeq.fTickTime      ));
        DrawString( SCREEN_X - 350, 120, "      fCumTime       = " + std::to_string( anSeq.fCumTime       ));
        DrawString( SCREEN_X - 350, 130, "F07 - scale          = " + std::to_string( m_scale              ));
        DrawString( SCREEN_X - 350, 140, "F08 - tile grid X    = " + std::to_string( anSeq.tilesX         ));
        DrawString( SCREEN_X - 350, 150, "F09 - tile grid Y    = " + std::to_string( anSeq.tilesY         ));

        return true;
    }
    private:
        int m_scale = 1;
};

int main()
{
	AnimationEngine demo;
	if (demo.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
		demo.Start();

	return 0;
}

