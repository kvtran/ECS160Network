#ifndef FONTTILESET_H
#define FONTTILESET_H
/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.
    
    All source material (source code, images, sounds, etc.) have been provided to 
    University of California, Davis students of course ECS 160 for educational 
    purposes. It may not be distributed beyond those enrolled in the course without 
    prior permission from the copyright holder. 
    
    Some sound files, sound fonts, and midi files have been included that were 
    freely available via internet sources. They have been included in this 
    distribution for educational purposes only and this copyright notice does not 
    attempt to claim any ownership of this material.
*/
#include "GraphicTileset.h"
#include <vector>

class CFontTileset : public CGraphicTileset{
    protected:
        std::vector< gint > DCharacterWidths;
        std::vector< std::vector < gint > > DDeltaWidths;
        
    public:
        CFontTileset();
        ~CFontTileset();
        
        bool LoadFont(GdkDrawable *drawable, GdkGC *gc, const std::string &filename);
        
        void DrawText(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, const std::string &str);
        void MeasureText(const std::string &str, gint &width, gint &height);
};

#endif

