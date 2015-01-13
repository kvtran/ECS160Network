#ifndef TERRAINMAP_H
#define TERRAINMAP_H
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
#include "GameDataTypes.h"
#include <vector>

/**
 * CTerrainMap class, used to build game map.
 */
class CTerrainMap{
    protected:
        CGraphicTileset *D2DTileset; /*!< 2D tile set. */
        CGraphicTileset *D3DTileset; /*!< 3D tile set. */
        std::vector< std::vector< int > > D2DMap; /*!< 2D map */
        std::vector< std::vector< int > > D3DMap; /*!< 3D map */
        std::vector< std::vector< EPlayerColor > > DTileTypeMap; /*!< Tile type map */
        std::vector< std::string > DStringMap; /*!< String map */
        std::vector< int > DCastleLocations; /*!< Castle locations */
        int DPlayerCount; /*!< Player count */
        std::string DMapName; /*!< Map name */
        
    public:
	/**
     	 * Empty constructor, initialize map to NULL.
	 */
        CTerrainMap();

	/**
	 * Terrain Map constructor.
	 *
	 * @param map Used to initialized terrain map
	 */
        CTerrainMap(const CTerrainMap &map);

	/**
   	 * Terrain Map destructor.
	 */
        ~CTerrainMap();
        
	/**
	 * Operator overload =.
	 */
        CTerrainMap &operator=(const CTerrainMap &map);
        
	/**
	 * Getter function for DMapName.
	 */
        std::string MapName() const;

	/**
	 * Getter function for DPlayerCount.
	 */
        int PlayerCount() const;

	/**
	 * Returns castle count.
	 */
        int CastleCount() const;

	/**
	 * Sets castle location.
	 *
	 * @param index Index of castle in DCastleLocations
	 * @param xpos Set if given index exists
	 * @param ypos Set if given index exists
	 * @return False if given index is not in constraints of array DCastleLocations, and true if it is - xpos, ypos set only if false is not returned.
	 */
        bool CastleLocation(int index, int &xpos, int &ypos) const;
        
	/**
	 * Gets width of map.
	 */
        int Width() const;

	/**
	 * Gets height of map.
	 */
        int Height() const;
        
	/**
	 * Gets tile type if x and y indices are within constraints of DTileTypeMap.
	 * @param xindex xindex
	 * @param yindex yindex
	 */
        EPlayerColor TileType(int xindex, int yindex) const;
        
	/**
	 * Loads map
	 *
	 * @param tileset2d 2D tile set
	 * @param tileset3d 3D tile set
	 * @param filename Map to load
	 */
        bool LoadMap(CGraphicTileset *tileset2d, CGraphicTileset *tileset3d, const std::string &filename);
        
	/**
	 * Draws preview of map.
	 *
	 * @param drawable The destination to draw the map
	 * @param gc The graphics context to draw the map in
	 * @param xoff Offset x
	 * @param yoff Offset y	
	 */
        void DrawPreviewMap(GdkDrawable *drawable, GdkGC *gc, gint xoff, gint yoff);

	/**
	 * Draws 2D version of the map
	 *
	 * @param drawable The destination to draw the map
	 * @param gc The graphics context to draw the map in
	 */
        void Draw2DMap(GdkDrawable *drawable, GdkGC *gc);

	/**
	 * Draws 3D version of the map
	 *
	 * @param drawable The destination to draw the map
	 * @param gc The graphics context to draw the map in
	 * @param winddir Wind direction
	 * @param totalsteps Total steps
	 * @param timestep Time increment
	 */
        void Draw3DMap(GdkDrawable *drawable, GdkGC *gc, int winddir, int totalsteps, int timestep);
};

#endif

