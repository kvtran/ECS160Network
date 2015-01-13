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

/**
 * CGraphicTileset class, used for tile set operations.
 */
class CGraphicTileset{
    protected:
        GdkPixmap *DTileset; /*!< Pixel Map for tile set. */
        GdkPixbuf *DPixbufTileset; /*!< Tile set for client side. */
        std::map< std::string, int > DMapping; /*!< Map between tilename and index. */
        int DTileCount; /*!< Tile count. */      
        int DTileWidth; /*!< Tile width. */
        int DTileHeight; /*!< Tile height. */
        
    public:
        /**
         * Empty constructor, initializes everythign to NULL or 0.
         */
        CGraphicTileset();

        /**
         * Graphic Tileset destructor.
         */
        ~CGraphicTileset();

        /**
         * Getter function for tile count.
         */
        int TileCount() const{
            return DTileCount;
        };

        /**
         * Getter function for tile width.
         */
        int TileWidth() const{
            return DTileWidth;
        };

        /**
         * Getter function for tile height.
         */
        int TileHeight() const{
            return DTileHeight;
        };

        /**
         * Gets tile index if tilename is in map.
         *
         * @param tilename tile name
         */
        int FindTile(const std::string &tilename) const;

        /**
         * Loads Tileset.
         * 
         * @param drawable A GdkDrawable, used to determine default values for the new pixmap
         * @param gc Graphics context that holds information about how things are drawn
         * @param filename Name of file to load Tileset from
         */
        bool LoadTileset(GdkDrawable *drawable, GdkGC *gc, const std::string &filename);
        
        /**
         * Draws Tile if tileindex is between 0 and DTileCount.
         *
         * @param drawable destination drawable to draw tile
         * @param gc Graphics context, used for clipping
         * @param xpos Destination X coord inside drawable
         * @param ypos Destination Y coord inside drawable
         * @param tileindex index of tile
         */
        void DrawTile(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, int tileindex);

        /**
         * Draws Tile corners if tileindex is between 0 and DTileCount.
         *
         * @param drawable destination drawable to draw tile
         * @param gc Graphics context, used for clipping
         * @param xpos Destination X coord inside drawable
         * @param ypos Destination Y coord inside drawable
         * @param tileindex index of tile
         */
        void DrawPixelCorners(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, int tileindex);
};

#endif

