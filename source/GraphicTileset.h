#ifndef GRAPHICTILESET_H
#define GRAPHICTILESET_H
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
#include <gtk/gtk.h>
#include <string>
#include <map>

class CGraphicTileset{
    protected:
        GdkPixmap *DTileset;
        GdkPixbuf *DPixbufTileset;
        std::map< std::string, int > DMapping;
        int DTileCount;       
        int DTileWidth;
        int DTileHeight;
        
    public:
        CGraphicTileset();
        ~CGraphicTileset();

        int TileCount() const{
            return DTileCount;
        };
        int TileWidth() const{
            return DTileWidth;
        };
        int TileHeight() const{
            return DTileHeight;
        };
        int FindTile(const std::string &tilename) const;

        bool LoadTileset(GdkDrawable *drawable, GdkGC *gc, const std::string &filename);
        
        void DrawTile(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, int tileindex);
        void DrawPixelCorners(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, int tileindex);
};

#endif

