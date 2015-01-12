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
#include "TerrainMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
        
/**
 * Terrain Map empty constructor
 * 
 * Initialize map to NULL
 */
CTerrainMap::CTerrainMap(){
    D2DTileset = NULL;
    D3DTileset = NULL;
}

/**
 * Terrain Map constructor
 *
 * @param &map Map used to initialize TerrainMap
 */
CTerrainMap::CTerrainMap(const CTerrainMap &map){
    D2DTileset = map.D2DTileset;
    D3DTileset = map.D3DTileset;
    D2DMap = map.D2DMap;
    D3DMap = map.D3DMap;
    DTileTypeMap = map.DTileTypeMap;
    DStringMap = map.DStringMap;
    DCastleLocations = map.DCastleLocations;
    DPlayerCount = map.DPlayerCount;
    DMapName = map.DMapName;
}

/**
 * Terrain Map destructor
 */
CTerrainMap::~CTerrainMap(){

}

/**
 *  Operator overload function for '='
 */
CTerrainMap &CTerrainMap::operator=(const CTerrainMap &map){
    if(this != &map){
        D2DTileset = map.D2DTileset;
        D3DTileset = map.D3DTileset;
        D2DMap = map.D2DMap;
        D3DMap = map.D3DMap;
        DTileTypeMap = map.DTileTypeMap;
        DStringMap = map.DStringMap;
        DCastleLocations = map.DCastleLocations;
        DPlayerCount = map.DPlayerCount;
        DMapName = map.DMapName;   
    }
    return *this;
}

/**
 * Getter function for DMapName
 */
std::string CTerrainMap::MapName() const{
    return DMapName;   
}

/**
 * Getter function for DPlayerCount
 */
int CTerrainMap::PlayerCount() const{
    return DPlayerCount;
}

/**
 * Getter function for castle count
 */ 
int CTerrainMap::CastleCount() const{
    return DCastleLocations.size() / 2;
}

bool CTerrainMap::CastleLocation(int index, int &xpos, int &ypos) const{
    index *= 2;
    if((0 > index)||(index >= DCastleLocations.size())){
        return false;
    }
    xpos = DCastleLocations[index];
    ypos = DCastleLocations[index + 1];
    return true;
}

/**
 * Gets width of map
 */
int CTerrainMap::Width() const{
    if(D2DMap.size()){
        return D2DMap[0].size();
    }
    return 0;
}

/**
 * Gets height of map
 */
int CTerrainMap::Height() const{
    return D2DMap.size();
}

EPlayerColor CTerrainMap::TileType(int xindex, int yindex) const{
    if((0 > xindex)||(0 > yindex)){
        return pcMax;   
    }
    if(DTileTypeMap.size() <= yindex){
        return pcMax;    
    }
    if(DTileTypeMap[yindex].size() <= xindex){
        return pcMax;   
    }
    return DTileTypeMap[yindex][xindex];
}

