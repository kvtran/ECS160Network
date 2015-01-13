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
        CGraphicTileset *D2DTileset;
        CGraphicTileset *D3DTileset;
        std::vector< std::vector< int > > D2DMap;
        std::vector< std::vector< int > > D3DMap;
        std::vector< std::vector< EPlayerColor > > DTileTypeMap;
        std::vector< std::string > DStringMap;
        std::vector< int > DCastleLocations;
        int DPlayerCount;
        std::string DMapName;
        
    public:
        CTerrainMap();
        CTerrainMap(const CTerrainMap &map);
        ~CTerrainMap();
        
        CTerrainMap &operator=(const CTerrainMap &map);
        
        std::string MapName() const;
        int PlayerCount() const;
        int CastleCount() const;
        bool CastleLocation(int index, int &xpos, int &ypos) const;
        
        int Width() const;
        int Height() const;
        
        EPlayerColor TileType(int xindex, int yindex) const;
        
        bool LoadMap(CGraphicTileset *tileset2d, CGraphicTileset *tileset3d, const std::string &filename);
        
        void DrawPreviewMap(GdkDrawable *drawable, GdkGC *gc, gint xoff, gint yoff);
        void Draw2DMap(GdkDrawable *drawable, GdkGC *gc);
        void Draw3DMap(GdkDrawable *drawable, GdkGC *gc, int winddir, int totalsteps, int timestep);
};

#endif

