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
#include "FontTileset.h"
#include <stdio.h>
#include <stdlib.h>


CFontTileset::CFontTileset(){
    
}

CFontTileset::~CFontTileset(){
    
}

bool CFontTileset::LoadFont(GdkDrawable *drawable, GdkGC *gc, const std::string &filename){
    char *TempBuffer = NULL;
    size_t BufferSize = 0;
    FILE *FilePointer;
    bool ReturnStatus = false;
    
    if(!LoadTileset(drawable, gc, filename)){
        return false;    
    }
    
    FilePointer = fopen(filename.c_str(), "r");
    if(NULL == FilePointer){
        printf("Failed to load file %s\n", filename.c_str());
        return false;   
    }
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        printf("Getline failed for path.\n");
        goto LoadFontExit1;
    }
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        goto LoadFontExit2;
    }
    for(int Index = 0; Index < DTileCount; Index++){
        if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
            goto LoadFontExit2;
        }   
    }
    DCharacterWidths.resize(DTileCount);
    DDeltaWidths.resize(DTileCount);
    for(int Index = 0; Index < DTileCount; Index++){
        int CurWidth;
        if(1 != fscanf(FilePointer, "%d", &CurWidth)){
            goto LoadFontExit2;
        }   
        DCharacterWidths[Index] = CurWidth;
    }
    for(int FromIndex = 0; FromIndex < DTileCount; FromIndex++){
        DDeltaWidths[FromIndex].resize(DTileCount);
        for(int ToIndex = 0; ToIndex < DTileCount; ToIndex++){
            int CurDelta;
            if(1 != fscanf(FilePointer, "%d", &CurDelta)){
                goto LoadFontExit2;
            }   
            DDeltaWidths[FromIndex][ToIndex] = CurDelta;
        }
    }
    ReturnStatus = true;
LoadFontExit2:
    free(TempBuffer);
LoadFontExit1:
    
    fclose(FilePointer);
    return ReturnStatus;
}

void CFontTileset::DrawText(GdkDrawable *drawable, GdkGC *gc, gint xpos, gint ypos, const std::string &str){
    int LastChar, NextChar;
    for(int Index = 0; Index < str.length(); Index++){
        NextChar = str[Index] - ' ';
        
        if(Index){
            xpos += DCharacterWidths[LastChar] + DDeltaWidths[LastChar][NextChar]; 
        }
        DrawTile(drawable, gc, xpos, ypos, NextChar);
        LastChar = NextChar;
    }
}

void CFontTileset::MeasureText(const std::string &str, gint &width, gint &height){
    int LastChar, NextChar;
    width = 0;
    for(int Index = 0; Index < str.length(); Index++){
        NextChar = str[Index] - ' ';
        
        if(Index){
            width += DDeltaWidths[LastChar][NextChar]; 
        }
        width += DCharacterWidths[NextChar]; 
        LastChar = NextChar;
    }
    height = DTileHeight;
}

