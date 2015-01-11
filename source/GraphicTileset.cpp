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
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

CGraphicTileset::CGraphicTileset(){
    DTileset = NULL;
    DPixbufTileset = NULL;
    DTileCount = 0;
    DTileWidth = 0;
    DTileHeight = 0;
}

CGraphicTileset::~CGraphicTileset(){
    if(NULL != DTileset){
        g_object_unref(G_OBJECT(DTileset));      
    }
    if(NULL != DTileset){
        g_object_unref(G_OBJECT(DPixbufTileset));      
    }
}

int CGraphicTileset::FindTile(const std::string &tilename) const{
    std::map< std::string, int >::const_iterator Iterator = DMapping.find(tilename);
    if(DMapping.end() != Iterator){
        return Iterator->second;
    }
    printf("Unknown tilename %s\n",tilename.c_str());
    return -1;
}

bool CGraphicTileset::LoadTileset(GdkDrawable *drawable, GdkGC *gc, const std::string &filename){
    char *TempBuffer = NULL;
    size_t BufferSize = 0;
    int LastChar;
    FILE *FilePointer;
    bool ReturnStatus = false;
    
    FilePointer = fopen(filename.c_str(), "r");
    if(NULL == FilePointer){
        printf("Failed to load file %s\n", filename.c_str());
        return false;   
    }
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        printf("Getline failed for path.\n");
        goto LoadTilesetExit1;
    }
    LastChar = strlen(TempBuffer) - 1;
    while((0 <= LastChar)&&(('\r' == TempBuffer[LastChar])||('\n' == TempBuffer[LastChar]))){
        TempBuffer[LastChar] = '\0';
        LastChar--;
    }
    DPixbufTileset = gdk_pixbuf_new_from_file((gchar *)TempBuffer, NULL);
    if(NULL == DPixbufTileset){
        printf("Failed to load file %s.\n", TempBuffer);
        goto LoadTilesetExit1;        
    }
    DTileWidth = gdk_pixbuf_get_width(DPixbufTileset);
    DTileHeight = gdk_pixbuf_get_height(DPixbufTileset);
    
    DTileset = gdk_pixmap_new(drawable, DTileWidth, DTileHeight, -1);
    if(NULL == DTileset){
        goto LoadTilesetExit1;        
    }
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        goto LoadTilesetExit1;
    }

    gdk_draw_pixbuf(DTileset, gc, DPixbufTileset, 0, 0, 0, 0, DTileWidth, DTileHeight, GDK_RGB_DITHER_NONE, 0, 0);
    DTileCount = atoi(TempBuffer);
    DTileHeight /= DTileCount;
    for(int Index = 0; Index < DTileCount; Index++){
        if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
            goto LoadTilesetExit1;
        }
        LastChar = strlen(TempBuffer) - 1;
        while((0 <= LastChar)&&(('\r' == TempBuffer[LastChar])||('\n' == TempBuffer[LastChar]))){
            TempBuffer[LastChar] = '\0';
            LastChar--;
        }
        DMapping[std::string(TempBuffer)] = Index;
    }

    ReturnStatus = true;

LoadTilesetExit1:
    free(TempBuffer);
    fclose(FilePointer);
    return ReturnStatus;
}

void CGraphicTileset::DrawTile(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, int tileindex){
    if((0 > tileindex)||(tileindex >= DTileCount)){
        return;
    }
    gdk_draw_pixbuf(drawable, gc, DPixbufTileset, 0, tileindex * DTileHeight, xpos, ypos, DTileWidth, DTileHeight, GDK_RGB_DITHER_NONE, 0, 0);
    
//    gdk_draw_pixmap(drawable, gc, DTileset, 0, tileindex * DTileHeight, xpos, ypos, DTileWidth, DTileHeight);
}

void CGraphicTileset::DrawPixelCorners(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, int tileindex){
    if((0 > tileindex)||(tileindex >= DTileCount)){
        return;
    }
    gdk_draw_pixbuf(drawable, gc, DPixbufTileset, 0, tileindex * DTileHeight, xpos, ypos, 1, 1, GDK_RGB_DITHER_NONE, 0, 0);
    gdk_draw_pixbuf(drawable, gc, DPixbufTileset, DTileWidth-1, tileindex * DTileHeight, xpos+1, ypos, 1, 1, GDK_RGB_DITHER_NONE, 0, 0);
    gdk_draw_pixbuf(drawable, gc, DPixbufTileset, 0, tileindex * DTileHeight + DTileHeight - 1, xpos, ypos+1, 1, 1, GDK_RGB_DITHER_NONE, 0, 0);
    gdk_draw_pixbuf(drawable, gc, DPixbufTileset, DTileWidth-1, tileindex * DTileHeight + DTileHeight - 1, xpos+1, ypos+1, 1, 1, GDK_RGB_DITHER_NONE, 0, 0);
    
}