bool CTerrainMap::LoadMap(CGraphicTileset *tileset2d, CGraphicTileset *tileset3d, const std::string &filename){
    std::vector< std::string > WaterNames2D, WaterNames3D;
    char *TempBuffer = NULL;
    size_t BufferSize = 0;
    int LastChar, MapWidth, MapHeight, CastleCount;
    FILE *FilePointer;
    bool ReturnStatus = false;
    int PlayerColorsFound = 0;
    
    D2DTileset = tileset2d;
    D3DTileset = tileset3d;
    D2DMap.clear();
    
    FilePointer = fopen(filename.c_str(), "r");
    if(NULL == FilePointer){
        printf("Failed to load file %s\n", filename.c_str());
        return false;   
    }
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        printf("Failed to read map line.\n");
        goto LoadMapExit;
    }
    DMapName = std::string(TempBuffer);
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        printf("Failed to read map line.\n");
        goto LoadMapExit;
    }
    if(2 != sscanf(TempBuffer, "%d%d",&MapWidth, &MapHeight)){
        printf("Invalid map dimensions.\n");
        goto LoadMapExit;
    }
    if((8 > MapWidth)||(8 > MapHeight)){
        printf("Invalid map dimensions.\n");
        goto LoadMapExit;        
    }
    while(!feof(FilePointer) && (DStringMap.size() < MapHeight + 2)){
        if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
            if(!feof(FilePointer)){
                printf("Failed to read map line.\n");
                goto LoadMapExit;
            }
            else{
                break;   
            }
        }
        LastChar = strlen(TempBuffer) - 1;
        while((0 <= LastChar)&&(('\r' == TempBuffer[LastChar])||('\n' == TempBuffer[LastChar]))){
            TempBuffer[LastChar] = '\0';
            LastChar--;
        }
        DStringMap.push_back(std::string(TempBuffer));
        if(MapWidth + 2 > DStringMap.back().length()){
            printf("Map line %zd too short!\n", DStringMap.size());
            goto LoadMapExit;
        }
    }
    if(MapHeight + 2 > DStringMap.size()){
        printf("Map has too few lines!\n");
        goto LoadMapExit;
    }
    D2DMap.resize(MapHeight);
    D3DMap.resize(MapHeight);
    DTileTypeMap.resize(MapHeight);
    for(int Index = 0; Index < D2DMap.size(); Index++){
        D2DMap[Index].resize(MapWidth);
        D3DMap[Index].resize(MapWidth);
        DTileTypeMap[Index].resize(MapWidth);
    }
    for(int Index = 0; Index < DStringMap.size(); Index++){
        std::string::iterator Iter = DStringMap[Index].begin();
        while(DStringMap[Index].end() != Iter){
            switch(*Iter){
                case 'B':   PlayerColorsFound |= 0x01;
                            break;
                case 'R':   PlayerColorsFound |= 0x02;
                            break;
                case 'Y':   PlayerColorsFound |= 0x04;
                            break;
                default:    break;
            }
            Iter++;   
        }
    }
    switch(PlayerColorsFound){
        case 3: // 2 player B & R
                DPlayerCount = 2;
                break;
        case 7: // 3 player B, R, & Y
                DPlayerCount = 3;
                break;
        case 5: 
        case 6: // 2 player need shiftdown
                for(int Index = 0; Index < DStringMap.size(); Index++){
                    std::string::iterator Iter = DStringMap[Index].begin();
                    while(DStringMap[Index].end() != Iter){
                        switch(*Iter){
                            case 'R':   *Iter = 'B';  
                                        break;
                            case 'Y':   *Iter = 'R';
                                        break;
                            default:    break;
                        }
                        Iter++;   
                    }
                }
                DPlayerCount = 2;
                break;
        default:    printf("Map has too few players!\n");
                    goto LoadMapExit;
    }
    
    WaterNames2D.resize(16);
    WaterNames2D[0] = "water";
    WaterNames2D[1] = "shore-n";
    WaterNames2D[2] = "shore-e";
    WaterNames2D[3] = "shore-ne";
    WaterNames2D[4] = "shore-s";
    WaterNames2D[5] = "shore-n";
    WaterNames2D[6] = "shore-se";
    WaterNames2D[7] = "shore-e";
    WaterNames2D[8] = "shore-w";
    WaterNames2D[9] = "shore-nw";
    WaterNames2D[10] = "shore-e";
    WaterNames2D[11] = "shore-n";
    WaterNames2D[12] = "shore-sw";
    WaterNames2D[13] = "shore-w";
    WaterNames2D[14] = "shore-s";
    WaterNames2D[15] = "water";
    WaterNames3D.resize(16);
    WaterNames3D[0] = "water-0";
    WaterNames3D[1] = "shore-n-0";
    WaterNames3D[2] = "shore-e-0";
    WaterNames3D[3] = "shore-ne-0";
    WaterNames3D[4] = "shore-s-0";
    WaterNames3D[5] = "shore-n-0";
    WaterNames3D[6] = "shore-se-0";
    WaterNames3D[7] = "shore-e-0";
    WaterNames3D[8] = "shore-w-0";
    WaterNames3D[9] = "shore-nw-0";
    WaterNames3D[10] = "shore-e-0";
    WaterNames3D[11] = "shore-n-0";
    WaterNames3D[12] = "shore-sw-0";
    WaterNames3D[13] = "shore-w-0";
    WaterNames3D[14] = "shore-s-0";
    WaterNames3D[15] = "water-0";
    
    for(int YPos = 0; YPos < D2DMap.size(); YPos++){
        for(int XPos = 0; XPos < D2DMap[YPos].size(); XPos++){
            bool IsEven = (YPos + XPos) & 0x01 ? false : true;
            
            switch(DStringMap[YPos+1][XPos+1]){
                case 'B': DTileTypeMap[YPos][XPos] = pcBlue;
                          break;
                case 'R': DTileTypeMap[YPos][XPos] = pcRed;
                          break;
                case 'Y': DTileTypeMap[YPos][XPos] = pcYellow;
                          break;
                default:  DTileTypeMap[YPos][XPos] = pcNone;
                          break;
            }
            
            if(DStringMap[YPos+1][XPos+1] == ' '){
                int WaterType = 0;
                
                if(DStringMap[YPos+1][XPos] != ' '){
                    WaterType |= 0x8;
                }
                if(DStringMap[YPos][XPos+1] != ' '){
                    WaterType |= 0x1;
                }
                if(DStringMap[YPos+1][XPos+2] != ' '){
                    WaterType |= 0x2;
                }
                if(DStringMap[YPos+2][XPos+1] != ' '){
                    WaterType |= 0x4;
                }
                if(WaterType && (WaterType != 15)){
                    D2DMap[YPos][XPos] = D2DTileset->FindTile(WaterNames2D[WaterType] + (IsEven ? "-even" : "-odd"));
                    D3DMap[YPos][XPos] = D3DTileset->FindTile(WaterNames3D[WaterType]);
                }
                else{
                    WaterType = 0;
                    if(DStringMap[YPos][XPos] != ' '){
                        WaterType |= 0x8;
                    }
                    if(DStringMap[YPos+2][XPos] != ' '){
                        WaterType |= 0x4;
                    }
                    if(DStringMap[YPos][XPos+2] != ' '){
                        WaterType |= 0x1;
                    }
                    if(DStringMap[YPos+2][XPos+2] != ' '){
                        WaterType |= 0x2;
                    }
                    switch(WaterType){
                        case 1:     D2DMap[YPos][XPos] = D2DTileset->FindTile("shore-ne");
                                    D3DMap[YPos][XPos] = D3DTileset->FindTile("shore-nec-0");
                                    break;
                        case 2:     D2DMap[YPos][XPos] = D2DTileset->FindTile("shore-se");
                                    D3DMap[YPos][XPos] = D3DTileset->FindTile("shore-sec-0");
                                    break;
                        case 4:     D2DMap[YPos][XPos] = D2DTileset->FindTile("shore-sw");
                                    D3DMap[YPos][XPos] = D3DTileset->FindTile("shore-swc-0");
                                    break;
                        case 8:     D2DMap[YPos][XPos] = D2DTileset->FindTile("shore-nw");
                                    D3DMap[YPos][XPos] = D3DTileset->FindTile("shore-nwc-0");
                                    break;
                        case 5:     D2DMap[YPos][XPos] = D2DTileset->FindTile("shore-nesw");
                                    D3DMap[YPos][XPos] = D3DTileset->FindTile("shore-nesw-0");
                                    break;
                        case 10:    D2DMap[YPos][XPos] = D2DTileset->FindTile("shore-nwse");
                                    D3DMap[YPos][XPos] = D3DTileset->FindTile("shore-nwse-0");
                                    break;
                        default:    D2DMap[YPos][XPos] = D2DTileset->FindTile("water");
                                    D3DMap[YPos][XPos] = D3DTileset->FindTile("water-0");
                                    break;
                    }
                }
            }
            else{
                if(IsEven){
                    D2DMap[YPos][XPos] = D2DTileset->FindTile("grass-even");
                }
                else{
                    D2DMap[YPos][XPos] = D2DTileset->FindTile("grass-odd");
                }
                D3DMap[YPos][XPos] = D3DTileset->FindTile("grass-0");
            }
        }
    }
    
    
    if(1 != fscanf(FilePointer, "%d", &CastleCount)){
        printf("Invalid castle count.\n");
        goto LoadMapExit;
    }
    DCastleLocations.resize(CastleCount*2);
    for(int Index = 0; Index < CastleCount; Index++){
        int XPos, YPos;
        
        if(2 != fscanf(FilePointer, "%d%d", &XPos, &YPos)){
            printf("Invalid castle location.\n");
            goto LoadMapExit;
        }
        if((0 > XPos)||(0 > YPos)||(XPos >= MapWidth)||(YPos >= MapHeight)){
            printf("Invalid castle location.\n");
            goto LoadMapExit;            
        }
        DCastleLocations[Index * 2] = XPos;
        DCastleLocations[Index * 2 + 1] = YPos;
    }
    for(int Index = 0; Index < CastleCount; Index++){
        for(int Inner = Index + 1; Inner < CastleCount; Inner++){
            bool Swap = false;
            if(DCastleLocations[Index * 2 + 1] > DCastleLocations[Inner * 2 + 1]){
                Swap = true;
            }
            else if((DCastleLocations[Index * 2 + 1] == DCastleLocations[Inner * 2 + 1])&&(DCastleLocations[Index * 2] > DCastleLocations[Inner * 2])){
                Swap = true;
            }
            if(Swap){
                int TempX = DCastleLocations[Index * 2];
                int TempY = DCastleLocations[Index * 2 + 1];
                
                DCastleLocations[Index * 2] = DCastleLocations[Inner * 2];
                DCastleLocations[Index * 2 + 1] = DCastleLocations[Inner * 2 + 1];
                
                DCastleLocations[Inner * 2] = TempX;
                DCastleLocations[Inner * 2 + 1] = TempY;
            }
        }
    }
    
    
    ReturnStatus = true;

