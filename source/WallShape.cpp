#include "WallShape.h"
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
CWallShape::CWallShape(){
    Randomize(0);
}

CWallShape::~CWallShape(){

}

int CWallShape::Width(){
    if(DOrientation & 0x01){
        return DLocations.size();
    }
    return DLocations[0].size();
}

int CWallShape::Height(){
    if(DOrientation & 0x01){
        return DLocations[0].size();
    }
    return DLocations.size();
}

void CWallShape::Randomize(unsigned int val){
    unsigned int PolyominoType = val % 5;
    
    val /= 5;
    DOrientation = val % 4;
    val /= 4;
    if(0 == PolyominoType){
        // monomino
        DLocations.resize(1);
        DLocations[0].resize(1);
        DLocations[0][0] = true;
    }
    else if(1 == PolyominoType){
        // domino
        DLocations.resize(1);
        DLocations[0].resize(2);
        DLocations[0][0] = true;
        DLocations[0][1] = true;
    }
    else if(2 == PolyominoType){
        // tromino
        if(val & 0x1){
            // XX 
            // X
            DLocations.resize(2);
            DLocations[0].resize(2);
            DLocations[1].resize(2);
            DLocations[0][0] = true;
            DLocations[0][1] = true;
            DLocations[1][0] = true;
            DLocations[1][1] = false;
        }
        else{
            // XXX
            DLocations.resize(1);
            DLocations[0].resize(3);
            DLocations[0][0] = true;
            DLocations[0][1] = true;
            DLocations[0][2] = true;
        }
    }
    else if(3 == PolyominoType){
        // tetromino
        switch(val % 7){
            case 0:     // XXXX
                        DLocations.resize(1);
                        DLocations[0].resize(4);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[0][3] = true;
                        break;
            case 1:     // XX
                        // XX
                        DLocations.resize(2);
                        DLocations[0].resize(2);
                        DLocations[1].resize(2);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = true;
                        break;
            case 2:     // XXX
                        //  X
                        DLocations.resize(2);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = false;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        break;
            case 3:     // XXX
                        //   X
                        DLocations.resize(2);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = false;
                        DLocations[1][1] = false;
                        DLocations[1][2] = true;
                        break;
            case 4:     // XXX
                        // X
                        DLocations.resize(2);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = false;
                        DLocations[1][2] = false;
                        break;
            case 5:     //  XX
                        // XX
                        DLocations.resize(2);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[0][0] = false;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        break;
            default:    // XX
                        //  XX
                        DLocations.resize(2);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = false;
                        DLocations[1][0] = false;
                        DLocations[1][1] = true;
                        DLocations[1][2] = true;
                        break;
        }
    }
    else{
        // pentomino
        switch(val % 18){
            case 0:     // XXXXX
                        DLocations.resize(1);
                        DLocations[0].resize(5);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[0][3] = true;
                        DLocations[0][4] = true;
                        break;
            case 1:     //  XX
                        // XX
                        //  X
                        DLocations.resize(3);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[2].resize(3);
                        DLocations[0][0] = false;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        DLocations[2][0] = false;
                        DLocations[2][1] = true;
                        DLocations[2][2] = false;
                        break;
            case 2:     // XX 
                        //  XX
                        //  X
                        DLocations.resize(3);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[2].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = false;
                        DLocations[1][0] = false;
                        DLocations[1][1] = true;
                        DLocations[1][2] = true;
                        DLocations[2][0] = false;
                        DLocations[2][1] = true;
                        DLocations[2][2] = false;
                        break;
            case 3:     // XXXX
                        //    X
                        DLocations.resize(2);
                        DLocations[0].resize(4);
                        DLocations[1].resize(4);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[0][3] = true;
                        DLocations[1][0] = false;
                        DLocations[1][1] = false;
                        DLocations[1][2] = false;
                        DLocations[1][3] = true;
                        break;
            case 4:     // XXXX
                        // X
                        DLocations.resize(2);
                        DLocations[0].resize(4);
                        DLocations[1].resize(4);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[0][3] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = false;
                        DLocations[1][2] = false;
                        DLocations[1][3] = false;
                        break;
            case 5:     // XXX
                        //  XX
                        DLocations.resize(2);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = false;
                        DLocations[1][1] = true;
                        DLocations[1][2] = true;
                        break;
            case 6:     // XXX
                        // XX
                        DLocations.resize(2);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        break;
            case 7:     // XXX
                        //   XX
                        DLocations.resize(2);
                        DLocations[0].resize(4);
                        DLocations[1].resize(4);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[0][3] = false;
                        DLocations[1][0] = false;
                        DLocations[1][1] = false;
                        DLocations[1][2] = true;
                        DLocations[1][3] = true;
                        break;
            case 8:     //  XXX
                        // XX
                        DLocations.resize(2);
                        DLocations[0].resize(4);
                        DLocations[1].resize(4);
                        DLocations[0][0] = false;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[0][3] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        DLocations[1][3] = false;
                        break;
            case 9:     // XXX
                        //  X
                        //  X
                        DLocations.resize(3);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[2].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = false;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        DLocations[2][0] = false;
                        DLocations[2][1] = true;
                        DLocations[2][2] = false;
                        break;
            case 10:    // XXX
                        // X X
                        DLocations.resize(2);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = false;
                        DLocations[1][2] = true;
                        break;
            case 11:    // XXX
                        // X
                        // X
                        DLocations.resize(3);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[2].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = false;
                        DLocations[1][2] = false;
                        DLocations[2][0] = true;
                        DLocations[2][1] = false;
                        DLocations[2][2] = false;
                        break;
            case 12:    //  XX
                        // XX
                        // X
                        DLocations.resize(3);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[2].resize(3);
                        DLocations[0][0] = false;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = true;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        DLocations[2][0] = true;
                        DLocations[2][1] = false;
                        DLocations[2][2] = false;
                        break;
            case 13:    //  X
                        // XXX
                        //  X
                        DLocations.resize(3);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[2].resize(3);
                        DLocations[0][0] = false;
                        DLocations[0][1] = true;
                        DLocations[0][2] = false;
                        DLocations[1][0] = true;
                        DLocations[1][1] = true;
                        DLocations[1][2] = true;
                        DLocations[2][0] = false;
                        DLocations[2][1] = true;
                        DLocations[2][2] = false;
                        break;
            case 14:    // XXXX
                        //  X
                        DLocations.resize(2);
                        DLocations[0].resize(4);
                        DLocations[1].resize(4);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[0][3] = true;
                        DLocations[1][0] = false;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        DLocations[1][3] = false;
                        break;
            case 15:    // XXXX
                        //   X
                        DLocations.resize(2);
                        DLocations[0].resize(4);
                        DLocations[1].resize(4);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[0][3] = true;
                        DLocations[1][0] = false;
                        DLocations[1][1] = false;
                        DLocations[1][2] = true;
                        DLocations[1][3] = false;
                        break;
            case 16:    //  XX
                        //  X
                        // XX
                        DLocations.resize(3);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[2].resize(3);
                        DLocations[0][0] = false;
                        DLocations[0][1] = true;
                        DLocations[0][2] = true;
                        DLocations[1][0] = false;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        DLocations[2][0] = true;
                        DLocations[2][1] = true;
                        DLocations[2][2] = false;
                        break;
            default:    // XX
                        //  X
                        //  XX
                        DLocations.resize(3);
                        DLocations[0].resize(3);
                        DLocations[1].resize(3);
                        DLocations[2].resize(3);
                        DLocations[0][0] = true;
                        DLocations[0][1] = true;
                        DLocations[0][2] = false;
                        DLocations[1][0] = false;
                        DLocations[1][1] = true;
                        DLocations[1][2] = false;
                        DLocations[2][0] = false;
                        DLocations[2][1] = true;
                        DLocations[2][2] = true;
                        break;
        }
    }
}

void CWallShape::Rotate(){
    DOrientation++;
    DOrientation &= 0x3;
}

bool CWallShape::IsBlock(int x, int y){
    int LookupX, LookupY;
    
    switch(DOrientation){
        case 1:     // 90 degree rotation to the left
                    LookupX = y;
                    LookupY = DLocations.size() -  1 - x;
                    break;
        case 2:     // 180 degree rotation
                    LookupX = DLocations[0].size() -  1 - x;
                    LookupY = DLocations.size() -  1 - y;
                    break;
        case 3:     // 90 degree rotation to the right
                    LookupX = DLocations[0].size() -  1 - y;
                    LookupY = x;
                    break;
        default:    // normal orientation
                    LookupX = x;
                    LookupY = y;
                    break;
    }
    return DLocations[LookupY][LookupX];
}
