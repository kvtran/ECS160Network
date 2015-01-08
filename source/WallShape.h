#ifndef WALLSHAPE_H
#define WALLSHAPE_H
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
#include <vector>

class CWallShape{
    protected:
        std::vector< std::vector< bool > > DLocations;
        int DOrientation;
        
    public:
        CWallShape();
        ~CWallShape();
        
        int Width();
        int Height();
        
        void Randomize(unsigned int val);
        
        void Rotate();
        
        bool IsBlock(int x, int y);
};

#endif