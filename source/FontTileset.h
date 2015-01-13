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

/**
 * CFontTileset class, used to write text.
 */
class CFontTileset : public CGraphicTileset{
    protected:
        std::vector< gint > DCharacterWidths; /*!< vector of character widths. */
        std::vector< std::vector < gint > > DDeltaWidths; /*!< vector or vectors of delta widths. */
        
    public:
        /**
         * Empty constructor, currently does nothing.
         */
        CFontTileset();

        /**
         * Font Tileset destructor.
         */
        ~CFontTileset();
        
        /**
         * Loads Font and Tileset.
         * 
         * @param drawable A GdkDrawable, used to determine default values for the new pixmap
         * @param gc Graphics context that holds information about how things are drawn
         * @param filename Name of file to load Tileset from
         */
        bool LoadFont(GdkDrawable *drawable, GdkGC *gc, const std::string &filename);
        
        /**
         * Draws string by callign DrawTile where each letter is a tile.
         *
         * @param drawable destination drawable to draw tile
         * @param gc Graphics context, used for clipping
         * @param xpos Destination X coord inside drawable
         * @param ypos Destination Y coord inside drawable
         * @param str string to be drawn
         */
        void DrawText(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, const std::string &str);

        /**
         * Finds the length and width of str if it were drawn.
         *
         * @param str string to be measured
         * @param width the gint that will be altered to match the width of the text
         * @param height the gint that will be altered to match the height of the text
         */
        void MeasureText(const std::string &str, gint &width, gint &height);
};

#endif