LoadMapExit:
    free(TempBuffer);
    fclose(FilePointer);
    return ReturnStatus;
}

void CTerrainMap::DrawPreviewMap(GdkDrawable *drawable, GdkGC *gc, gint xoff, gint yoff){
    for(int YIndex = 0, YPos = yoff; YIndex < D2DMap.size(); YIndex++, YPos += 2){
        for(int XIndex = 0, XPos = xoff; XIndex < D2DMap[YIndex].size(); XIndex++, XPos += 2){
            D2DTileset->DrawPixelCorners(drawable, gc, XPos, YPos, D2DMap[YIndex][XIndex]);
        }
    }
}

void CTerrainMap::Draw2DMap(GdkDrawable *drawable, GdkGC *gc){
    int TileWidth, TileHeight;
    
    TileWidth = D2DTileset->TileWidth();
    TileHeight = D2DTileset->TileHeight();
    for(int YIndex = 0, YPos = 0; YIndex < D2DMap.size(); YIndex++, YPos += TileHeight){
        for(int XIndex = 0, XPos = 0; XIndex < D2DMap[YIndex].size(); XIndex++, XPos += TileWidth){
            D2DTileset->DrawTile(drawable, gc, XPos, YPos, D2DMap[YIndex][XIndex]);
        }
    }
}

void CTerrainMap::Draw3DMap(GdkDrawable *drawable, GdkGC *gc, int winddir, int totalsteps, int timestep){
    int TileWidth, TileHeight;
    
    TileWidth = D3DTileset->TileWidth();
    TileHeight = D3DTileset->TileHeight();
    for(int YIndex = 0, YPos = 0; YIndex < D3DMap.size(); YIndex++, YPos += TileHeight){
        for(int XIndex = 0, XPos = 0; XIndex < D3DMap[YIndex].size(); XIndex++, XPos += TileWidth){
            D3DTileset->DrawTile(drawable, gc, XPos, YPos, D3DMap[YIndex][XIndex] + (winddir * totalsteps) + timestep);
        }
    }
}

