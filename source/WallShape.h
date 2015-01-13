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

/**
 * WallShape class
 */
class CWallShape{
    protected:
        std::vector< std::vector< bool > > DLocations; /*!< Polyomino square locations */
        int DOrientation; /*!< Polyomino orientation */ 
        
    public:
        /**
         * WallShape constructor.
         */
        CWallShape();

        /**
         * WallShape destructor.
         */
        ~CWallShape();
        
        /**
         * Gets width of polyomino.
         */
        int Width();

        /**
         * Gets height of polyomino. 
         */
        int Height();
        
        /**
         * Sets type and orientation of polyomino.
         *
         * @param val Seed value
         */
        void Randomize(unsigned int val);
        
        /**
         * Changes polyomino orientation.
         */
        void Rotate();
        
        /**
         * Determines if part of the polyomino is contained at the position specified.
         *
         * @param x x position
         * @param y y position
         *
         * @returns True if position contains part of polyomino, false if it does not
         */
        bool IsBlock(int x, int y);
};

#endif
