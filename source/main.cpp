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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>
#include <list>
#include <sstream>
#include <dirent.h>
#include "GraphicTileset.h"
#include "FontTileset.h"
#include "TerrainMap.h"
#include "WallShape.h"
#include "SoundLibraryMixer.h"
#include "RandomNumberGenerator.h"

#define INITIAL_MAP_WIDTH       480
#define INITIAL_MAP_HEIGHT      288
#define TIMEOUT_INTERVAL        50
#define ANIMATION_TIMESTEPS     4
#define WINDDIRECTION_COUNT     9
#define WINDSPEED_COUNT         8
#define STANDARD_GRAVITY        9.80665
#define TIMESTEP_PERIOD         ((double)TIMEOUT_INTERVAL / 500.0)
#define REBUILD_TIME            15
#define BATTLE_TIME             15
#define PLACEMENT_TIME          10
#define SELECT_TIME             15
#define DEBUG                   1
#define RANDOM_NUMBER_MAX       1000000

typedef struct{
    int DXIndex;
    int DYIndex;
} SMapLocation, *SMapLocationRef;

typedef struct{
    double DXPosition;
    double DYPosition;
    double DZPosition;
    double DXVelocity;
    double DYVelocity;
    double DZVelocity;
    EPlayerColor DOwner;
    int DCannon;
} SCannonballTrajectory, *SCannonballTrajectoryRef;

typedef struct{
  int DXIndex;
  int DYIndex;
  int DXOffset;
  int DYOffset;
  int DSpriteIndex;
  int DStep;
} SSpriteState, *SSpriteStateRef;
int NextPiece = 0;
class CApplicationData{
    
    protected:
        typedef enum{
            gmMainMenu,
            gmSelectMap,
            gmOptionsMenu,
            gmSoundOptions,
            gmTransitionSelectCastle,
            gmSelectCastle,
            gmTransitionRebuild,
            gmRebuild,
            gmTransitionCannonPlacement,
            gmCannonPlacement,
            gmTransitionBattle,
            gmBattle,
            gmGameOver
        } EGameMode, *EGameModeRef;

        typedef enum{
            cttNone,
            cttBlueFloor,
            cttBlueWall,
            cttBlueWallDamaged,
            cttRedFloor,
            cttRedWall,
            cttRedWallDamaged,
            cttYellowFloor,
            cttYellowWall,
            cttYellowWallDamaged,
            cttGroundDamaged,
            cttBlueFloorDamaged,
            cttRedFloorDamaged,
            cttYellowFloorDamaged
        } EConstructionTileType, *EConstructionTileTypeRef;
        
        typedef enum{
            etWallExplosion0 = 0,
            etWallExplosion1,
            etWaterExplosion0,
            etWaterExplosion1,
            etGroundExplosion0,
            etGroundExplosion1,
            etMax
        } EExplosionType, *EExplosionTypeRef;
        
        typedef enum{
            btRubbleBurn0 = 0,
            btRubbleBurn1,
            btHoleBurn0,
            btHoleBurn1,
            btMax
        } EBurnType, *EBurnTypeRef;
        
        typedef enum{
            dNorth = 0,
            dNorthEast,
            dEast,
            dSouthEast,
            dSouth,
            dSouthWest,
            dWest,
            dNorthWest,
            dMax
        } EDirection, *EDirectionRef;
        
        typedef enum{
            sctTick = 0,
            sctTock,
            sctCannon0,
            sctCannon1,
            sctPlace,
            sctTriumph,
            sctExplosion0,
            sctExplosion1,
            sctExplosion2,
            sctExplosion3,
            sctGroundExplosion0,
            sctGroundExplosion1,
            sctWaterExplosion0,
            sctWaterExplosion1,
            sctWaterExplosion2,
            sctWaterExplosion3,
            sctReady,
            sctAim,
            sctFire,
            sctCeasefire,
            sctTransition,
            sctMax
        } ESoundClipType, *ESoundClipTypeRef;
        
        typedef enum{
            stMenu,
            stLoss,
            stWin,
            stPlace,
            stRebuild,
            stMax
        } ESongType, *ESongTypeRef;
        
        typedef enum{
            bbtTopCenter = 0,
            bbtTopRight,
            bbtRight0,
            bbtRight1,
            bbtBottomRight,
            bbtBottomCenter,
            bbtBottomLeft,
            bbtLeft0,
            bbtLeft1,
            bbtTopLeft,
            bbtSingle,
            bbtMax
        } EBorderBrickType, *EBorderBrickTypeRef;
        
        typedef enum{
            bmtTopLeft0 = 0,
            bmtTopLeft1,
            bmtTopLeft2,
            bmtTopCenter,
            bmtTopRight0,
            bmtTopRight1,
            bmtTopRight2,
            bmtRightTop0,
            bmtRightTop1,
            bmtRightTop2,
            bmtRightCenter,
            bmtRightBottom0,
            bmtRightBottom1,
            bmtRightBottom2,
            bmtBottomRight0,
            bmtBottomRight1,
            bmtBottomRight2,
            bmtBottomCenter,
            bmtBottomLeft0,
            bmtBottomLeft1,
            bmtBottomLeft2,
            bmtLeftTop0,
            bmtLeftTop1,
            bmtLeftTop2,
            bmtLeftCenter,
            bmtLeftBottom0,
            bmtLeftBottom1,
            bmtLeftBottom2,
            bmtMax
        } EBorderMotarType, *EBorderMortarTypeRef;
        
        typedef enum{
            wtNone,
            wtMild,
            wtModerate,
            wtErratic
        } EWindType, *EWindTypeRef;
        
        typedef enum{
            bmsReady,
            bmsAim,
            bmsBattle
        } EBattleModeStage, *EBattleModeStageRef;
        
        GtkWidget *DMainWindow;
        GtkWidget *DDrawingArea;
        GdkCursor* DBlankCursor;
        GdkGC     *DDrawingContext;
        GdkPixmap *DDoubleBufferPixmap;
        GdkPixmap *DPreviousWorkingBufferPixmap;
        GdkPixmap *DWorkingBufferPixmap;
        GdkPixmap *D2DTerrainPixmap;
        GdkPixmap *DBannerPixmap;
        GdkPixmap *DMessagePixmap;
        GdkPixbuf *DWorkingPixbuf;
        std::vector< GdkPixmap * > D3DTerrainPixmaps;
        CGraphicTileset D2DTerrainTileset;
        CGraphicTileset D3DTerrainTileset;
        CGraphicTileset D3DFloorTileset;
        CGraphicTileset D3DWallTileset;
        CGraphicTileset D3DCannonTileset;
        CGraphicTileset D3DCastleTileset;
        CGraphicTileset D3DCannonballTileset;
        CGraphicTileset D3DExplosionTileset;
        CGraphicTileset D3DBurnTileset;
        CGraphicTileset D3DCannonPlumeTileset;
        CGraphicTileset D2DCastleCannonTileset;
        CGraphicTileset D2DCastleSelectTileset;
        CGraphicTileset D2DWallFloorTileset;
        CGraphicTileset DDigitTileset;
        CGraphicTileset DBrickTileset;
        CGraphicTileset DMortarTileset;
        CGraphicTileset DTargetTileset;
        CFontTileset DBlackFont;
        CFontTileset DWhiteFont;
        std::vector< CTerrainMap > DTerrainMaps;
        CTerrainMap DTerrainMap;
        CSoundLibraryMixer DSoundMixer;
        CRandomNumberGenerator DRandomNumberGenerator;
        int DScaling;
        int DMapWidth, DMapHeight;
        int DTileWidth, DTileHeight;
        int DCanvasWidth, DCanvasHeight;
        gint DBannerLocation;
        struct timeval DNextExpectedTimeout;
        struct timeval DCurrentStageTimeout;
        struct timeval DLastTickTockTime;
        int DAnimationTimestep;
        EWindType DWindType;
        int DWindSpeed, DWindDirection;
        EGameMode DGameMode;
        EGameMode DNextGameMode;
        EPlayerColor DPlayerColor;
        int DPlayerCount;
        bool DPlayerIsAI[pcMax];
        bool DPlayerIsAlive[pcMax];
        bool DPlayerIsConquered[pcMax];
        int DSurroundedCastleCounts[pcMax];
        std::vector< double > DInitialVelocities;
        std::vector< std::vector< EConstructionTileType > > DConstructionTiles;
        std::vector< std::vector< int > > DHitsTaken;
        std::vector< bool > DSurroundedCastles[pcMax]; 
        std::vector< SMapLocation > DCastleLocations[pcMax]; 
        std::vector< SMapLocation > DCannonLocations[pcMax];
        std::vector< int > DCannonballToneIDs[pcMax];
        std::list< SCannonballTrajectory > DCannonballTrajectories;
        std::list< int > DCannonsReady[pcMax];
        std::list< SSpriteState > DExplosionStates;
        std::list< SSpriteState > DBurnStates;
        std::list< SSpriteState > DPlumeStates;
        std::vector< std::string > DMenuItems;
        std::string DMenuTitle;
        int DSelectedMenuItem;
        int DLastSelectedMenuItem;
        int DCurrentX[pcMax];
        int DCurrentY[pcMax];
        bool DLeftClick[pcMax];
        bool DRightClick[pcMax];
        int DCannonsToPlace[pcMax];
        CWallShape DWallShape[pcMax];
        bool DCanPlaceWallCannon[pcMax];
        bool DCompletedStage[pcMax];
        int DAITargetX[pcMax];
        int DAITargetY[pcMax];
        int DAITargetCastle[pcMax];
        int D2DCastleIndices[pcMax];
        int D2DFloorIndices[pcMax];
        int D2DWallIndices[pcMax+2];
        int D2DCannonIndices[pcMax];
        int D2DSelectColorIndices[pcMax];
        int D2DDamagedGroundIndex;
        int D3DFloorIndices[pcMax];
        int D3DTargetIndices[pcMax];
        int D3DWallIndices[pcMax][16];
        int D3DDamagedWallIndices[pcMax];
        int D3DCastleIndices[pcMax];
        int D3DDamagedGroundIndex;
        int D3DExplosionIndices[etMax];
        int D3DBurnIndices[btMax];
        int D3DCannonPlumeIndices[dMax];
        int DSoundClipIndices[sctMax];
        int DSongIndices[stMax];
        int DBrickIndices[bbtMax];
        int DMortarIndices[bmtMax];
        int DVoiceClipID;
        EBattleModeStage DBattleModeStage;
        int DExplosionSteps;
        int DBurnSteps;
        int DCannonPlumeSteps;
        float DSoundEffectVolume;
        float DMusicVolume;
    
        static bool TileTypeIsFloor(EConstructionTileType type){
            return (cttBlueFloor == type)||(cttRedFloor == type)||(cttYellowFloor == type);
        };
        
        static bool TileTypeIsFloorDamaged(EConstructionTileType type){
            return (cttBlueFloorDamaged == type)||(cttRedFloorDamaged == type) || (cttYellowFloorDamaged == type);
        };
        
        static bool TileTypeIsWall(EConstructionTileType type){
            return (cttBlueWall == type)||(cttRedWall == type)||(cttYellowWall == type);
        };
        
        static bool TileTypeIsWallDamaged(EConstructionTileType type){
            return (cttBlueWallDamaged == type)||(cttRedWallDamaged == type)||(cttYellowWallDamaged == type);
        };
        
        static bool TileTypeIsFloorGroundDamaged(EConstructionTileType type){
            return (cttGroundDamaged == type)||(cttBlueFloorDamaged == type)||(cttRedFloorDamaged == type) || (cttYellowFloorDamaged == type);
        };
        
        // Code from http://www.codecodex.com/wiki/Calculate_an_integer_square_root
        static unsigned int IntegerSquareRoot(unsigned int x){
            register unsigned int Op, Result, One;
            
            Op = x;
            Result = 0;
            
            One = 1 << (sizeof(unsigned int) * 8 - 2);
            while(One > Op){
                One >>= 2;
            }
            while(0 != One){
                if(Op >= Result + One){
                    Op -= Result + One;  
                    Result += One << 1;  // <-- faster than 2 * one  
                }
                Result >>= 1;
                One >>= 2;
            }
            return Result;
        };
        
        static bool TrajectoryCompare(const SCannonballTrajectory &first, const SCannonballTrajectory &second){
            if(first.DZPosition < second.DZPosition){
                return true;
            }
            if(first.DZPosition > second.DZPosition){
                return false;
            }
            if(first.DYPosition < second.DYPosition){
                return true;
            }
            if(first.DYPosition > second.DYPosition){
                return false;
            }
            if(first.DXPosition < second.DXPosition){
                return true;    
            }
            return false;
        };
        
        static bool SpriteCompare(const SSpriteState &first, const SSpriteState &second){
            if(first.DYIndex < second.DYIndex){
                return true;
            }
            if(first.DYIndex > second.DYIndex){
                return false;   
            }
            if(first.DXIndex < second.DXIndex){
                return true;
            }
            return false;
        };
        
        static int SecondsUntilDeadline(struct timeval deadline){
            struct timeval CurrentTime;
            
            gettimeofday(&CurrentTime, NULL);
            
            return ((deadline.tv_sec * 1000 + deadline.tv_usec / 1000) - (CurrentTime.tv_sec * 1000 + CurrentTime.tv_usec / 1000)) / 1000;
        };
        
        static int MiliSecondsUntilDeadline(struct timeval deadline){
            struct timeval CurrentTime;
            
            gettimeofday(&CurrentTime, NULL);
            
            return ((deadline.tv_sec * 1000 + deadline.tv_usec / 1000) - (CurrentTime.tv_sec * 1000 + CurrentTime.tv_usec / 1000));
        };
        
        static int CalculateDirection(int x1, int y1, int x2, int y2){
            int XDistance = x2 - x1;
            int YDistance = y2 - y1;
            bool NegativeX = XDistance < 0;
            bool NegativeY = YDistance > 0; // Top of screen is 0
            double SinSquared;
            
            XDistance *= XDistance;
            YDistance *= YDistance;
            
            if(0 == (XDistance + YDistance)){
                return dNorth;
            }
            SinSquared = (double)YDistance / (XDistance + YDistance);
            
            if(0.1464466094 > SinSquared){
                // East or West
                if(NegativeX){
                    return dWest; // West   
                }
                else{
                    return dEast; // East
                }
            }
            else if(0.85355339059 > SinSquared){
                // NE, SE, SW, NW
                if(NegativeY){
                    if(NegativeX){
                        return dSouthWest; // SW
                    }
                    else{
                        return dSouthEast; // SE
                    }
                }
                else{
                    if(NegativeX){
                        return dNorthWest; // NW
                    }
                    else{
                        return dNorthEast; // NE
                    }
                }
            }
            else{
                // North or South
                if(NegativeY){
                    return dSouth; // South    
                }
                else{
                    return dNorth; // North
                }
            }
            
        };
        
        static int CalculateCannonballSize(double z){
            if(100.1499064 < z){
                return 11;   
            }
            if(97.5217589  < z){
                return 10;
            }
            if(95.41127261 < z){
                return 9;
            }
            if(92.81657303 < z){
                return 8;
            }
            if(89.55578929 < z){
                return 7;
            }
            if(85.34315122 < z){
                return 6;
            }
            if(79.70240953 < z){
                return 5;
            }
            if(71.77635988 < z){
                return 4;
            }
            if(59.85065264 < z){
                return 3;
            }
            /*if(39.92522189 < z){
                return 2;
            }*/
            return 2;
        };
        
        static gboolean TimeoutCallback(gpointer data);
        static gboolean MainWindowDeleteEventCallback(GtkWidget *widget, GdkEvent *event, gpointer data);
        static void MainWindowDestroyCallback(GtkWidget *widget, gpointer data);
        static gboolean MainWindowKeyPressEventCallback(GtkWidget *widget, GdkEventKey *event, gpointer data);
        static gboolean DrawingAreaExposeCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
        static gboolean DrawingAreaButtonPressEventCallback(GtkWidget *widget, GdkEventButton *event, gpointer data);
        static gboolean DrawingAreaMotionNotifyEventCallback(GtkWidget *widget, GdkEventMotion *event, gpointer data);
        
        
        gboolean Timeout();
        gboolean MainWindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
        void MainWindowDestroy(GtkWidget *widget);
        gboolean MainWindowKeyPressEvent(GtkWidget *widget, GdkEventKey *event);
        gboolean DrawingAreaExpose(GtkWidget *widget, GdkEventExpose *event);
        gboolean DrawingAreaButtonPressEvent(GtkWidget *widget, GdkEventButton *event);
        gboolean DrawingAreaMotionNotifyEvent(GtkWidget *widget, GdkEventMotion *event);
        
        void DrawMenu();
        void DrawSelectMap();
        void DrawSoundOptions();
        void Draw2DFrame();
        void Draw3DFrame();
        void DrawBanner(const std::string &message);
        void DrawMessage(const std::string &message);
        void DrawMenuBackground(GdkPixmap *buffer, gint width, gint height);
        void DrawTextFrame(GdkPixmap *buffer, gint width, gint height);
        void DrawBrickFrame(GdkPixmap *buffer, gint xoff, gint yoff, gint width, gint height);
        
        void GameOverMode();
        void TransitionMode();
        void MainMenuMode();
        void SelectMapMode();
        void OptionsMenuMode();
        void SoundOptionsMode();
        void SelectCastleMode();
        void CannonPlacementMode();
        void RebuildMode();
        void BattleMode();
        void ChangeMode(EGameMode nextmode);
        void InitializeBanner(const std::string &message);
        
        void MoveAI(int colorindex);
        void SelectCastleAI();
        void CannonPlacementAI();
        void RebuildAI();
        void BattleAI();
        
        void CheckSurroundedCastles();
        bool CannonCastleInterfere(int xindex, int yindex, int width, int height);
        bool ValidCannonPlacement(int colorindex, int xindex, int yindex);
        bool ValidWallPlacement(int colorindex, int xindex, int yindex);
        void ExpandUnclaimed(int xpos, int ypos);
        bool CleanUpWallEdges();
        bool BoundsCheck(int &xindex, int &yindex);
        void PlayTickTockSound();
        int DetermineConquered();
        void LoadTerrainMap(int index);
        void ResetMap();
        void ResizeCanvases();
        
        
    public:
        CApplicationData();
        ~CApplicationData();
        
        int Init(int argc, char *argv[]);
        
};

CApplicationData::CApplicationData(){
    DGameMode = DNextGameMode = gmMainMenu;
    DPlayerColor = pcBlue;
    
    DMainWindow = NULL;
    DDrawingArea = NULL;
    DBlankCursor = NULL;
    DDrawingContext = NULL;
    DDoubleBufferPixmap = NULL;
    DPreviousWorkingBufferPixmap = NULL;
    DWorkingBufferPixmap = NULL;
    D2DTerrainPixmap = NULL;
    DBannerPixmap = NULL;
    DMessagePixmap = NULL;
    DWorkingPixbuf = NULL;
    
    DSelectedMenuItem = 0;
    DAnimationTimestep = 0;
    DWindDirection = 0;
    DWindSpeed = 1;
    DWindType = wtNone;
    DScaling = 1;
    
    DSoundEffectVolume = 1.0;
    DMusicVolume = 1.0;
    
    for(int Index = 0; Index < pcMax; Index++){
       DCanPlaceWallCannon[Index] = false;
       DCannonsToPlace[Index] = 0;
       DCurrentX[Index] = 0;
       DCurrentY[Index] = 0;
       DLeftClick[Index] = false;
       DRightClick[Index] = false;
    }
}

CApplicationData::~CApplicationData(){
    
}


gboolean CApplicationData::TimeoutCallback(gpointer data){
    CApplicationData *AppData = (CApplicationData *)data;

    if(AppData->Timeout()){
        struct timeval CurrentTime;
        int64_t TimeDelta;
        
        gettimeofday(&CurrentTime, NULL);
        do{
            AppData->DNextExpectedTimeout.tv_usec += TIMEOUT_INTERVAL * 1000;
            if(1000000 <= AppData->DNextExpectedTimeout.tv_usec){
                AppData->DNextExpectedTimeout.tv_usec %= 1000000;
                AppData->DNextExpectedTimeout.tv_sec++;
            }
            TimeDelta = (AppData->DNextExpectedTimeout.tv_sec * 1000 + AppData->DNextExpectedTimeout.tv_usec / 1000) - (CurrentTime.tv_sec * 1000 + CurrentTime.tv_usec / 1000);
        }while(0 >= TimeDelta);
        g_timeout_add(TimeDelta, TimeoutCallback, data);
    }
    return FALSE;
}


// This is a callback function to signify that the delete windows has been called.
// If you return FALSE in the "delete-event" signal handler, GTK will emit the 
// "destroy" signal. Returning TRUE means you don't want the window to be 
// destroyed. This is useful for popping up 'are you sure you want to quit?'
// type dialogs.
gboolean CApplicationData::MainWindowDeleteEventCallback(GtkWidget *widget, GdkEvent *event, gpointer data){
    CApplicationData *AppData = (CApplicationData *)data;

    return AppData->MainWindowDeleteEvent(widget, event);
}

// The destroy window callback
void CApplicationData::MainWindowDestroyCallback(GtkWidget *widget, gpointer data){
    CApplicationData *AppData = (CApplicationData *)data;

    AppData->MainWindowDestroy(widget);
}


gboolean CApplicationData::MainWindowKeyPressEventCallback(GtkWidget *widget, GdkEventKey *event, gpointer data){
    CApplicationData *AppData = (CApplicationData *)data;

    return AppData->MainWindowKeyPressEvent(widget, event);
}

gboolean CApplicationData::DrawingAreaExposeCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data){
    CApplicationData *AppData = (CApplicationData *)data;

    return AppData->DrawingAreaExpose(widget, event);
}

gboolean CApplicationData::DrawingAreaButtonPressEventCallback(GtkWidget *widget, GdkEventButton *event, gpointer data){
    CApplicationData *AppData = (CApplicationData *)data;

    return AppData->DrawingAreaButtonPressEvent(widget, event);
}

gboolean CApplicationData::DrawingAreaMotionNotifyEventCallback(GtkWidget *widget, GdkEventMotion *event, gpointer data){
    CApplicationData *AppData = (CApplicationData *)data;

    return AppData->DrawingAreaMotionNotifyEvent(widget, event);
}

gboolean CApplicationData::Timeout(){
    
    switch(DGameMode){
        case gmMainMenu:                    MainMenuMode();
                                            DrawMenu();
                                            break;
        case gmSelectMap:                   SelectMapMode();
                                            DrawSelectMap();
                                            break;
        case gmOptionsMenu:                 OptionsMenuMode();
                                            DrawMenu();
                                            break;
        case gmSoundOptions:                SoundOptionsMode();
                                            DrawSoundOptions();
                                            break;
        case gmSelectCastle:                SelectCastleAI();
                                            SelectCastleMode();
                                            Draw2DFrame();
                                            break;
        case gmRebuild:                     RebuildAI();
                                            RebuildMode();
                                            Draw2DFrame();
                                            break;
        case gmCannonPlacement:             CannonPlacementAI();
                                            CannonPlacementMode();
                                            Draw2DFrame();
                                            break;                         
        case gmBattle:                      BattleAI();
                                            BattleMode();
                                            Draw3DFrame();
                                            break;
        case gmTransitionBattle:            TransitionMode();
                                            Draw3DFrame();
                                            break;
        case gmTransitionRebuild:                 
        case gmTransitionSelectCastle:
        case gmTransitionCannonPlacement:   TransitionMode();
                                            Draw2DFrame();
                                            break;
        case gmGameOver:                    GameOverMode();
                                            Draw2DFrame();
                                            break;
        
    }
    for(int Index = 0; Index < pcMax; Index++){
        DLeftClick[Index] = false;
        DRightClick[Index] = false;
    }
    if(DNextGameMode != DGameMode){
        gdk_draw_pixmap(DPreviousWorkingBufferPixmap, DDrawingContext, DWorkingBufferPixmap, 0, 0, 0, 0, -1, -1);  
        
        if(gmRebuild == DGameMode){
            // Clean up edges
            while(CleanUpWallEdges());   
        }
        if(gmTransitionRebuild == DNextGameMode){
            CheckSurroundedCastles();
        }
    }
    
    if((NULL != DBannerPixmap)&&(DBannerLocation < DCanvasHeight)){
        if(0 > DBannerLocation){
            gdk_draw_pixmap(DWorkingBufferPixmap, DDrawingContext, DPreviousWorkingBufferPixmap, 0, 0, 0, 0, -1, -1);
        }
        else{
            gdk_draw_pixmap(DWorkingBufferPixmap, DDrawingContext, DPreviousWorkingBufferPixmap, 0, DBannerLocation, 0, DBannerLocation, -1, DCanvasHeight - DBannerLocation);
        }
        gdk_draw_pixmap(DWorkingBufferPixmap, DDrawingContext, DBannerPixmap, 0, 0, 0, DBannerLocation, -1, -1);
    }
    if(gmGameOver == DGameMode){
        gint MessageWidth, MessageHeight;
        
        gdk_pixmap_get_size(DMessagePixmap, &MessageWidth, &MessageHeight); 
        gdk_draw_pixmap(DWorkingBufferPixmap, DDrawingContext, DMessagePixmap, 0, 0, (DCanvasWidth/2) - (MessageWidth/2), (DCanvasHeight/2) - (MessageHeight/2), -1, -1);
    }
    DWorkingPixbuf = gdk_pixbuf_get_from_drawable(DWorkingPixbuf, DWorkingBufferPixmap, NULL, 0, 0, 0, 0, -1, -1);
    if(1 < DScaling){
        GdkPixbuf *ScaledPixbuf = gdk_pixbuf_scale_simple(DWorkingPixbuf, DCanvasWidth * DScaling, DCanvasHeight * DScaling, GDK_INTERP_BILINEAR);
        gdk_draw_pixbuf(DDoubleBufferPixmap, DDrawingContext, ScaledPixbuf, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
        g_object_unref(ScaledPixbuf);
    }
    else{
        gdk_draw_pixbuf(DDoubleBufferPixmap, DDrawingContext, DWorkingPixbuf, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
    }
    gdk_draw_pixmap(DDrawingArea->window, DDrawingContext, DDoubleBufferPixmap, 0, 0, 0, 0, -1, -1);
    DGameMode = DNextGameMode;
    return TRUE;
}

gboolean CApplicationData::MainWindowDeleteEvent(GtkWidget *widget, GdkEvent *event){
    g_print("Delete event occurred\n");    
    return FALSE;    
}

void CApplicationData::MainWindowDestroy(GtkWidget *widget){
    gtk_main_quit();
}

gboolean CApplicationData::MainWindowKeyPressEvent(GtkWidget *widget, GdkEventKey *event){
    if(('1' == event->keyval)&&(1 != DScaling)){
        DScaling = 1;

        ResizeCanvases();
        DWorkingPixbuf = gdk_pixbuf_get_from_drawable(DWorkingPixbuf, DWorkingBufferPixmap, NULL, 0, 0, 0, 0, -1, -1);
        gdk_draw_pixbuf(DDoubleBufferPixmap, DDrawingContext, DWorkingPixbuf, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
    }
    else if(('2' == event->keyval)&&(2 != DScaling)){
        DScaling = 2;

        ResizeCanvases();
        DWorkingPixbuf = gdk_pixbuf_get_from_drawable(DWorkingPixbuf, DWorkingBufferPixmap, NULL, 0, 0, 0, 0, -1, -1);
        GdkPixbuf *ScaledPixbuf = gdk_pixbuf_scale_simple(DWorkingPixbuf, DCanvasWidth * DScaling, DCanvasHeight * DScaling, GDK_INTERP_BILINEAR);
        gdk_draw_pixbuf(DDoubleBufferPixmap, DDrawingContext, ScaledPixbuf, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
        g_object_unref(ScaledPixbuf);
    } 
    return TRUE;
}

gboolean CApplicationData::DrawingAreaExpose(GtkWidget *widget, GdkEventExpose *event){
    gdk_draw_pixmap(widget->window, widget->style->fg_gc[gtk_widget_get_state (widget)], DDoubleBufferPixmap, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
    return FALSE;
}

gboolean CApplicationData::DrawingAreaButtonPressEvent(GtkWidget *widget, GdkEventButton *event){
    if(1 == event->button){
        if(GDK_CONTROL_MASK & event->state){
            DRightClick[pcNone] = DRightClick[DPlayerColor] = true;
        }
        else{
            DLeftClick[pcNone] = DLeftClick[DPlayerColor] = true;   
        }
    }
    if(3 == event->button){
        DRightClick[pcNone] = DRightClick[DPlayerColor] = true;
    }
    return TRUE;
}

gboolean CApplicationData::DrawingAreaMotionNotifyEvent(GtkWidget *widget, GdkEventMotion *event){
    int EventX, EventY;
    EventX = event->x;
    EventY = event->y;
    if(EventX > DCanvasWidth * DScaling){
        EventX = DCanvasWidth * DScaling - 1;
    }
    if(0 > EventX){
        EventX = 0;   
    }
    if(EventY > DCanvasHeight * DScaling){
        EventY = DCanvasHeight * DScaling - 1;
    }
    if(0 > EventY){
        EventY = 0;   
    }

    DCurrentX[pcNone] = DCurrentX[DPlayerColor] = EventX/DScaling;
    DCurrentY[pcNone] = DCurrentY[DPlayerColor] = EventY/DScaling;    
    return TRUE;
}

void CApplicationData::DrawMenu(){
    gint TotalTextHeight, TextX, TextY, TextWidth, TextHeight;
    gint RequiredWidth, RequiredHeight;
    
    DrawMenuBackground(DWorkingBufferPixmap, DCanvasWidth, DCanvasHeight);
    
    DBlackFont.MeasureText(DMenuTitle, TextWidth, TextHeight);
    
    RequiredWidth = TextWidth + (DBrickTileset.TileWidth() * 3) / 2; 
    RequiredWidth = ((RequiredWidth + DBrickTileset.TileWidth() - 1)/DBrickTileset.TileWidth()) * DBrickTileset.TileWidth();
    RequiredHeight = TextHeight + 1;
    RequiredHeight += DBrickTileset.TileHeight() * 2;
    RequiredHeight = ((RequiredHeight + DBrickTileset.TileHeight() - 1) / DBrickTileset.TileHeight()) * DBrickTileset.TileHeight();
    
    for(gint TopPosition = 0, XOffset = (DCanvasWidth/2) - (RequiredWidth/2) - 1; TopPosition + D2DWallFloorTileset.TileHeight() <= RequiredHeight; TopPosition += D2DWallFloorTileset.TileHeight()){
        D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XOffset, TopPosition, D2DFloorIndices[pcNone]);
        D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XOffset + RequiredWidth + 2 - D2DWallFloorTileset.TileWidth(), TopPosition, D2DFloorIndices[pcNone]);
    }
    for(gint LeftPosition = (DCanvasWidth/2) - (RequiredWidth/2) - 1, YOffset = RequiredHeight - D2DWallFloorTileset.TileHeight() + 1; LeftPosition + D2DWallFloorTileset.TileWidth() <= (DCanvasWidth/2) + (RequiredWidth/2) + 1; LeftPosition += D2DWallFloorTileset.TileWidth()){
        D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, LeftPosition, YOffset, D2DFloorIndices[pcNone]);
    }
    D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (DCanvasWidth/2) + (RequiredWidth/2) + 1 - D2DWallFloorTileset.TileWidth(), RequiredHeight - D2DWallFloorTileset.TileHeight() + 1, D2DFloorIndices[pcNone]);
    
    DrawBrickFrame(DWorkingBufferPixmap, (DCanvasWidth/2) - (RequiredWidth/2), 0, RequiredWidth, RequiredHeight); 
    TextX = (DCanvasWidth/2) - (TextWidth/2);
    TextY = (RequiredHeight/2) - (TextHeight/2);
    DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, DMenuTitle);
    DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, DMenuTitle);
    
    TotalTextHeight = DMenuItems.size() * DBlackFont.TileHeight() + (DMenuItems.size() - 1) * DBlackFont.TileHeight() / 2;
    TextY = RequiredHeight + ((DCanvasHeight - RequiredHeight)/2) - (TotalTextHeight/2);
    for(int Index = 0; Index < DMenuItems.size(); Index++){
        gint TextHeight, TextWidth;
        
        DBlackFont.MeasureText(DMenuItems[Index], TextWidth, TextHeight);
        TextX = (DCanvasWidth/2) - (TextWidth/2);
        if(DSelectedMenuItem == Index){
            DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, DMenuItems[Index]);
            DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, DMenuItems[Index]);
        }
        else{
            DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, DMenuItems[Index]);
            DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, DMenuItems[Index]);            
        }
        TextY += TextHeight + TextHeight/2;
    }
    
}

void CApplicationData::DrawSoundOptions(){
    gint TextX, TextY, TextWidth, TextHeight, MaxWidth, MaxHeight;
    gint RequiredWidth, RequiredHeight;
    std::string TempString;
    std::vector< std::string > TempItems = DMenuItems;
    std::stringstream TempStringStream;

    TempString = DMenuTitle;
    DMenuTitle = "SOUND OPTIONS";
    DMenuItems.clear();
    DrawMenu();
    DMenuTitle = TempString;
    DMenuItems = TempItems;

    DBlackFont.MeasureText(DMenuTitle, TextWidth, TextHeight);
    DBlackFont.MeasureText("100%", MaxWidth, MaxHeight);
    
    RequiredWidth = TextWidth + DBrickTileset.TileWidth()/2; 
    RequiredWidth = ((RequiredWidth + DBrickTileset.TileWidth() - 1)/DBrickTileset.TileWidth()) * DBrickTileset.TileWidth();
    RequiredHeight = TextHeight + 1;
    RequiredHeight += DBrickTileset.TileHeight() * 2;
    RequiredHeight = ((RequiredHeight + DBrickTileset.TileHeight() - 1) / DBrickTileset.TileHeight()) * DBrickTileset.TileHeight();

    TextX = DCanvasWidth / 2;
    TextY = RequiredHeight + TextHeight / 2;

    TempString = "SOUND FX ";   
    DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
    TextX -= TextWidth;
    if(2 > DSelectedMenuItem){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    
    TempString = "  - ";   
    DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
    TextX = DCanvasWidth / 2;
    if(0 == DSelectedMenuItem){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    TextX += TextWidth + MaxWidth;
    TempStringStream<<(int)round(100.0 * DSoundEffectVolume)<<"%";
    TempString = TempStringStream.str();
    DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
    TextX -= TextWidth;
    if(2 > DSelectedMenuItem){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    TextX += TextWidth;
    TempString = "  +";   
    if(1 == DSelectedMenuItem){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    
    TextY += TextHeight;
    
    TempString = "MUSIC ";
    DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
    TextX = DCanvasWidth / 2;
    TextX -= TextWidth;
    if((2 <= DSelectedMenuItem)&&(4 > DSelectedMenuItem)){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    TempString = "  - ";   
    DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
    TextX = DCanvasWidth / 2;
    if(2 == DSelectedMenuItem){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    TextX += TextWidth + MaxWidth;
    TempStringStream.str("");
    TempStringStream<<(int)round(100.0 * DMusicVolume)<<"%";
    TempString = TempStringStream.str();
    DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
    TextX -= TextWidth;
    if((2 <= DSelectedMenuItem)&&(4 > DSelectedMenuItem)){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    TextX += TextWidth;
    TempString = "  +";   
    if(3 == DSelectedMenuItem){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }

    TextX = DCanvasWidth - (DBrickTileset.TileWidth() * 3) / 2;
    TextY = DCanvasHeight - (DBrickTileset.TileHeight() * 3) / 2;
    TempString = "BACK";
    DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
    TextX -= TextWidth;
    TextY -= TextHeight;
    if(4 <= DSelectedMenuItem){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
}


void CApplicationData::DrawSelectMap(){
    gint TextX, TextY, TextWidth, TextHeight;
    gint RequiredWidth, RequiredHeight;
    gint MiniX, MiniY;
    std::string TempString;
    std::vector< std::string > TempItems = DMenuItems;

    TempString = DMenuTitle;
    DMenuTitle = "SELECT MAP";
    DMenuItems.clear();
    DrawMenu();
    DMenuTitle = TempString;
    DMenuItems = TempItems;

    DBlackFont.MeasureText(DMenuTitle, TextWidth, TextHeight);
    
    RequiredWidth = TextWidth + DBrickTileset.TileWidth()/2; 
    RequiredWidth = ((RequiredWidth + DBrickTileset.TileWidth() - 1)/DBrickTileset.TileWidth()) * DBrickTileset.TileWidth();
    RequiredHeight = TextHeight + 1;
    RequiredHeight += DBrickTileset.TileHeight() * 2;
    RequiredHeight = ((RequiredHeight + DBrickTileset.TileHeight() - 1) / DBrickTileset.TileHeight()) * DBrickTileset.TileHeight();

    TextX = (DBrickTileset.TileWidth() * 3) / 2;
    TextY = RequiredHeight + TextHeight / 2;
    MiniY = TextY;
    for(int Index = 0; Index < DTerrainMaps.size(); Index++){
        TempString = DTerrainMaps[Index].MapName();
        
        DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
        if(DSelectedMenuItem == Index){
            std::stringstream TempStringStream;
            gint InfoWidth, InfoHeight;
            
            DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
            DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
            MiniX = DCanvasWidth - (DBrickTileset.TileWidth() * 3) / 2;
            MiniX -= DTerrainMaps[Index].Width() * 2;
            DTerrainMaps[Index].DrawPreviewMap(DWorkingBufferPixmap, DDrawingContext, MiniX, MiniY);
            
            MiniX += DTerrainMaps[Index].Width();
            MiniY += DTerrainMaps[Index].Height() * 2 + TextHeight/2;
            
            TempStringStream<<DTerrainMaps[Index].PlayerCount()<<" PLAYERS";
            DBlackFont.MeasureText(TempStringStream.str(), InfoWidth, InfoHeight);
            DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, MiniX-(InfoWidth/2) + 1, MiniY + 1, TempStringStream.str());
            DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, MiniX-(InfoWidth/2), MiniY, TempStringStream.str());
            MiniY += InfoHeight + 2;
            TempStringStream.str("");
            TempStringStream<<DTerrainMaps[Index].Width()<<" x "<<DTerrainMaps[Index].Height();
            DBlackFont.MeasureText(TempStringStream.str(), InfoWidth, InfoHeight);
            DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, MiniX-(InfoWidth/2) + 1, MiniY + 1, TempStringStream.str());
            DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, MiniX-(InfoWidth/2), MiniY, TempStringStream.str());
            
        }
        else{
            DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
            DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);            
        }
        TextY += TextHeight + TextHeight/2;
    }
    TextX = DCanvasWidth - (DBrickTileset.TileWidth() * 3) / 2;
    TextY = DCanvasHeight - (DBrickTileset.TileHeight() * 3) / 2;
    TempString = "BACK";
    DBlackFont.MeasureText(TempString, TextWidth, TextHeight);
    TextX -= TextWidth;
    TextY -= TextHeight;
    if(DSelectedMenuItem >= DTerrainMaps.size()){
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
    else{
        DWhiteFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX + 1, TextY + 1, TempString);
        DBlackFont.DrawText(DWorkingBufferPixmap, DDrawingContext, TextX, TextY, TempString);
    }
}

void CApplicationData::Draw2DFrame(){
    gdk_draw_pixmap(DWorkingBufferPixmap, DDrawingContext, D2DTerrainPixmap, 0, 0, 0, 0, -1, -1);

    for(int YIndex = 0, YPos = 0; YIndex < DConstructionTiles.size(); YIndex++, YPos += DTileHeight){
        for(int XIndex = 0, XPos = 0; XIndex < DConstructionTiles[YIndex].size(); XIndex++, XPos+= DTileWidth){
            bool IsEven = ((YIndex + XIndex) & 0x1) ? false : true;
            int WallType = 0xF;
            if(TileTypeIsWall(DConstructionTiles[YIndex][XIndex])){
                if(YIndex && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex]))){
                    WallType &= 0xE;
                }
                if((XIndex + 1 < DConstructionTiles[YIndex].size()) && (TileTypeIsWall(DConstructionTiles[YIndex][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex][XIndex+1]))){
                    WallType &= 0xD;
                }
                if((YIndex + 1 < DConstructionTiles.size()) && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex]))){
                    WallType &= 0xB;
                }
                if(XIndex && (TileTypeIsWall(DConstructionTiles[YIndex][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex][XIndex-1]))){
                    WallType &= 0x7;
                }
            }
            
            switch(DConstructionTiles[YIndex][XIndex]){
                case cttBlueWall:           D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D2DWallIndices[pcBlue] + WallType);
                                            break;
                case cttRedWall:            D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D2DWallIndices[pcRed] + WallType);
                                            break;
                case cttYellowWall:         D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D2DWallIndices[pcYellow] + WallType);
                                            break;
                case cttBlueFloor:          
                case cttBlueFloorDamaged:   D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, IsEven ? D2DFloorIndices[pcNone] : D2DFloorIndices[pcBlue]);
                                            break;
                case cttRedFloor:
                case cttRedFloorDamaged:    D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, IsEven ? D2DFloorIndices[pcNone] : D2DFloorIndices[pcRed]);
                                            break;
                case cttYellowFloor:    
                case cttYellowFloorDamaged: D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, IsEven ? D2DFloorIndices[pcNone] : D2DFloorIndices[pcYellow]);
                                            break;
                default:                break;
            }
            if(TileTypeIsFloorGroundDamaged(DConstructionTiles[YIndex][XIndex])){
                D2DTerrainTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D2DDamagedGroundIndex);
            }
        }
    }
    for(int ColorIndex = pcBlue; ColorIndex < pcMax; ColorIndex++){
        std::vector< bool >::iterator SurroundedIterator;
        std::vector< SMapLocation >::iterator LocationIterator;
        
        SurroundedIterator = DSurroundedCastles[ColorIndex].begin();
        LocationIterator = DCastleLocations[ColorIndex].begin();
        while(DSurroundedCastles[ColorIndex].end() != SurroundedIterator){
            int XPos, YPos;
            XPos = LocationIterator->DXIndex;
            YPos = LocationIterator->DYIndex;
            if(*SurroundedIterator){
                int SelectColorIndex = 0;
                
                SelectColorIndex = D2DSelectColorIndices[ColorIndex];  
                D2DCastleCannonTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos * DTileWidth, YPos * DTileHeight, D2DCastleIndices[ColorIndex]);
                if((gmSelectCastle == DGameMode)&&(!DCompletedStage[ColorIndex])&&(DGameMode == DNextGameMode)){
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos * DTileWidth, (YPos - 1) * DTileHeight, SelectColorIndex);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos + 1) * DTileWidth, (YPos - 1) * DTileHeight, SelectColorIndex);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos + 2) * DTileWidth, (YPos - 1) * DTileHeight, SelectColorIndex + 1);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos + 2) * DTileWidth, YPos * DTileHeight, SelectColorIndex + 2);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos + 2) * DTileWidth, (YPos + 1) * DTileHeight, SelectColorIndex + 2);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos + 2) * DTileWidth, (YPos + 2) * DTileHeight, SelectColorIndex + 3);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos + 1) * DTileWidth, (YPos + 2) * DTileHeight, SelectColorIndex + 4);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos * DTileWidth, (YPos + 2) * DTileHeight, SelectColorIndex + 4);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos - 1) * DTileWidth, (YPos + 2) * DTileHeight, SelectColorIndex + 5);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos - 1) * DTileWidth, (YPos + 1) * DTileHeight, SelectColorIndex + 6);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos - 1) * DTileWidth, YPos * DTileHeight, SelectColorIndex + 6);
                    D2DCastleSelectTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XPos - 1) * DTileWidth, (YPos - 1) * DTileHeight, SelectColorIndex + 7);
                }
            }
            else{
                D2DCastleCannonTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos * DTileWidth, YPos * DTileHeight, D2DCastleIndices[pcNone]);
            }
            SurroundedIterator++;
            LocationIterator++;
        }
    }
    for(int Index = pcBlue; Index < pcMax; Index++){
        std::vector< SMapLocation >::iterator Iterator = DCannonLocations[Index].begin();
        
        while(DCannonLocations[Index].end() != Iterator){
            D2DCastleCannonTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, Iterator->DXIndex * DTileWidth, Iterator->DYIndex * DTileHeight, D2DCannonIndices[pcNone]);
            Iterator++;
        }
    }
    if(gmCannonPlacement == DGameMode){
        for(int Index = pcBlue; Index < pcBlue + DPlayerCount; Index++){
            if((DCannonsToPlace[Index])&&(DGameMode == DNextGameMode)){
                int XTile, YTile;
                XTile = DCurrentX[Index] / DTileWidth;
                YTile = DCurrentY[Index] / DTileHeight;
                D2DCastleCannonTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XTile * DTileWidth, YTile * DTileHeight, D2DCannonIndices[Index] + DCannonsToPlace[Index] - 1);
            }
        }
    }
    else if(gmRebuild == DGameMode){
        for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
            if((!DCompletedStage[ColorIndex])&&(DGameMode == DNextGameMode)){
                int XTile, YTile;
                int WallIndexEven, WallIndexOdd;
                
                XTile = DCurrentX[ColorIndex] / DTileWidth;
                YTile = DCurrentY[ColorIndex] / DTileHeight;
                if(XTile + DWallShape[ColorIndex].Width() >= DMapWidth){
                    XTile = DMapWidth - DWallShape[ColorIndex].Width();
                }
                if(YTile + DWallShape[ColorIndex].Height() >= DMapHeight){
                    YTile = DMapHeight - DWallShape[ColorIndex].Height();
                }
                if(DCanPlaceWallCannon[ColorIndex]){
                    struct timeval CurrentTime;
                    
                    gettimeofday(&CurrentTime, NULL);
                    if(CurrentTime.tv_usec < 500000){
                        WallIndexEven = D2DWallIndices[pcMax]; 
                        WallIndexOdd = D2DWallIndices[pcMax + 1];
                    }
                    else{
                        WallIndexEven = D2DWallIndices[pcMax + 1];
                        WallIndexOdd = D2DWallIndices[pcMax];
                    }
                }
                else{
                    WallIndexEven = WallIndexOdd = D2DWallIndices[pcNone];       
                }
                for(int WallYPos = 0; WallYPos < DWallShape[ColorIndex].Height(); WallYPos++){
                    for(int WallXPos = 0; WallXPos < DWallShape[ColorIndex].Width(); WallXPos++){
                        if(DWallShape[ColorIndex].IsBlock(WallXPos, WallYPos)){
                            int WallType = 0xF;
                            bool IsEven = (WallYPos + WallXPos) & 0x1 ? false : true;
                            
                            if(WallYPos && DWallShape[ColorIndex].IsBlock(WallXPos, WallYPos-1)){
                                WallType &= 0xE;
                            }
                            if((WallXPos + 1 < DWallShape[ColorIndex].Width()) && DWallShape[ColorIndex].IsBlock(WallXPos+1, WallYPos)){
                                WallType &= 0xD;
                            }
                            if((WallYPos + 1 < DWallShape[ColorIndex].Height()) && DWallShape[ColorIndex].IsBlock(WallXPos, WallYPos+1)){
                                WallType &= 0xB;
                            }
                            if(WallXPos && DWallShape[ColorIndex].IsBlock(WallXPos-1, WallYPos)){
                                WallType &= 0x7;
                            }
                            D2DWallFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (XTile + WallXPos) * DTileWidth, (YTile + WallYPos) * DTileHeight, (IsEven ? WallIndexEven : WallIndexOdd) + WallType);
                        }
                    }
                }
            }
        }
    }
    if((gmCannonPlacement == DGameMode)||(gmRebuild == DGameMode)){
        int SecondsLeft = SecondsUntilDeadline(DCurrentStageTimeout);
        
        if(0 > SecondsLeft){
            SecondsLeft = 0;    
        }
        DDigitTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, 0, 0, (SecondsLeft/10) % 10);
        DDigitTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, DDigitTileset.TileWidth(), 0, SecondsLeft % 10);    
    }
} 

void CApplicationData::Draw3DFrame(){
    std::vector< SMapLocation >::iterator CannonPositions[pcMax];
    std::vector< SMapLocation >::iterator CastlePositions[pcMax];
    std::vector< bool >::iterator CastleSurroundedPositions[pcMax];      
    std::list< SSpriteState >::iterator ExplosionPosition;
    std::list< SSpriteState >::iterator BurnPosition;
    std::list< SSpriteState >::iterator PlumePosition;
    int CurrentAnimationTimestep = (DAnimationTimestep/4) % ANIMATION_TIMESTEPS;

    for(int Index = 0; Index < pcMax; Index++){
        CannonPositions[Index] = DCannonLocations[Index].begin();
    }
    for(int Index = 0; Index < pcMax; Index++){
        CastlePositions[Index] = DCastleLocations[Index].begin();
        CastleSurroundedPositions[Index] = DSurroundedCastles[Index].begin();
    }    
    ExplosionPosition = DExplosionStates.begin();
    BurnPosition = DBurnStates.begin();
    PlumePosition = DPlumeStates.begin();
    
    gdk_draw_pixmap(DWorkingBufferPixmap, DDrawingContext, D3DTerrainPixmaps[CurrentAnimationTimestep], 0, 0, 0, 0, -1, -1);
    
    for(int YIndex = 0, YPos = 0; YIndex < DConstructionTiles.size(); YIndex++, YPos += DTileHeight){
        for(int XIndex = 0, XPos = 0; XIndex < DConstructionTiles[YIndex].size(); XIndex++, XPos += DTileWidth){
            switch(DConstructionTiles[YIndex][XIndex]){
                case cttBlueWall:
                case cttBlueWallDamaged:
                case cttRedWall:
                case cttRedWallDamaged:
                case cttYellowWall:
                case cttYellowWallDamaged:  if(YIndex + 1 < DConstructionTiles.size()){
                                                if(!TileTypeIsFloor(DConstructionTiles[YIndex+1][XIndex])){
                                                    break;
                                                }
                                            }
                                            else{
                                                break;   
                                            }
                case cttBlueFloor:
                case cttBlueFloorDamaged:
                case cttRedFloor:       
                case cttRedFloorDamaged:
                case cttYellowFloor:
                case cttYellowFloorDamaged: D3DFloorTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DFloorIndices[DTerrainMap.TileType(XIndex, YIndex)]);
                                        break;
                default:                break;
            }
        }
    }

    for(int YIndex = 0, YPos = -DTileHeight; YIndex < DConstructionTiles.size(); YIndex++, YPos += DTileHeight){
        for(int XIndex = 0, XPos = 0; XIndex < DConstructionTiles[YIndex].size(); XIndex++, XPos += DTileWidth){
            int WallType = 0xF;
            int WallOffset = 0x0;
            if(TileTypeIsWall(DConstructionTiles[YIndex][XIndex])){
                if(YIndex && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex]))){
                    WallType &= 0xE;
                }
                if((XIndex + 1 < DConstructionTiles[YIndex].size()) && (TileTypeIsWall(DConstructionTiles[YIndex][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex][XIndex+1]))){
                    WallType &= 0xD;
                }
                if((YIndex + 1 < DConstructionTiles.size()) && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex]))){
                    WallType &= 0xB;
                }
                if(XIndex && (TileTypeIsWall(DConstructionTiles[YIndex][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex][XIndex-1]))){
                    WallType &= 0x7;
                }
                switch(WallType){
                    case 0: WallOffset = 0xF;
                            if(YIndex){
                                if((XIndex + 1 < DConstructionTiles[YIndex].size()) && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex+1]))){
                                    WallOffset &= 0xE;   
                                }
                                if(XIndex && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex-1]))){
                                    WallOffset &= 0x7;
                                }
                            }
                            if(YIndex + 1 < DConstructionTiles.size()){
                                if((XIndex + 1 < DConstructionTiles[YIndex].size()) && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex+1]))){
                                    WallOffset &= 0xD;   
                                }
                                if(XIndex && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex-1]))){
                                    WallOffset &= 0xB;
                                } 
                            }
                            break;
                    case 1: WallOffset = 0x3;
                            if(YIndex + 1 < DConstructionTiles.size()){
                                if((XIndex + 1 < DConstructionTiles[YIndex].size()) && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex+1]))){
                                    WallOffset &= 0x2;   
                                }
                                if(XIndex && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex-1]))){
                                    WallOffset &= 0x1;
                                } 
                            }
                            break;
                    case 2: WallOffset = 0x3;
                            if(XIndex){
                                if((YIndex + 1 < DConstructionTiles.size()) && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex-1]))){
                                    WallOffset &= 0x2;   
                                }
                                if(YIndex && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex-1]))){
                                    WallOffset &= 0x1;
                                } 
                            }
                            break;
                    case 3: WallOffset = 1;
                            if(XIndex && (YIndex + 1 < DConstructionTiles.size()) && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex-1]))){
                                WallOffset = 0;
                            }
                            break;
                    case 4: WallOffset = 0x3;
                            if(YIndex){
                                if((XIndex + 1 < DConstructionTiles[YIndex].size()) && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex+1]))){
                                    WallOffset &= 0x2;   
                                }
                                if(XIndex && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex-1]))){
                                    WallOffset &= 0x1;
                                }
                            }
                            break;
                    case 6: WallOffset = 1;
                            if(XIndex && YIndex && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex-1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex-1]))){
                                WallOffset = 0;
                            }
                            break;
                    case 8: WallOffset = 0x3;
                            if(XIndex + 1 < DConstructionTiles[YIndex].size()){
                                if(YIndex && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex+1]))){
                                    WallOffset &= 0x2;
                                }
                                if((YIndex + 1 < DConstructionTiles.size()) && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex+1]))){
                                    WallOffset &= 0x1;   
                                }
                            }
                            break;
                    case 9: WallOffset = 1;
                            if((XIndex + 1 < DConstructionTiles[YIndex].size()) && (YIndex + 1 < DConstructionTiles.size()) && (TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex+1][XIndex+1]))){
                                WallOffset = 0;
                            }
                            break;
                    case 12:WallOffset = 1;
                            if((XIndex + 1 < DConstructionTiles[YIndex].size()) && YIndex && (TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex+1])||TileTypeIsWallDamaged(DConstructionTiles[YIndex-1][XIndex+1]))){
                                WallOffset = 0;
                            }
                            break;
                    default:WallOffset = 0;
                            break;
                }
            
                switch(DConstructionTiles[YIndex][XIndex]){
                    case cttBlueWall:       D3DWallTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DWallIndices[pcBlue][WallType] + WallOffset);
                                            break;
                    case cttRedWall:        D3DWallTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DWallIndices[pcRed][WallType] + WallOffset);
                                            break;
                    case cttYellowWall:     D3DWallTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DWallIndices[pcYellow][WallType] + WallOffset);
                                            break;
                    default:                break;
                }
            }
            else if(TileTypeIsWallDamaged(DConstructionTiles[YIndex][XIndex])){
                if(YIndex && TileTypeIsWall(DConstructionTiles[YIndex-1][XIndex])){
                    WallType &= 0xE;
                }
                if((XIndex + 1 < DConstructionTiles[YIndex].size()) && TileTypeIsWall(DConstructionTiles[YIndex][XIndex+1])){
                    WallType &= 0xD;
                }
                if((YIndex + 1 < DConstructionTiles.size()) && TileTypeIsWall(DConstructionTiles[YIndex+1][XIndex])){
                    WallType &= 0xB;
                }
                if(XIndex && TileTypeIsWall(DConstructionTiles[YIndex][XIndex-1])){
                    WallType &= 0x7;
                }
                switch(DConstructionTiles[YIndex][XIndex]){
                    case cttBlueWallDamaged:    D3DWallTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DDamagedWallIndices[pcBlue]+ WallType);
                                                break;
                    case cttRedWallDamaged:     D3DWallTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DDamagedWallIndices[pcRed] + WallType);
                                                break;
                    case cttYellowWallDamaged:  D3DWallTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DDamagedWallIndices[pcYellow] + WallType);
                                                break;
                    default:                    break;
                }
            }
            else if(TileTypeIsFloorGroundDamaged(DConstructionTiles[YIndex][XIndex])){
                D3DTerrainTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos + DTileHeight, D3DDamagedGroundIndex);    
            }
            else{
                for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
                    if(DCannonLocations[ColorIndex].end() != CannonPositions[ColorIndex]){
                        if((CannonPositions[ColorIndex]->DXIndex == XIndex)&&(CannonPositions[ColorIndex]->DYIndex == YIndex)){
                            if(TileTypeIsFloor(DConstructionTiles[YIndex][XIndex])){
                                int CenterX, CenterY;
                                int CannonDirection;
                                
                                CenterX = (CannonPositions[ColorIndex]->DXIndex + 1) * DTileWidth;
                                CenterY = (CannonPositions[ColorIndex]->DYIndex + 1) * DTileHeight;
                                CannonDirection = CalculateDirection(CenterX, CenterY, DCurrentX[ColorIndex], DCurrentY[ColorIndex]);
                                
                                D3DCannonTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos + DTileHeight, CannonDirection);
                            }
                            else{
                                D3DCannonTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos + DTileHeight, 4);
                            }
                            CannonPositions[ColorIndex]++;
                        }
                    }
                }
                for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
                    if(DCastleLocations[ColorIndex].end() != CastlePositions[ColorIndex]){
                        if((XIndex == CastlePositions[ColorIndex]->DXIndex)&&(YIndex == CastlePositions[ColorIndex]->DYIndex)){
                            if(*CastleSurroundedPositions[ColorIndex]){
                        
                                if(pcNone == ColorIndex){
                                    D3DCastleTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DCastleIndices[pcNone]);
                                }
                                else{
                                    D3DCastleTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DCastleIndices[ColorIndex] + (DWindDirection * ANIMATION_TIMESTEPS) + CurrentAnimationTimestep);
                                }
                            }
                            else{
                                D3DCastleTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, D3DCastleIndices[pcNone]);
                            }
                            CastleSurroundedPositions[ColorIndex]++;
                            CastlePositions[ColorIndex]++;
                        }
                    }
                }
            }
            while(BurnPosition != DBurnStates.end()){
                if((BurnPosition->DXIndex == XIndex)&&(BurnPosition->DYIndex == YIndex)){
                    D3DBurnTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos + DTileHeight, BurnPosition->DSpriteIndex + BurnPosition->DStep);   
                    BurnPosition++;    
                }
                else{
                    break;    
                }
            }
            while(ExplosionPosition != DExplosionStates.end()){
                if((ExplosionPosition->DXIndex == XIndex)&&(ExplosionPosition->DYIndex == YIndex)){
                    D3DExplosionTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos - DTileWidth/2 + ExplosionPosition->DXOffset, YPos - DTileHeight + ExplosionPosition->DYOffset, ExplosionPosition->DSpriteIndex + ExplosionPosition->DStep);   
                    ExplosionPosition++;    
                }
                else{
                    break;
                }
            }

            while(PlumePosition != DPlumeStates.end()){
                if((PlumePosition->DXIndex == XIndex)&&(PlumePosition->DYIndex == YIndex)){
                    D3DCannonPlumeTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos + DTileWidth - D3DCannonPlumeTileset.TileWidth()/2, YPos, PlumePosition->DSpriteIndex + PlumePosition->DStep);   
                    PlumePosition++;    
                }
                else{
                    break;
                }
                   
            }
        }
    }
    while((BurnPosition != DBurnStates.end())||(ExplosionPosition != DExplosionStates.end())||(PlumePosition != DPlumeStates.end())){
        if(BurnPosition != DBurnStates.end()){
            D3DBurnTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, BurnPosition->DXIndex * DTileWidth, BurnPosition->DYIndex * DTileHeight, BurnPosition->DSpriteIndex + BurnPosition->DStep);   
            BurnPosition++;
        }
        else if(ExplosionPosition != DExplosionStates.end()){
            D3DExplosionTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, ExplosionPosition->DXIndex * DTileWidth - DTileWidth/2 + ExplosionPosition->DXOffset, (ExplosionPosition->DYIndex - 2) * DTileHeight + ExplosionPosition->DYOffset, ExplosionPosition->DSpriteIndex + ExplosionPosition->DStep);   
            ExplosionPosition++;
        }
        else if(PlumePosition != DPlumeStates.end()){
            D3DCannonPlumeTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, (PlumePosition->DXIndex + 1) * DTileWidth - D3DCannonPlumeTileset.TileWidth()/2, (PlumePosition->DYIndex - 1) * DTileHeight, PlumePosition->DSpriteIndex + PlumePosition->DStep);   
            PlumePosition++; 
        }
    }
    
    for(std::list< SCannonballTrajectory >::iterator Cannonball = DCannonballTrajectories.begin(); Cannonball != DCannonballTrajectories.end(); Cannonball++){
        int XPos = Cannonball->DXPosition;
        int YPos = Cannonball->DYPosition;
        
        XPos -= D3DCannonballTileset.TileWidth() / 2;
        YPos -= D3DCannonballTileset.TileHeight() / 2;
        YPos -= Cannonball->DZPosition;
        
        D3DCannonballTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, XPos, YPos, CalculateCannonballSize(Cannonball->DZPosition));
    }
    
    if((gmBattle == DGameMode)&&(gmBattle == DNextGameMode)){
        for(int Index = pcBlue; Index < pcBlue + DPlayerCount; Index++){
            DTargetTileset.DrawTile(DWorkingBufferPixmap, DDrawingContext, DCurrentX[Index] - DTargetTileset.TileWidth() / 2, DCurrentY[Index] - DTargetTileset.TileHeight() / 2, D3DTargetIndices[Index]);
        }
    }
    
    DAnimationTimestep += DWindSpeed;
}


void CApplicationData::DrawBanner(const std::string &message){
    gint BannerWidth = 0; 
    gint BannerHeight = 0; 
    gint RequiredWidth, RequiredHeight; 
    gint TextRequiredWidth, TextRequiredHeight;
    gint TextX, TextY;
    //gint BrickType = 0;
    
    DBlackFont.MeasureText(message, TextRequiredWidth, TextRequiredHeight); 
    RequiredHeight = TextRequiredHeight + 1;
    RequiredWidth = DCanvasWidth;
    RequiredHeight += DBrickTileset.TileHeight() * 2;
    RequiredHeight = ((RequiredHeight + DBrickTileset.TileHeight() - 1) / DBrickTileset.TileHeight()) * DBrickTileset.TileHeight();
    if(NULL != DBannerPixmap){
        gdk_pixmap_get_size(DBannerPixmap, &BannerWidth, &BannerHeight); 
    }
    /**
     * Creates Bannerpixmap if banner's dimensions don't mix
     */
    if((BannerWidth != RequiredWidth)||(BannerHeight != RequiredHeight)){
        if(NULL != DBannerPixmap){
             g_object_unref(DBannerPixmap);
        }
        DBannerPixmap = gdk_pixmap_new(DDrawingArea->window, RequiredWidth, RequiredHeight, -1);
    }

    DrawTextFrame(DBannerPixmap, RequiredWidth, RequiredHeight); /* draws text frame */
    TextX = (DCanvasWidth / 2) - (TextRequiredWidth / 2); /* determines textX */
    TextY = (RequiredHeight / 2) - (TextRequiredHeight / 2); /* determines textY */
    DBlackFont.DrawText(DBannerPixmap, DDrawingContext, TextX + 1, TextY + 1, message); /* draws text in black font */
    DWhiteFont.DrawText(DBannerPixmap, DDrawingContext, TextX, TextY, message); /* draws text in white font */
}
/**
 * Draws message 
 */
void CApplicationData::DrawMessage(const std::string &message){
    gint MessageWidth = 0; /* message's width */
    gint MessageHeight = 0; /* message's height */
    gint RequiredWidth, RequiredHeight; /* required height and weight */
    gint TextRequiredWidth = 0, TextRequiredHeight = 0; /*required width for text and height for text */
    gint TextX, TextY; /* text x and text y */ 
    std::list< std::string > Lines; /* string of lines */ 
    std::size_t Anchor = 0; /* sets Anchor to 0 */
    /**
     * uses while loop to draw message while pushing back each letter
     */    
    while(Anchor < message.length()){
        std::size_t NewPos = message.find("\n",Anchor);
        if(std::string::npos == NewPos){
            Lines.push_back(message.substr(Anchor));
            Anchor = message.length();
        }
        else{
            Lines.push_back(message.substr(Anchor, NewPos - Anchor));
            Anchor = NewPos + 1;
        }
    }
    /**
     * for loop ensures the required width/height of text of current line
     */ 
    for(std::list< std::string >::iterator CurLine = Lines.begin(); CurLine != Lines.end(); CurLine++){
        gint LineHeight, LineWidth;
        DBlackFont.MeasureText(*CurLine, LineWidth, LineHeight); 
        if(TextRequiredWidth < LineWidth){
            TextRequiredWidth = LineWidth;
        }
        TextRequiredHeight += LineHeight + 2;
    }
    
    RequiredWidth = TextRequiredWidth + 1; /* required width */
    RequiredWidth += DBrickTileset.TileWidth(); /* add brick tile width to req width */
    RequiredWidth = ((RequiredWidth + DBrickTileset.TileWidth() - 1) / DBrickTileset.TileWidth()) * DBrickTileset.TileWidth(); /* sets the required width */
    RequiredHeight = TextRequiredHeight; /* required height */
    RequiredHeight += DBrickTileset.TileHeight() * 2; /* add brick tile height to req height */ 
    RequiredHeight = ((RequiredHeight + DBrickTileset.TileHeight() - 1) / DBrickTileset.TileHeight()) * DBrickTileset.TileHeight(); /* sets required height */
    /**
     * instantiates a messagepixmap
     */
    if(NULL != DMessagePixmap){
        gdk_pixmap_get_size(DMessagePixmap, &MessageWidth, &MessageHeight); 
    }
    if((MessageWidth != RequiredWidth)||(MessageHeight != RequiredHeight)){
        if(NULL != DMessagePixmap){
             g_object_unref(DMessagePixmap);
        }
        DMessagePixmap = gdk_pixmap_new(DDrawingArea->window, RequiredWidth, RequiredHeight, -1);
    }
    
    DrawTextFrame(DMessagePixmap, RequiredWidth, RequiredHeight); /* draws text frame */
    TextY = DBrickTileset.TileHeight() + 1; /* sets textY */
    /**
     * draws message in black font and white font
     */
    for(std::list< std::string >::iterator CurLine = Lines.begin(); CurLine != Lines.end(); CurLine++){
        gint LineHeight, LineWidth;
        DBlackFont.MeasureText(*CurLine, LineWidth, LineHeight); 
        TextX = (RequiredWidth / 2) - (LineWidth / 2);
        DBlackFont.DrawText(DMessagePixmap, DDrawingContext, TextX + 1, TextY + 1, *CurLine);
        DWhiteFont.DrawText(DMessagePixmap, DDrawingContext, TextX, TextY, *CurLine);    
        TextY += LineHeight + 2;
    }
    
}

/**
 * draws background of menu
 */
void CApplicationData::DrawMenuBackground(GdkPixmap *buffer, gint width, gint height){
    /**
     * draws tile according to height width
     * @param HeightOffset = height of indiv tile
     * @param WidthOffset = width of indiv tile
     */
    for(gint HeightOffset = 0; HeightOffset < height; HeightOffset += 8){
        if(HeightOffset & 0x8){
            for(gint WidthOffset = -(DBrickTileset.TileWidth()/2); WidthOffset < width; WidthOffset += DBrickTileset.TileWidth()){
                DBrickTileset.DrawTile(buffer, DDrawingContext, WidthOffset, HeightOffset, DBrickIndices[bbtSingle]);
            }
        }
        else{
            for(gint WidthOffset = 0; WidthOffset < width; WidthOffset += DBrickTileset.TileWidth()){
                DBrickTileset.DrawTile(buffer, DDrawingContext, WidthOffset, HeightOffset, DBrickIndices[bbtSingle]);
            }            
        }
    }
    DrawBrickFrame(buffer, 0, 0, width, height);    
}

/**
 * draws text frame
 */
void CApplicationData::DrawTextFrame(GdkPixmap *buffer, gint width, gint height){
    /**
     * fills up tiles as long as collective height/width of tiles is less than height/width of frame
     */
    for(gint HeightOffset = 0; HeightOffset < height; HeightOffset += D2DWallFloorTileset.TileHeight()){
        for(gint WidthOffset = 0; WidthOffset < width; WidthOffset += D2DWallFloorTileset.TileWidth()){
            D2DWallFloorTileset.DrawTile(buffer, DDrawingContext, WidthOffset, HeightOffset, D2DFloorIndices[DPlayerColor]);
        }
    }
    DrawBrickFrame(buffer, 0, 0, width, height);
}

/**
 * draws brick frame
 */
void CApplicationData::DrawBrickFrame(GdkPixmap *buffer, gint xoff, gint yoff, gint width, gint height){
    gint BrickType = 0;
    /**
     * draws brick frame as long as cumulative width offset is less than width
     */
    for(gint WidthOffset = 0; WidthOffset < width; WidthOffset += DBrickTileset.TileWidth()){
        DBrickTileset.DrawTile(buffer, DDrawingContext, xoff + WidthOffset, yoff, DBrickIndices[bbtTopCenter]);
        DBrickTileset.DrawTile(buffer, DDrawingContext, xoff + WidthOffset, yoff + height - DBrickTileset.TileHeight(), DBrickIndices[bbtBottomCenter]);
    }
    /**
     * draws brick frame as long as cumulative height is less than frame height
     */ 
    for(gint HeightOffset = 0; HeightOffset < height; HeightOffset += DBrickTileset.TileHeight()){
        DBrickTileset.DrawTile(buffer, DDrawingContext, xoff, yoff + HeightOffset, BrickType ? DBrickIndices[bbtLeft1] : DBrickIndices[bbtLeft0]);
        DBrickTileset.DrawTile(buffer, DDrawingContext, xoff + width - DBrickTileset.TileWidth(), yoff + HeightOffset, BrickType ? DBrickIndices[bbtRight1] : DBrickIndices[bbtRight0]);
        BrickType++;
        BrickType &= 0x1;
    }
    /**
     * draws the 4 corner tiles 
     */
    DBrickTileset.DrawTile(buffer, DDrawingContext, xoff, 0, DBrickIndices[bbtTopLeft]);
    DBrickTileset.DrawTile(buffer, DDrawingContext, xoff + width - DBrickTileset.TileWidth(), yoff, DBrickIndices[bbtTopRight]);
    DBrickTileset.DrawTile(buffer, DDrawingContext, xoff + width - DBrickTileset.TileWidth(), yoff + height - DBrickTileset.TileHeight(), DBrickIndices[bbtBottomRight]);
    DBrickTileset.DrawTile(buffer, DDrawingContext, xoff, yoff + height - DBrickTileset.TileHeight(), DBrickIndices[bbtBottomLeft]);
    /**
     * draws the corner mortar tiles
     */
    DMortarTileset.DrawTile(buffer, DDrawingContext, xoff, yoff - 5, DMortarIndices[bmtLeftTop2]);
    DMortarTileset.DrawTile(buffer, DDrawingContext, xoff + width - DMortarTileset.TileWidth(), yoff - 5, DMortarIndices[bmtRightTop2]);
    DMortarTileset.DrawTile(buffer, DDrawingContext, xoff,  yoff + height -DMortarTileset.TileHeight() + 5, DMortarIndices[bmtLeftBottom2]);
    DMortarTileset.DrawTile(buffer, DDrawingContext, xoff + width - DMortarTileset.TileWidth(), yoff + height -DMortarTileset.TileHeight() + 5, DMortarIndices[bmtRightBottom2]);
    /**
     * draws mortar tiles according to frame's width
     * sets index according to index of mortars where center = 0
     */
    for(gint WidthOffset = 0; WidthOffset < width; WidthOffset += DMortarTileset.TileWidth()){
        int TopIndex, BottomIndex; /* top index and bottom index of width */
        int CenterPoint = WidthOffset + DMortarTileset.TileWidth() / 2; /* center point = middle of width */
        int BrickDistance = (width / 2 - CenterPoint) / DMortarTileset.TileWidth(); /* distance of bricks */ 
        
        if(-3 >= BrickDistance){
            TopIndex = DMortarIndices[bmtTopRight2];
            BottomIndex = DMortarIndices[bmtBottomRight2];
        }
        else if(-2 == BrickDistance){
            TopIndex = DMortarIndices[bmtTopRight1];
            BottomIndex = DMortarIndices[bmtBottomRight1];
        }
        else if(-1 == BrickDistance){
            TopIndex = DMortarIndices[bmtTopRight0];
            BottomIndex = DMortarIndices[bmtBottomRight0];
        }
        else if(0 == BrickDistance){
            TopIndex = DMortarIndices[bmtTopCenter];
            BottomIndex = DMortarIndices[bmtBottomCenter];
        }
        else if(1 == BrickDistance){
            TopIndex = DMortarIndices[bmtTopLeft0];
            BottomIndex = DMortarIndices[bmtBottomLeft0];
        }
        else if(2 == BrickDistance){
            TopIndex = DMortarIndices[bmtTopLeft1];
            BottomIndex = DMortarIndices[bmtBottomLeft1];
        }
        else{
            TopIndex = DMortarIndices[bmtTopLeft2];
            BottomIndex = DMortarIndices[bmtBottomLeft2];
        }
        
        DMortarTileset.DrawTile(buffer, DDrawingContext, xoff + WidthOffset, yoff, TopIndex); /* draws a row of tiles at the top index */
        DMortarTileset.DrawTile(buffer, DDrawingContext, xoff + WidthOffset, yoff + height - DMortarTileset.TileHeight(), BottomIndex); /* draws tiles at bottom */
    }

    /**
     * determines the left index and right index of the frame
     */
    for(gint HeightOffset = 3; HeightOffset < height-DMortarTileset.TileHeight(); HeightOffset += 8){
        int LeftIndex, RightIndex;
        int CenterPoint = HeightOffset + DMortarTileset.TileHeight() / 2;
        int BrickDistance = (height / 2 - CenterPoint) / 8;
        
        if(3 <= BrickDistance){
            LeftIndex = DMortarIndices[bmtLeftTop2];
            RightIndex = DMortarIndices[bmtRightTop2];
        }
        else if(2 == BrickDistance){
            LeftIndex = DMortarIndices[bmtLeftTop1];
            RightIndex = DMortarIndices[bmtRightTop1];
        }
        else if(1 == BrickDistance){
            LeftIndex = DMortarIndices[bmtLeftTop0];
            RightIndex = DMortarIndices[bmtRightTop0];
        }
        else if(0 == BrickDistance){
            LeftIndex = DMortarIndices[bmtLeftCenter];
            RightIndex = DMortarIndices[bmtRightCenter];
        }
        else if(-1 == BrickDistance){
            LeftIndex = DMortarIndices[bmtLeftBottom0];
            RightIndex = DMortarIndices[bmtRightBottom0];
        }
        else if(-2 == BrickDistance){
            LeftIndex = DMortarIndices[bmtLeftBottom1];
            RightIndex = DMortarIndices[bmtRightBottom1];
        }
        else{
            LeftIndex = DMortarIndices[bmtLeftBottom2];
            RightIndex = DMortarIndices[bmtRightBottom2];
        }
        
        DMortarTileset.DrawTile(buffer, DDrawingContext, xoff, yoff + HeightOffset, LeftIndex); /* draws the mortar tiles on left column */
        DMortarTileset.DrawTile(buffer, DDrawingContext, xoff + width - DMortarTileset.TileWidth(), yoff + HeightOffset, RightIndex); /* draws tile on right col */

    }
}

/**
 * Lets user end game
 */
void CApplicationData::GameOverMode(){

    if(DLeftClick[DPlayerColor]){

        ChangeMode(gmMainMenu);   /* change mode to main menu if left clicks */
    }
}
/**
 * turns on transition mode 
 */
void CApplicationData::TransitionMode(){
    gint BannerWidth = 0, BannerHeight = 0; /* banner width / height = 0 */
    bool LastBannerOffscreen = DBannerLocation < 0;
    /**
     * if bannerpixmap exists, get its width/height 
     */
    if(NULL != DBannerPixmap){
        gdk_pixmap_get_size(DBannerPixmap, &BannerWidth, &BannerHeight); 
    }
    DBannerLocation += DCanvasHeight / TIMEOUT_INTERVAL;
    /**
     * if banner goes away, play sound clip
     */
    if(LastBannerOffscreen && (0 <= DBannerLocation)){
        DSoundMixer.PlayClip(DSoundClipIndices[sctTransition], DSoundEffectVolume, 0.0);   
    }
    /**
     * if banner is higher than canvas, do transitions accordingly
     */
    if(DBannerLocation >= DCanvasHeight){
        for(int ColorIndex = pcBlue; ColorIndex < pcMax; ColorIndex++){
            DCompletedStage[ColorIndex] = false;
            DAITargetX[ColorIndex] = -1;
            DAITargetY[ColorIndex] = -1;
            DAITargetCastle[ColorIndex] = -1;
        }

        if(gmTransitionCannonPlacement == DGameMode){
            DNextGameMode = gmCannonPlacement;
            gettimeofday(&DCurrentStageTimeout,NULL);
            DCurrentStageTimeout.tv_sec += PLACEMENT_TIME;
            DSoundMixer.PlaySong(DSongIndices[stPlace], DMusicVolume);
        }
        else if(gmTransitionSelectCastle == DGameMode){
            DNextGameMode = gmSelectCastle;
            gettimeofday(&DCurrentStageTimeout,NULL);
            DCurrentStageTimeout.tv_sec += SELECT_TIME;
        }
        else if(gmTransitionBattle == DGameMode){
            DBattleModeStage = bmsReady;
            DNextGameMode = gmBattle;
            gettimeofday(&DCurrentStageTimeout,NULL);
            DCurrentStageTimeout.tv_sec += BATTLE_TIME;
        }
        else if(gmTransitionRebuild == DGameMode){
            DNextGameMode = gmRebuild;
            gettimeofday(&DCurrentStageTimeout,NULL);
            DCurrentStageTimeout.tv_sec += REBUILD_TIME;
            DSoundMixer.PlaySong(DSongIndices[stRebuild], DMusicVolume);
        }
    }
    else if(DBannerLocation + BannerHeight >= DCanvasHeight){
        if(gmTransitionBattle == DGameMode){
            if(0 > DVoiceClipID){
                DVoiceClipID = DSoundMixer.PlayClip(DSoundClipIndices[sctReady], DSoundEffectVolume, 0.0);
                gettimeofday(&DCurrentStageTimeout,NULL);
                DCurrentStageTimeout.tv_sec += 30;
            }
        }
    }
}

/**
 * lets user go back to main menu
 */
void CApplicationData::MainMenuMode(){
    DSelectedMenuItem = DCurrentY[pcNone] / (DCanvasHeight /  DMenuItems.size());
    
    if(DLeftClick[pcNone]){
        // Single Player Game
        if(0 == DSelectedMenuItem){
            DNextGameMode = gmSelectMap;
            DLastSelectedMenuItem = -1;
            DSoundMixer.PlayClip(DSoundClipIndices[sctPlace], DSoundEffectVolume, 0.0);
        }
        else if(1 == DSelectedMenuItem){
            
        }
        else if(2 == DSelectedMenuItem){
            ChangeMode(gmOptionsMenu);
        }
        else{
            gtk_main_quit();
        }
    }
    else if((0 <= DLastSelectedMenuItem)&&(DLastSelectedMenuItem != DSelectedMenuItem)){
        DSoundMixer.PlayClip(DSoundClipIndices[sctTick], DSoundEffectVolume, 0.0);
    }
    DLastSelectedMenuItem = DSelectedMenuItem;
}
/** 
 * lets user select map
 */
void CApplicationData::SelectMapMode(){
    DSelectedMenuItem = DCurrentY[pcNone] / (DCanvasHeight /  (DTerrainMaps.size() + 1)); 
    
    if(DLeftClick[pcNone]){
        DSoundMixer.PlayClip(DSoundClipIndices[sctPlace], DSoundEffectVolume, 0.0);
        if(DSelectedMenuItem < DTerrainMaps.size()){
            LoadTerrainMap(DSelectedMenuItem);
            ChangeMode(gmSelectCastle);
        }
        else{
            ChangeMode(gmMainMenu);
        }
    }
    else if((0 <= DLastSelectedMenuItem)&&(DLastSelectedMenuItem != DSelectedMenuItem)){
        DSoundMixer.PlayClip(DSoundClipIndices[sctTick], DSoundEffectVolume, 0.0);
    }
    DLastSelectedMenuItem = DSelectedMenuItem;
}

/**
  * lets user go to options menu
  */
void CApplicationData::OptionsMenuMode(){
    DSelectedMenuItem = DCurrentY[pcNone] / (DCanvasHeight /  DMenuItems.size());
    
    if(DLeftClick[pcNone]){
        if(0 == DSelectedMenuItem){
            ChangeMode(gmSoundOptions);  /* changes mode to sound options */
        }
        else if(1 == DSelectedMenuItem){

        }
        else{
            ChangeMode(gmMainMenu);   /* changes to the main menu */
        }
    }
    else if((0 <= DLastSelectedMenuItem)&&(DLastSelectedMenuItem != DSelectedMenuItem)){
        DSoundMixer.PlayClip(DSoundClipIndices[sctTick], DSoundEffectVolume, 0.0);
    }
    DLastSelectedMenuItem = DSelectedMenuItem; 
}

/**
 *  lets user choose sound options
 */
void CApplicationData::SoundOptionsMode(){
    DSelectedMenuItem = DCurrentY[pcNone] / (DCanvasHeight /  3); 
    DSelectedMenuItem *= 2;
    if(DCurrentX[pcNone] > (DCanvasWidth/2)){
        DSelectedMenuItem++;
    }
    if(DLeftClick[pcNone]){
        if(0 == DSelectedMenuItem){
            DSoundEffectVolume -= 0.05;
            if(0.0 > DSoundEffectVolume){
                DSoundEffectVolume = 0.0;
            }
            DSoundMixer.PlayClip(DSoundClipIndices[sctTock], DSoundEffectVolume, 0.0);
        }
        else if(1 == DSelectedMenuItem){
            DSoundEffectVolume += 0.05;
            if(1.0 < DSoundEffectVolume){
                DSoundEffectVolume = 1.0;
            }
            DSoundMixer.PlayClip(DSoundClipIndices[sctTock], DSoundEffectVolume, 0.0);
        }
        else if(2 == DSelectedMenuItem){
            DMusicVolume -= 0.05;
            if(0.0 > DMusicVolume){
                DMusicVolume = 0.0;
            }
            DSoundMixer.SongVolume(DMusicVolume);
        }
        else if(3 == DSelectedMenuItem){
            DMusicVolume += 0.05;
            if(1.0 < DMusicVolume){
                DMusicVolume = 1.0;
            }
            DSoundMixer.SongVolume(DMusicVolume);
        }
        else{
            ChangeMode(gmMainMenu);
        }
    }
    else if((0 <= DLastSelectedMenuItem)&&(DLastSelectedMenuItem != DSelectedMenuItem)){
        DSoundMixer.PlayClip(DSoundClipIndices[sctTick], DSoundEffectVolume, 0.0);
    }
    DLastSelectedMenuItem = DSelectedMenuItem;
}

void CApplicationData::SelectCastleMode(){
    int XTile, YTile;
    int XPos, YPos;
    int BestDistance = 999999;
    int BestCastle[pcMax];
    bool AllPlayersCompleted = true;
    
    for(int ColorIndex = pcBlue; ColorIndex < pcMax; ColorIndex++){
        if(!DCompletedStage[ColorIndex]){
            XTile = DCurrentX[ColorIndex] / DTileWidth;
            YTile = DCurrentY[ColorIndex] / DTileHeight;
            BoundsCheck(XTile, YTile);
            BestDistance = 999999;
            BestCastle[ColorIndex] = -1;
            
            for(int Index = 0; Index < DSurroundedCastles[ColorIndex].size(); Index++){
                int Distance;
                XPos = DCastleLocations[ColorIndex][Index].DXIndex;
                YPos = DCastleLocations[ColorIndex][Index].DYIndex;
                
                Distance = (XPos - XTile) * (XPos - XTile) + (YPos - YTile) * (YPos - YTile);
                DSurroundedCastles[ColorIndex][Index] = false;
                if(Distance < BestDistance){
                    BestDistance = Distance;
                    BestCastle[ColorIndex] = Index;
                } 
            }
            if(0 <= BestCastle[ColorIndex]){
                DSurroundedCastles[ColorIndex][BestCastle[ColorIndex]] = true;
            }
        }
    }
    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        if(DLeftClick[ColorIndex] && !DCompletedStage[ColorIndex]){
            EConstructionTileType ConTileType, WallType;
            double LRRatio;
                
            XPos = DCastleLocations[ColorIndex][BestCastle[ColorIndex]].DXIndex;
            YPos = DCastleLocations[ColorIndex][BestCastle[ColorIndex]].DYIndex;
            
            if(pcBlue == ColorIndex){
                ConTileType = cttBlueFloor;
                WallType = cttBlueWall;
            }
            else if(pcRed == ColorIndex){
                ConTileType = cttRedFloor;
                WallType = cttRedWall;
            }
            else{
                ConTileType = cttYellowFloor;
                WallType = cttYellowWall;
            }
            
            for(int YOffset = -3; YOffset <= 4; YOffset++){
                int YI = YPos + YOffset;
                for(int XOffset = -3; XOffset <= 4; XOffset++){
                    int XI = XPos + XOffset;
                    char TileType = DTerrainMap.TileType(XI, YI);
    
                    if(TileType == ColorIndex){
                        DConstructionTiles[YI][XI] = ConTileType;       
                    }
                }
            }
            for(int YOffset = -3; YOffset <= 4; YOffset++){
                int YI = YPos + YOffset;
                for(int XOffset = -3; XOffset <= 4; XOffset++){
                    int XI = XPos + XOffset;
                    
                    if(ConTileType == DConstructionTiles[YI][XI]){
                        bool MakeWall = false;
                        if(YI){
                            if(XI && (cttNone == DConstructionTiles[YI-1][XI-1])){
                                MakeWall = true;   
                            }
                            if(cttNone == DConstructionTiles[YI-1][XI]){
                                MakeWall = true;
                            }
                            if((XI + 1 < DConstructionTiles[YI].size()) && (cttNone == DConstructionTiles[YI-1][XI+1])){
                                MakeWall = true;
                            }
                        }
                        else{
                            MakeWall = true;   
                        }
                        if(XI){
                            if(cttNone == DConstructionTiles[YI][XI-1]){
                                MakeWall = true;
                            }
                        }
                        else{
                            MakeWall = true;   
                        }
                        if(XI + 1 < DConstructionTiles[YI].size()){
                            if(cttNone == DConstructionTiles[YI][XI+1]){
                                MakeWall = true;
                            }
                        }
                        else{
                            MakeWall = true;    
                        }
                        if(YI + 1 < DConstructionTiles.size()){
                            if(XI && (cttNone == DConstructionTiles[YI+1][XI-1])){
                                MakeWall = true;
                            }
                            if(cttNone == DConstructionTiles[YI+1][XI]){
                                MakeWall = true;
                            }
                            if((XI + 1 < DConstructionTiles[YI].size()) && (cttNone == DConstructionTiles[YI+1][XI+1])){
                                MakeWall = true;
                            }
                        }
                        else{
                            MakeWall = true;    
                        }
                        if(MakeWall){
                            DConstructionTiles[YI][XI] = WallType;
                        }
                    }
                }
            }
            
            LRRatio = (((double)XPos / (DMapWidth - 1)) - 0.5) * 2.0;
            DSoundMixer.PlayClip(DSoundClipIndices[sctTriumph], DSoundEffectVolume, LRRatio);

            DCompletedStage[ColorIndex] = true;
        }
        if(!DCompletedStage[ColorIndex]){
            AllPlayersCompleted = false;
        }
    }   
    
    if(AllPlayersCompleted){
        for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
            DWallShape[ColorIndex].Randomize(DRandomNumberGenerator.Random());
            DCannonsToPlace[ColorIndex] = 3;
        }
        
        ChangeMode(gmCannonPlacement);
    }
}

void CApplicationData::RebuildMode(){
    int XTile, YTile;
    int XPos, YPos;
    char RequiredTileType;
    EConstructionTileType WallType;
    bool Done, AllPlayersCompleted;
    int SurroundedBefore = 0, SurroundedAfter = 0;

    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        if(!DCompletedStage[ColorIndex]){
            XTile = DCurrentX[ColorIndex] / DTileWidth;
            YTile = DCurrentY[ColorIndex] / DTileHeight;
            BoundsCheck(XTile, YTile);
            if(XTile + DWallShape[ColorIndex].Width() >= DMapWidth){
                XTile = DMapWidth - DWallShape[ColorIndex].Width();
            }
            if(YTile + DWallShape[ColorIndex].Height() >= DMapHeight){
                YTile = DMapHeight - DWallShape[ColorIndex].Height();
            }
            if(DRightClick[ColorIndex]){
                DWallShape[ColorIndex].Rotate();
            }
            RequiredTileType = ColorIndex;
            if(pcBlue == ColorIndex){
                WallType = cttBlueWall;
            }
            else if(pcRed == ColorIndex){
                WallType = cttRedWall;
            }
            else{
                WallType = cttYellowWall;
            }
    
            do{
                Done = true;
                DCanPlaceWallCannon[ColorIndex] = ValidWallPlacement(ColorIndex, XTile, YTile);
                if(DCanPlaceWallCannon[ColorIndex] && DLeftClick[ColorIndex]){
                    double LRRatio = (((double)XTile / (DMapWidth - 1)) - 0.5) * 2.0;
                    
                    for(int WallYPos = 0; WallYPos < DWallShape[ColorIndex].Height(); WallYPos++){
                        YPos = YTile + WallYPos;
                        for(int WallXPos = 0; WallXPos < DWallShape[ColorIndex].Width(); WallXPos++){
                            XPos = XTile + WallXPos;
                            if(DWallShape[ColorIndex].IsBlock(WallXPos, WallYPos)){
                                DConstructionTiles[YPos][XPos] = WallType;
                            }
                        }
                    }
                    if(0 < SecondsUntilDeadline(DCurrentStageTimeout)){
                        DWallShape[ColorIndex].Randomize(DRandomNumberGenerator.Random());
                        Done = false;
                    }
                    else{
                        DCompletedStage[ColorIndex] = true;
                    }
                    DLeftClick[ColorIndex] = false;
                    
                    DSoundMixer.PlayClip(DSoundClipIndices[sctPlace], DSoundEffectVolume, LRRatio);
                }
            }while(!Done);
        }
    }

    for(int ColorIndex = pcBlue; ColorIndex < pcMax; ColorIndex++){
        for(int Index = 0; Index < DSurroundedCastles[ColorIndex].size(); Index++){
            if(DSurroundedCastles[ColorIndex][Index]){
                SurroundedBefore++;
            }
        }
    }    
    CheckSurroundedCastles();
    for(int ColorIndex = pcBlue; ColorIndex < pcMax; ColorIndex++){
        for(int Index = 0; Index < DSurroundedCastles[ColorIndex].size(); Index++){
            if(DSurroundedCastles[ColorIndex][Index]){
                SurroundedAfter++;
            }
        }
    }
    if(SurroundedBefore != SurroundedAfter){
        DSoundMixer.PlayClip(DSoundClipIndices[sctTriumph], DSoundEffectVolume, 0.0);   
    }
    
    AllPlayersCompleted = true;
    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        if(!DCompletedStage[ColorIndex]){
            AllPlayersCompleted = false;
            break;
        }
    }

    PlayTickTockSound();
    if((-3 >= SecondsUntilDeadline(DCurrentStageTimeout))||AllPlayersCompleted){
        int LivingPlayers;
        
        LivingPlayers = DetermineConquered();
        if(1 < LivingPlayers){
            for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
                DCannonsToPlace[ColorIndex] = DSurroundedCastleCounts[ColorIndex];
            }
            DSoundMixer.StopSong();
            ChangeMode(gmCannonPlacement);
        }
        else if(1 == LivingPlayers){
            std::string Message;
            // Show winner   
            if(pcBlue == DPlayerColor){
                Message = "BLUE\n";
            }
            else if(pcRed == DPlayerColor){
                Message = "RED\n";                    
            }
            else{
                Message = "YELLOW\n";
            }
            if(DPlayerIsConquered[DPlayerColor]){
                Message += "ARMY\nDEFEATED";
                DSoundMixer.PlaySong(DSongIndices[stLoss], DMusicVolume);
            }
            else{
                Message += "ARMY\nCONQUERS";
                DSoundMixer.PlaySong(DSongIndices[stWin], DMusicVolume * 2.0);
            }
            DrawMessage(Message);
            DNextGameMode = gmGameOver;
        }
        else{
            // Do rebuild because all would die
            gettimeofday(&DCurrentStageTimeout,NULL);
            DCurrentStageTimeout.tv_sec += REBUILD_TIME;
            for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
                DCompletedStage[ColorIndex] = false;
            }
        }
    }
}

void CApplicationData::CannonPlacementMode(){
    int XTile, YTile;
    bool AllPlayersCompleted = true;
    
    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        XTile = DCurrentX[ColorIndex] / DTileWidth;
        YTile = DCurrentY[ColorIndex] / DTileHeight;
        BoundsCheck(XTile, YTile);
        
        if(DLeftClick[ColorIndex] && DCannonsToPlace[ColorIndex]){
            if(ValidCannonPlacement(ColorIndex, XTile, YTile)){
                SMapLocation TempLocation;
                double LRRatio = (((double)XTile / (DMapWidth - 1)) - 0.5) * 2.0;
                
                TempLocation.DXIndex = XTile;
                TempLocation.DYIndex = YTile;
                
                DCannonLocations[ColorIndex].push_back(TempLocation);
                DCannonballToneIDs[ColorIndex].push_back(-1);
                DCannonsToPlace[ColorIndex]--;
                if(0 == DCannonsToPlace[ColorIndex]){
                    DCompletedStage[ColorIndex] = true;
                }
                DSoundMixer.PlayClip(DSoundClipIndices[sctPlace], DSoundEffectVolume, LRRatio);
            }
        }
        if(!DCompletedStage[ColorIndex]){
            AllPlayersCompleted = false;   
        }
    }
    PlayTickTockSound();
    if(AllPlayersCompleted || (-3 >= SecondsUntilDeadline(DCurrentStageTimeout))){
        DSoundMixer.StopSong();
        ChangeMode(gmBattle);
        
        for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
            bool Done;
            
            DAITargetX[ColorIndex] = -1;
            DAITargetY[ColorIndex] = -1;
            
            do{
                Done = true;
                for(int CannonIndex = 1; CannonIndex < DCannonLocations[ColorIndex].size(); CannonIndex++){
                    bool Swap = false;
                    if(DCannonLocations[ColorIndex][CannonIndex].DYIndex < DCannonLocations[ColorIndex][CannonIndex-1].DYIndex){
                        Swap = true;
                    }
                    else if((DCannonLocations[ColorIndex][CannonIndex].DYIndex == DCannonLocations[ColorIndex][CannonIndex-1].DYIndex)&&(DCannonLocations[ColorIndex][CannonIndex].DXIndex < DCannonLocations[ColorIndex][CannonIndex-1].DXIndex)){
                        Swap = true;
                    }
                    if(Swap){
                        SMapLocation TempLocation = DCannonLocations[ColorIndex][CannonIndex];
                        
                        DCannonLocations[ColorIndex][CannonIndex] = DCannonLocations[ColorIndex][CannonIndex-1];
                        DCannonLocations[ColorIndex][CannonIndex-1] = TempLocation;
                        Done = false;
                    }
                }
            }while(!Done);
            DCannonsReady[ColorIndex].clear();
            for(int CannonIndex = 0; CannonIndex < DCannonLocations[ColorIndex].size(); CannonIndex++){
                if(TileTypeIsFloor(DConstructionTiles[DCannonLocations[ColorIndex][CannonIndex].DYIndex][DCannonLocations[ColorIndex][CannonIndex].DXIndex])){
                    DCannonsReady[ColorIndex].push_back(CannonIndex);
                    DCannonballToneIDs[ColorIndex][CannonIndex] = -1;
                }
            }            
        }
    }
}

void CApplicationData::BattleMode(){
    bool WindDirectionChange = false;
    double WindX = 0, WindY = 0;
    
    switch(DWindDirection){
        case 1:     WindY = -DWindSpeed;
                    break;
        case 2:     WindX = DWindSpeed;
                    WindY = -DWindSpeed;
                    break;
        case 3:     WindX = DWindSpeed;
                    break;    
        case 4:     WindX = DWindSpeed;
                    WindY = DWindSpeed;
                    break;
        case 5:     WindY = DWindSpeed;
                    break;
        case 6:     WindX = -DWindSpeed;
                    WindY = DWindSpeed;
                    break;
        case 7:     WindX = -DWindSpeed;
                    break;
        case 8:     WindX = -DWindSpeed;
                    WindY = -DWindSpeed;
                    break;
        default:    break;
    }
    WindX /= 10;
    WindY /= 10;
    WindX *= TIMESTEP_PERIOD;
    WindY *= TIMESTEP_PERIOD;
    

    if(bmsBattle == DBattleModeStage){
        for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
            if(DLeftClick[ColorIndex] && DCannonsReady[ColorIndex].size() && (0 <= SecondsUntilDeadline(DCurrentStageTimeout))){
                SCannonballTrajectory InitialTrajectory;
                SSpriteState TempPlumeState;
                int CannonIndex = DCannonsReady[ColorIndex].front();
                int CenterX, CenterY;
                int DeltaX, DeltaY;
                int TargetX, TargetY;
                int Distance;
                double LRRatio;
                
                DCannonsReady[ColorIndex].pop_front();
                                    
                CenterX = (DCannonLocations[ColorIndex][CannonIndex].DXIndex + 1) * DTileWidth;
                CenterY = (DCannonLocations[ColorIndex][CannonIndex].DYIndex + 1) * DTileHeight;
                
                TargetX = DCurrentX[ColorIndex];
                TargetY = DCurrentY[ColorIndex] + DTileHeight/2;
                
                DeltaX = TargetX - CenterX;
                DeltaY = TargetY - CenterY;
                Distance = IntegerSquareRoot(DeltaX * DeltaX + DeltaY * DeltaY);
                
                InitialTrajectory.DXPosition = CenterX;
                InitialTrajectory.DYPosition = CenterY;
                InitialTrajectory.DZPosition = 0;
                InitialTrajectory.DZVelocity = DInitialVelocities[Distance];
                InitialTrajectory.DXVelocity = DeltaX * InitialTrajectory.DZVelocity / Distance;
                InitialTrajectory.DYVelocity = DeltaY * InitialTrajectory.DZVelocity / Distance;
                InitialTrajectory.DOwner = (EPlayerColor)ColorIndex;
                InitialTrajectory.DCannon = CannonIndex;
                
                DCannonballTrajectories.push_back(InitialTrajectory);
                
                TempPlumeState.DXIndex = DCannonLocations[ColorIndex][CannonIndex].DXIndex;
                TempPlumeState.DYIndex = DCannonLocations[ColorIndex][CannonIndex].DYIndex;
                TempPlumeState.DXOffset = 0;
                TempPlumeState.DYOffset = 0;
                TempPlumeState.DStep = 0;
                TempPlumeState.DSpriteIndex = D3DCannonPlumeIndices[CalculateDirection(CenterX, CenterY, TargetX, TargetY)];
                DPlumeStates.push_back(TempPlumeState);
                
                LRRatio = (((double)CenterX / (DCanvasWidth - 1)) - 0.5) * 2.0;
                DSoundMixer.PlayClip(DSoundClipIndices[DRandomNumberGenerator.Random() & 0x1 ? sctCannon1 : sctCannon0], DSoundEffectVolume, LRRatio);
            }
        }
    }
    for(std::list< SSpriteState >::iterator Burn = DBurnStates.begin(); Burn != DBurnStates.end(); ){
        bool Advance = true;
        
        Burn->DStep++;
        if(DBurnSteps <= Burn->DStep){
            Burn = DBurnStates.erase(Burn);
            Advance = false;
        }        
        if(Advance){
            Burn++;   
        }
    }
    for(std::list< SSpriteState >::iterator Explosion = DExplosionStates.begin(); Explosion != DExplosionStates.end(); ){
        bool Advance = true;
        
        Explosion->DStep++;
        if(DExplosionSteps <= Explosion->DStep){
            int TempXTile, TempYTile;
            bool InBounds;
            TempXTile = Explosion->DXIndex;
            TempYTile = Explosion->DYIndex;
            InBounds = BoundsCheck(TempXTile, TempYTile);
            if(InBounds){
                if(TileTypeIsWallDamaged(DConstructionTiles[TempYTile][TempXTile])){
                    Explosion->DStep = 0;
                    
                    Explosion->DSpriteIndex = D3DBurnIndices[(DRandomNumberGenerator.Random() & 0x1) ? btRubbleBurn1 : btRubbleBurn0];
                    DBurnStates.push_back(*Explosion);
                }
                if(TileTypeIsFloorGroundDamaged(DConstructionTiles[TempYTile][TempXTile])){
                    Explosion->DStep = 0;
                    Explosion->DSpriteIndex = D3DBurnIndices[(DRandomNumberGenerator.Random() & 0x1) ? btHoleBurn1 : btHoleBurn0];
                    DBurnStates.push_back(*Explosion);
                }
            }
            Explosion = DExplosionStates.erase(Explosion);
            Advance = false;
        }        
        if(Advance){
            Explosion++;   
        }
    }
    for(std::list< SSpriteState >::iterator Plume = DPlumeStates.begin(); Plume != DPlumeStates.end(); ){
        bool Advance = true;
        
        Plume->DStep++;
        if(DCannonPlumeSteps <= Plume->DStep){
            Plume = DPlumeStates.erase(Plume);
            Advance = false;
        }        
        if(Advance){
            Plume++;   
        }
    }
    for(std::list< SCannonballTrajectory >::iterator Cannonball = DCannonballTrajectories.begin(); Cannonball != DCannonballTrajectories.end(); ){
        bool Advance = true;
        Cannonball->DXVelocity += WindX;
        Cannonball->DYVelocity += WindY;
        Cannonball->DXPosition += Cannonball->DXVelocity * TIMESTEP_PERIOD;
        Cannonball->DYPosition += Cannonball->DYVelocity * TIMESTEP_PERIOD;
        Cannonball->DZPosition += Cannonball->DZVelocity * TIMESTEP_PERIOD;
        Cannonball->DZVelocity -= STANDARD_GRAVITY * TIMESTEP_PERIOD;
        if(0 > Cannonball->DZVelocity){
            int TempXTile, TempYTile;
            bool Collision = false;
            bool InBounds, AltExplosion;
            SSpriteState TempExplosionState;
            
            if(0 > DCannonballToneIDs[Cannonball->DOwner][Cannonball->DCannon]){
                int CannonBallSize = CalculateCannonballSize(Cannonball->DZPosition);
                DCannonballToneIDs[Cannonball->DOwner][Cannonball->DCannon] = DSoundMixer.PlayTone(2500.0 + (CannonBallSize / 12.0) * 500.0, -500.0, DSoundEffectVolume * CannonBallSize / 36.0, -0.1 * DSoundEffectVolume,  (((double)Cannonball->DXPosition / DCanvasWidth) - 0.5) * 2.0, Cannonball->DXVelocity / (2.0 * DCanvasWidth));
            }
            AltExplosion = DRandomNumberGenerator.Random() & 0x01;
            TempXTile = Cannonball->DXPosition / DTileWidth;
            TempYTile = Cannonball->DYPosition / DTileHeight;
            InBounds = BoundsCheck(TempXTile, TempYTile);
            if(InBounds){
                if((DTileHeight * 2 > Cannonball->DZPosition)&&TileTypeIsWall(DConstructionTiles[TempYTile][TempXTile])){
                    // Hit wall
                    TempExplosionState.DXIndex = TempXTile;
                    TempExplosionState.DYIndex = TempYTile;
                    TempExplosionState.DXOffset = 0;
                    TempExplosionState.DYOffset = 0;
                    TempExplosionState.DStep = 0;
                    
                    TempExplosionState.DSpriteIndex = D3DExplosionIndices[AltExplosion ? etWallExplosion1 : etWallExplosion0];
                    Collision = true;   
                    switch(DTerrainMap.TileType(TempXTile, TempYTile)){
                        case pcBlue:    DConstructionTiles[TempYTile][TempXTile] = cttBlueWallDamaged;
                                        break;
                        case pcRed:     DConstructionTiles[TempYTile][TempXTile] = cttRedWallDamaged;
                                        break;
                        case pcYellow:  DConstructionTiles[TempYTile][TempXTile] = cttYellowWallDamaged;
                                        break;
                        default:        DConstructionTiles[TempYTile][TempXTile] = cttNone;
                                        TempExplosionState.DSpriteIndex = D3DExplosionIndices[AltExplosion ? etWaterExplosion1 : etWaterExplosion0];
                                        break;
                    }
                    DExplosionStates.push_back(TempExplosionState);
                    DSoundMixer.PlayClip(DSoundClipIndices[sctExplosion0 + (DRandomNumberGenerator.Random() % 4)], DSoundEffectVolume, (((double)TempXTile / (DMapWidth - 1)) - 0.5) * 2.0);
                }
                else if(0.0 >= Cannonball->DZPosition){
                    // Hit ground
                    TempExplosionState.DXIndex = TempXTile;
                    TempExplosionState.DYIndex = TempYTile;
                    TempExplosionState.DXOffset = (((int)Cannonball->DXPosition) % DTileWidth) - (DTileWidth/2);
                    TempExplosionState.DYOffset = (((int)Cannonball->DYPosition) % DTileHeight) - (DTileHeight/2);
                    TempExplosionState.DStep = 0;
                    TempExplosionState.DSpriteIndex = D3DExplosionIndices[AltExplosion ? etGroundExplosion1 : etGroundExplosion0];

                    if(pcNone == DTerrainMap.TileType(TempXTile, TempYTile)){
                        TempExplosionState.DSpriteIndex = D3DExplosionIndices[AltExplosion ? etWaterExplosion1 : etWaterExplosion0];
                        DSoundMixer.PlayClip(DSoundClipIndices[sctWaterExplosion0 + (DRandomNumberGenerator.Random() % 4)], DSoundEffectVolume, (((double)TempXTile / (DMapWidth - 1)) - 0.5) * 2.0);
                    }
                    else{
                        DHitsTaken[TempYTile][TempXTile]++;
                        if(TileTypeIsWallDamaged(DConstructionTiles[TempYTile][TempXTile])){
                            TempExplosionState.DXOffset = 0;  
                            TempExplosionState.DYOffset = 0;
                        }
                        DSoundMixer.PlayClip(DSoundClipIndices[sctGroundExplosion0 + (DRandomNumberGenerator.Random() % 2)], DSoundEffectVolume, (((double)TempXTile / (DMapWidth - 1)) - 0.5) * 2.0);
                    }
                    if(5 <= DHitsTaken[TempYTile][TempXTile]){
                        if(TileTypeIsFloor(DConstructionTiles[TempYTile][TempXTile]) && !CannonCastleInterfere(TempXTile, TempYTile, 1, 1)){
                            if(0 == (DRandomNumberGenerator.Random() % 5)){
                                switch(DTerrainMap.TileType(TempXTile, TempYTile)){
                                    case pcBlue:    DConstructionTiles[TempYTile][TempXTile] = cttBlueFloorDamaged;
                                                    break;
                                    case pcRed:     DConstructionTiles[TempYTile][TempXTile] = cttRedFloorDamaged;
                                                    break;
                                    case pcYellow:  DConstructionTiles[TempYTile][TempXTile] = cttYellowFloorDamaged;
                                                    break;
                                    default:        DConstructionTiles[TempYTile][TempXTile] = cttNone;
                                                    break;
                                }
                            }
                        }
                        else if(cttNone == DConstructionTiles[TempYTile][TempXTile]){
                            if(0 == (DRandomNumberGenerator.Random() % 5)){
                                DConstructionTiles[TempYTile][TempXTile] = cttGroundDamaged;
                            }
                        }
                        else if(TileTypeIsWallDamaged(DConstructionTiles[TempYTile][TempXTile])){
                            if(10 <= DHitsTaken[TempYTile][TempXTile]){
                                if(0 == (DRandomNumberGenerator.Random() % 5)){
                                    switch(DTerrainMap.TileType(TempXTile, TempYTile)){
                                        case pcBlue:    DConstructionTiles[TempYTile][TempXTile] = cttBlueFloorDamaged;
                                                        break;
                                        case pcRed:     DConstructionTiles[TempYTile][TempXTile] = cttRedFloorDamaged;
                                                        break;
                                        case pcYellow:  DConstructionTiles[TempYTile][TempXTile] = cttYellowFloorDamaged;
                                                        break;
                                        default:        DConstructionTiles[TempYTile][TempXTile] = cttNone;
                                                        break;
                                    }
                                }   
                            }
                        }
                    }    
                    if(TileTypeIsFloorGroundDamaged(DConstructionTiles[TempYTile][TempXTile])){
                        TempExplosionState.DXOffset = 0;  
                        TempExplosionState.DYOffset = 0;
                    }
                    DExplosionStates.push_back(TempExplosionState);
                    Collision = true;
                }
            }
            else if(0.0 >= Cannonball->DZPosition){
                Collision = true;
            }
            if(Collision || (DTileHeight > Cannonball->DZPosition)){
                DSoundMixer.StopTone(DCannonballToneIDs[Cannonball->DOwner][Cannonball->DCannon]);
                DCannonballToneIDs[Cannonball->DOwner][Cannonball->DCannon] = -1;
            }
            if(Collision){
                DCannonsReady[Cannonball->DOwner].push_back(Cannonball->DCannon);
                Advance = false;
                Cannonball = DCannonballTrajectories.erase(Cannonball);
            }
        }
        if(Advance){
            Cannonball++;    
        }
    }
    DCannonballTrajectories.sort(TrajectoryCompare);
    DExplosionStates.sort(SpriteCompare);
    DBurnStates.sort(SpriteCompare);
    if(DRightClick[DPlayerColor]){
        switch(DWindType){
            case wtNone:    DWindType = wtMild;
                            break;
            case wtMild:    DWindType = wtModerate;
                            break;
            case wtModerate:DWindType = wtErratic;
                            break;
            case wtErratic: DWindType = wtNone;
                            WindDirectionChange = true;
                            break;
        }
    }
    
    if(bmsBattle == DBattleModeStage){
        if((0 > DVoiceClipID)&&(MiliSecondsUntilDeadline(DCurrentStageTimeout) < (DSoundMixer.ClipDurationMS(DSoundClipIndices[sctCeasefire])/2))){
            DVoiceClipID = DSoundMixer.PlayClip(DSoundClipIndices[sctCeasefire], DSoundEffectVolume, 0.0);
        }
        if((0 == DCannonballTrajectories.size()) && (0 == DExplosionStates.size()) && (0 == DBurnStates.size()) && DSoundMixer.ClipCompleted(DVoiceClipID) && (0 > SecondsUntilDeadline(DCurrentStageTimeout))){
            ChangeMode(gmRebuild);
        }
    }
    else if(bmsReady == DBattleModeStage){
        if(DSoundMixer.ClipCompleted(DVoiceClipID)){
            if(1 < SecondsUntilDeadline(DCurrentStageTimeout)){
                gettimeofday(&DCurrentStageTimeout,NULL);
                DCurrentStageTimeout.tv_sec += 1;
            }
            else if(0 >= SecondsUntilDeadline(DCurrentStageTimeout)){
                DBattleModeStage = bmsAim;
                DVoiceClipID = DSoundMixer.PlayClip(DSoundClipIndices[sctAim], DSoundEffectVolume, 0.0);
                gettimeofday(&DCurrentStageTimeout,NULL);
                DCurrentStageTimeout.tv_sec += 30;
            }
        }
    }
    else if(bmsAim == DBattleModeStage){
        if(DSoundMixer.ClipCompleted(DVoiceClipID)){
            if(1 < SecondsUntilDeadline(DCurrentStageTimeout)){
                gettimeofday(&DCurrentStageTimeout,NULL);
                DCurrentStageTimeout.tv_sec += 1;
            }
            else if(0 >= SecondsUntilDeadline(DCurrentStageTimeout)){
                DBattleModeStage = bmsBattle;
                DSoundMixer.PlayClip(DSoundClipIndices[sctFire], DSoundEffectVolume, 0.0);
                gettimeofday(&DCurrentStageTimeout,NULL);
                DCurrentStageTimeout.tv_sec += BATTLE_TIME;
                DVoiceClipID = -1;
            }
        }
    }

    if(wtNone == DWindType){
        DWindDirection = 0;
        DWindSpeed = 1;
    }
    else{
        unsigned int ChangeProbability;
        unsigned int DirectionProbability;
        unsigned int SpeedProbability;
        int MinWindSpeed, MaxWindSpeed;

        if(wtMild == DWindType){
            ChangeProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.01));
            DirectionProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.01));
            SpeedProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.1));
            MinWindSpeed = 1;
            MaxWindSpeed = WINDSPEED_COUNT / 2;
        }
        else if(wtModerate == DWindType){
            ChangeProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.025));
            DirectionProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.025));
            SpeedProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.2));
            MinWindSpeed = 2;
            MaxWindSpeed = WINDSPEED_COUNT - 1;
        }
        else{
            ChangeProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.1));
            DirectionProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.1));
            SpeedProbability = ((unsigned int)(RANDOM_NUMBER_MAX * 0.3));
            MinWindSpeed = WINDSPEED_COUNT / 2;
            MaxWindSpeed = WINDSPEED_COUNT - 1;
        }
        
        if((DRandomNumberGenerator.Random() % RANDOM_NUMBER_MAX) < ChangeProbability){
            unsigned int Dirchange = DRandomNumberGenerator.Random() % RANDOM_NUMBER_MAX;
            unsigned int SpeedChange = DRandomNumberGenerator.Random() % RANDOM_NUMBER_MAX;
            
            if(Dirchange < DirectionProbability){
                DWindDirection += WINDDIRECTION_COUNT-1;
                DWindDirection %= WINDDIRECTION_COUNT;
                if(!DWindDirection){
                    DWindDirection = WINDDIRECTION_COUNT-1;
                }
                WindDirectionChange = true;
            }
            else if(Dirchange < DirectionProbability * 2){
                DWindDirection++;
                DWindDirection %= WINDDIRECTION_COUNT; 
                if(!DWindDirection){
                    DWindDirection = 1;
                }
                WindDirectionChange = true;
            }
            if(SpeedChange < DirectionProbability){
                DWindSpeed += WINDSPEED_COUNT-1;
                DWindSpeed %= WINDSPEED_COUNT;
            }
            else if(SpeedChange < DirectionProbability * 2){
                DWindSpeed++;
                DWindSpeed %= WINDSPEED_COUNT;            
            }

        }
        if(!DWindDirection){
            DWindDirection = 1;
            WindDirectionChange = true;
        }
        if(DWindSpeed < MinWindSpeed){
            DWindSpeed = MinWindSpeed;
        }
        if(DWindSpeed > MaxWindSpeed){
            DWindSpeed = MaxWindSpeed;
        }
    }
    if(WindDirectionChange){
        for(int Index = 0; Index < D3DTerrainPixmaps.size(); Index++){
            DTerrainMap.Draw3DMap(D3DTerrainPixmaps[Index], DDrawingContext, DWindDirection, ANIMATION_TIMESTEPS, Index);
        }
    }
}

void CApplicationData::ChangeMode(EGameMode nextmode){
    if(gmMainMenu == nextmode){
        DMenuTitle = "THE GAME";
        DMenuItems.clear();
        DMenuItems.push_back("SINGLE PLAYER");
        DMenuItems.push_back("MULTIPLAYER");
        DMenuItems.push_back("OPTIONS");
        DMenuItems.push_back("EXIT");
        DLastSelectedMenuItem = -1;
        DSoundMixer.PlaySong(DSongIndices[stMenu], DMusicVolume);
    }
    else if(gmOptionsMenu == nextmode){
        DMenuTitle = "OPTIONS";
        DMenuItems.clear();
        DMenuItems.push_back("SOUND");
        DMenuItems.push_back("NETWORK");
        DMenuItems.push_back("BACK");
        DLastSelectedMenuItem = -1;   
    }
    else if(gmBattle == nextmode){
        DVoiceClipID = -1;
        InitializeBanner("PREPARE FOR BATTLE");
        nextmode = gmTransitionBattle;
    }
    else if(gmSelectCastle == nextmode){
        InitializeBanner("SELECT CASTLE");
        nextmode = gmTransitionSelectCastle;
        DSoundMixer.StopSong();
    }
    else if(gmRebuild == nextmode){
        InitializeBanner("REBUILD WALLS TO STAY ALIVE");
        nextmode = gmTransitionRebuild;   
    }
    else if(gmCannonPlacement == nextmode){
        InitializeBanner("PLACE CANNONS");
        nextmode = gmTransitionCannonPlacement;
    }
    DNextGameMode = nextmode;
}

void CApplicationData::InitializeBanner(const std::string &message){
    gint BannerWidth, BannerHeight;
    
    DrawBanner(message);
    gdk_pixmap_get_size(DBannerPixmap, &BannerWidth, &BannerHeight); 
    DBannerLocation = -BannerHeight;
}

#define MAX_DELTAMOVE   3

void CApplicationData::MoveAI(int colorindex){
    int DeltaX, DeltaY;
    
    DeltaX = DAITargetX[colorindex] - DCurrentX[colorindex];
    DeltaY = DAITargetY[colorindex] - DCurrentY[colorindex];
    if(-MAX_DELTAMOVE > DeltaX){
        DeltaX = -MAX_DELTAMOVE;  
    }
    if(MAX_DELTAMOVE < DeltaX){
        DeltaX = MAX_DELTAMOVE;  
    }
    if(-MAX_DELTAMOVE > DeltaY){
        DeltaY = -MAX_DELTAMOVE;  
    }
    if(MAX_DELTAMOVE < DeltaY){
        DeltaY = MAX_DELTAMOVE;  
    }
    DCurrentX[colorindex] += DeltaX;
    DCurrentY[colorindex] += DeltaY;
}

void CApplicationData::SelectCastleAI(){
    int SecondsLeft = SecondsUntilDeadline(DCurrentStageTimeout);
    int IncompleteCount = 0;
    
    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        if(!DCompletedStage[ColorIndex]){
            IncompleteCount++;
        }    
    }
    
    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        if(DPlayerIsAI[ColorIndex]){
            if(!DCompletedStage[ColorIndex]){
                if(0 > DAITargetX[ColorIndex]){
                    int TargetCastleIndex = ColorIndex % DCastleLocations[ColorIndex].size();
                    
                    DAITargetX[ColorIndex] = (DCastleLocations[ColorIndex][TargetCastleIndex].DXIndex + 1) * DTileWidth;
                    DAITargetY[ColorIndex] = (DCastleLocations[ColorIndex][TargetCastleIndex].DYIndex + 1) * DTileHeight;
                }
                MoveAI(ColorIndex);
                
                if((20 > SecondsLeft)||(1 == IncompleteCount)||((DAITargetX[ColorIndex] == DCurrentX[ColorIndex])&&(DAITargetY[ColorIndex] == DCurrentY[ColorIndex]))){
                    DLeftClick[ColorIndex] = true;
                }
            }
        }
    }
}

void CApplicationData::CannonPlacementAI(){
    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        if(DPlayerIsAI[ColorIndex]){
            if(!DCompletedStage[ColorIndex]){
                if(0 > DAITargetX[ColorIndex]){
                    int XTile, YTile;
                    bool XDir, YDir;
                    XDir = DCannonsToPlace[ColorIndex] & 0x1;
                    YDir = DCannonsToPlace[ColorIndex] & 0x2;
                    XTile = DCurrentX[ColorIndex] / DTileWidth;
                    YTile = DCurrentY[ColorIndex] / DTileHeight;
                    BoundsCheck(XTile, YTile);
                    for(int YOff = 0; YOff < DMapHeight; YOff++){
                        for(int XOff = 0; XOff < DMapWidth; XOff++){
                            int AttemptX, AttemptY;
                            
                            if(XDir){
                                AttemptX = XTile + XOff;
                            }
                            else{
                                AttemptX = DMapWidth + XTile - XOff;
                            }
                            if(YDir){
                                AttemptY = YTile + YOff;
                            }
                            else{
                                AttemptY = DMapHeight + YTile - YOff;
                            }
                            AttemptX %= DMapWidth;
                            AttemptY %= DMapHeight;
                            if(ValidCannonPlacement(ColorIndex, AttemptX, AttemptY)){
                                DAITargetX[ColorIndex] = AttemptX * DTileWidth + DTileWidth / 2;
                                DAITargetY[ColorIndex] = AttemptY * DTileHeight + DTileHeight / 2;
                                break;
                            }
                        }
                        if(0 <= DAITargetX[ColorIndex]){
                            break;
                        }
                    }
                }
                MoveAI(ColorIndex);
                if((DAITargetX[ColorIndex] == DCurrentX[ColorIndex])&&(DAITargetY[ColorIndex] == DCurrentY[ColorIndex])){
                    DLeftClick[ColorIndex] = true;
                    DAITargetX[ColorIndex] = -1;
                    DAITargetY[ColorIndex] = -1;
                }
            }
        }
    }
}

void CApplicationData::RebuildAI(){
    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        if(DPlayerIsAI[ColorIndex]){
            bool DoMove = true;
            if((0 > DAITargetCastle[ColorIndex])||(DSurroundedCastles[ColorIndex][DAITargetCastle[ColorIndex]])){
                int TargetCastle = 0;
                int BestCastle = -1;
                int BestCannons = -1;

                while(TargetCastle < DSurroundedCastles[ColorIndex].size()){
                    if(!DSurroundedCastles[ColorIndex][TargetCastle]){
                        int CannonCount = 0;
                        std::vector< SMapLocation >::iterator Iterator;
    
                        Iterator = DCannonLocations[ColorIndex].begin();
                        while(DCannonLocations[ColorIndex].end() != Iterator){
                            int DeltaX = DCastleLocations[ColorIndex][TargetCastle].DXIndex - Iterator->DXIndex;
                            int DeltaY = DCastleLocations[ColorIndex][TargetCastle].DYIndex - Iterator->DYIndex;
                            
                            if((-2 <= DeltaX)&&(2 >= DeltaX)&&(-2 <= DeltaY)&&(2 >= DeltaY)){
                                CannonCount++;   
                            }
                            Iterator++;
                        }
                        if(BestCannons < CannonCount){
                            BestCastle = TargetCastle;   
                        }
                    }
                    TargetCastle++;
                }
                DAITargetCastle[ColorIndex] = BestCastle;
                DAITargetX[ColorIndex] = -1;
                DAITargetY[ColorIndex] = -1;
            }
            
            if(0 > DAITargetX[ColorIndex]){
                int XTile, YTile;
                int BestX, BestY, BestRotation, BestCoverage, BestInterference;

                if(0 > DAITargetCastle[ColorIndex]){
                    DAITargetX[ColorIndex] = DCastleLocations[ColorIndex][0].DXIndex;
                    DAITargetY[ColorIndex] = DCastleLocations[ColorIndex][0].DYIndex;
                    DAITargetX[ColorIndex] *= DTileWidth;
                    DAITargetX[ColorIndex] += DTileWidth / 2;
                    DAITargetY[ColorIndex] *= DTileHeight;
                    DAITargetY[ColorIndex] += DTileHeight / 2;
                }
                else{
                    XTile = DCastleLocations[ColorIndex][DAITargetCastle[ColorIndex]].DXIndex;
                    YTile = DCastleLocations[ColorIndex][DAITargetCastle[ColorIndex]].DYIndex;
                    
                    BestInterference = 0;
                    BestCoverage = 0;
                    BestX = -1;
                    BestY = -1;
                    for(int RotationIndex = 0; RotationIndex < 4; RotationIndex++){
                        for(int YOffset = -2 - DWallShape[ColorIndex].Height(); YOffset < 5; YOffset++){
                            int TargetY = YTile + YOffset;
                            
                            if(0 > TargetY){
                                continue;    
                            }
                            if(TargetY >= DMapHeight){
                                continue;
                            }
                            for(int XOffset = -2 - DWallShape[ColorIndex].Width(); XOffset < 5; XOffset++){
                                int TargetX = XTile + XOffset;
                                if(0 > TargetX){
                                    continue;    
                                }
                                if(TargetX >= DMapWidth){
                                    continue;
                                }
                                if(ValidWallPlacement(ColorIndex, TargetX, TargetY)){
                                    int Coverage = 0, Interference = 0;
                                    bool BetterSolution = false;
                                    
                                    for(int WallYPos = 0; WallYPos < DWallShape[ColorIndex].Height(); WallYPos++){
                                        for(int WallXPos = 0; WallXPos < DWallShape[ColorIndex].Width(); WallXPos++){
                                            if(DWallShape[ColorIndex].IsBlock(WallXPos, WallYPos)){
                                                int WallX = TargetX + WallXPos - XTile;
                                                int WallY = TargetY + WallYPos - YTile;
                                                
                                                if(((-3 == WallX)||(4 == WallX))&&((-2 <= WallY)&&(3 >= WallY))){
                                                    Coverage++;  
                                                }
                                                else if(((-3 == WallY)||(4 == WallY))&&((-3 <= WallX)&&(4 >= WallX))){
                                                    Coverage++;
                                                }
                                                else if(((-2 <= WallX)&&(3 >= WallX))&&((-2 <= WallY)&&(3 >= WallY))){
                                                    Interference++;   
                                                }
                                            }
                                        }
                                    }
                                    
                                    if(0 > BestX){
                                        BetterSolution = true;
                                    }
                                    if(Coverage > BestCoverage){
                                        BetterSolution = true;
                                    }
                                    if((Coverage == BestCoverage)&&(Interference < BestInterference)){
                                        BetterSolution = true;
                                    }
                                    if(BetterSolution){
                                        BestX = TargetX;
                                        BestY = TargetY;
                                        BestRotation = RotationIndex;
                                        BestCoverage = Coverage;
                                        BestInterference = Interference;
                                    }
                                }
                            }
                        }
                        DWallShape[ColorIndex].Rotate();
                    }
                    if(0 > BestX){
                        DoMove = false;
                    }
                    else if(BestRotation){
                        DRightClick[ColorIndex] = true;
                        DoMove = false;
                    }
                    else{
                        DAITargetX[ColorIndex] = BestX;
                        DAITargetY[ColorIndex] = BestY;
                        DAITargetX[ColorIndex] *= DTileWidth;
                        DAITargetX[ColorIndex] += DTileWidth / 2;
                        DAITargetY[ColorIndex] *= DTileHeight;
                        DAITargetY[ColorIndex] += DTileHeight / 2;
                    }
                }
            }
            if(DoMove){
                MoveAI(ColorIndex);
            }
            if(!DRightClick[ColorIndex]&&(DAITargetX[ColorIndex] == DCurrentX[ColorIndex])&&(DAITargetY[ColorIndex] == DCurrentY[ColorIndex])){
                DLeftClick[ColorIndex] = true;
                DAITargetX[ColorIndex] = -1;
                DAITargetY[ColorIndex] = -1;
            }
        }
    }
}

void CApplicationData::BattleAI(){
    for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
        if(DPlayerIsAI[ColorIndex]){
            if(0 > DAITargetX[ColorIndex]){
                DAITargetX[ColorIndex] = DRandomNumberGenerator.Random() % (DCanvasWidth);
                DAITargetY[ColorIndex] = DRandomNumberGenerator.Random() % (DCanvasHeight);
            }
            MoveAI(ColorIndex);
            if((DAITargetX[ColorIndex] == DCurrentX[ColorIndex])&&(DAITargetY[ColorIndex] == DCurrentY[ColorIndex])){
                DLeftClick[ColorIndex] = true;
                DAITargetX[ColorIndex] = -1;
                DAITargetY[ColorIndex] = -1;
            }
        }
    }
}

void CApplicationData::CheckSurroundedCastles(){
    // Check surrounding
    for(int YPos = 0; YPos < DMapHeight; YPos++){
        for(int XPos = 0; XPos < DMapWidth; XPos++){
            if(!TileTypeIsWall(DConstructionTiles[YPos][XPos])){
                if(TileTypeIsFloorGroundDamaged(DConstructionTiles[YPos][XPos])){
                    switch(DTerrainMap.TileType(XPos, YPos)){
                        case pcBlue:    DConstructionTiles[YPos][XPos] = cttBlueFloorDamaged;
                                        break;
                        case pcRed:     DConstructionTiles[YPos][XPos] = cttRedFloorDamaged;
                                        break;
                        case pcYellow:  DConstructionTiles[YPos][XPos] = cttYellowFloorDamaged;
                                        break;
                        default:        DConstructionTiles[YPos][XPos] = cttNone;
                                        break;
                    }
                }
                else{
                    switch(DTerrainMap.TileType(XPos, YPos)){
                        case pcBlue:    DConstructionTiles[YPos][XPos] = cttBlueFloor;
                                        break;
                        case pcRed:     DConstructionTiles[YPos][XPos] = cttRedFloor;
                                        break;
                        case pcYellow:  DConstructionTiles[YPos][XPos] = cttYellowFloor;
                                        break;
                        default:        DConstructionTiles[YPos][XPos] = cttNone;
                                        break;
                    }
                }
            }
        }
    }
    for(int YPos = 0; YPos < DMapHeight; YPos++){
        for(int XPos = 0; XPos < DMapWidth; XPos++){
            if(!TileTypeIsWall(DConstructionTiles[YPos][XPos])){
                if(0 == XPos){
                    ExpandUnclaimed(XPos, YPos);
                }
                else if(0 == YPos){
                    ExpandUnclaimed(XPos, YPos); 
                }
                else if(XPos + 1 == DMapWidth){
                    ExpandUnclaimed(XPos, YPos);   
                }
                else if(YPos + 1 == DMapHeight){
                    ExpandUnclaimed(XPos, YPos);    
                }
                else if(cttNone == DConstructionTiles[YPos][XPos]){
                    ExpandUnclaimed(XPos, YPos);    
                }
            }
        }
    }
    for(int ColorIndex = pcBlue; ColorIndex < pcMax; ColorIndex++){
        for(int Index = 0; Index < DSurroundedCastles[ColorIndex].size(); Index++){
            DSurroundedCastles[ColorIndex][Index] = TileTypeIsFloor(DConstructionTiles[DCastleLocations[ColorIndex][Index].DYIndex][DCastleLocations[ColorIndex][Index].DXIndex]);
        }
    }
}

bool CApplicationData::CannonCastleInterfere(int xindex, int yindex, int width, int height){
    std::vector< SMapLocation >::iterator Iterator;
    
    if((2 == width)&&(2 == height)){
        for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
            Iterator = DCannonLocations[ColorIndex].begin();
            while(DCannonLocations[ColorIndex].end() != Iterator){
                int XDelta, YDelta;
                XDelta = Iterator->DXIndex - xindex;
                XDelta = 0 > XDelta ? -XDelta : XDelta;
                YDelta = Iterator->DYIndex - yindex;
                YDelta = 0 > YDelta ? -YDelta : YDelta;
                if((XDelta < width)&&(YDelta < height)){
                    return true;
                }
                Iterator++;
            }
        
            Iterator = DCastleLocations[ColorIndex].begin();
            while(DCastleLocations[ColorIndex].end() != Iterator){
                int XDelta, YDelta;
                XDelta = Iterator->DXIndex - xindex;
                XDelta = 0 > XDelta ? -XDelta : XDelta;
                YDelta = Iterator->DYIndex - yindex;
                YDelta = 0 > YDelta ? -YDelta : YDelta;
                if((XDelta < width)&&(YDelta < height)){
                    return true;
                }
                Iterator++;
            }
        }
    }
    else{
        for(int ColorIndex = pcBlue; ColorIndex < pcBlue + DPlayerCount; ColorIndex++){
            Iterator = DCannonLocations[ColorIndex].begin();
            while(DCannonLocations[ColorIndex].end() != Iterator){
                if((xindex == Iterator->DXIndex)||(xindex == Iterator->DXIndex + 1)){
                    if((yindex == Iterator->DYIndex)||(yindex == Iterator->DYIndex + 1)){
                        return true;
                    }
                }
                Iterator++;
            }
            
            Iterator = DCastleLocations[ColorIndex].begin();
            while(DCastleLocations[ColorIndex].end() != Iterator){
                if((xindex == Iterator->DXIndex)||(xindex == Iterator->DXIndex + 1)){
                    if((yindex == Iterator->DYIndex)||(yindex == Iterator->DYIndex + 1)){
                        return true;
                    }
                }
                Iterator++;
            }
        }
    }
            
    return false;
}

bool CApplicationData::ValidCannonPlacement(int colorindex, int xindex, int yindex){
    EConstructionTileType ConTileType;
    
    if(pcBlue == colorindex){
        ConTileType = cttBlueFloor;
    }
    else if(pcRed == colorindex){
        ConTileType = cttRedFloor;
    }
    else{
        ConTileType = cttYellowFloor;
    }
    if(xindex + 1 >= DMapWidth){
        return false;
    }
    if(yindex + 1 >= DMapHeight){
        return false;
    }
    for(int YOff = 0; YOff < 2; YOff++){
        for(int XOff = 0; XOff < 2; XOff++){
            if(ConTileType != DConstructionTiles[yindex + YOff][xindex + XOff]){
                return false;
            }
        }
    }
    return !CannonCastleInterfere(xindex, yindex, 2, 2);   
}

bool CApplicationData::ValidWallPlacement(int colorindex, int xindex, int yindex){
    int XPos, YPos;
    for(int WallYPos = 0; WallYPos < DWallShape[colorindex].Height(); WallYPos++){
        YPos = yindex + WallYPos;
        if(YPos >= DMapHeight){
            return false;   
        }
        for(int WallXPos = 0; WallXPos < DWallShape[colorindex].Width(); WallXPos++){
            XPos = xindex + WallXPos;
            if(XPos >= DMapWidth){
                return false;   
            }
            if(DWallShape[colorindex].IsBlock(WallXPos, WallYPos)){
                if(colorindex != DTerrainMap.TileType(XPos, YPos)){
                    return false;
                }
                if(TileTypeIsWall(DConstructionTiles[YPos][XPos])){
                    return false;
                }
                if(TileTypeIsFloor(DConstructionTiles[YPos][XPos])){
                    return false;
                }
                if(TileTypeIsFloorGroundDamaged(DConstructionTiles[YPos][XPos])){
                    return false;   
                }
                if(CannonCastleInterfere(XPos, YPos, 1, 1)){
                    return false;
                }
            }
        }
    }
    return true;
}

void CApplicationData::ExpandUnclaimed(int xpos, int ypos){
    bool NValid, EValid, SValid, WValid;
    
    if(TileTypeIsFloorGroundDamaged(DConstructionTiles[ypos][xpos])){
        DConstructionTiles[ypos][xpos] = cttGroundDamaged;
    }
    else{
        DConstructionTiles[ypos][xpos] = cttNone;
    }
    NValid = 0 < ypos;
    WValid = 0 < xpos;
    SValid = ypos + 1 < DMapHeight;
    EValid = xpos + 1 < DMapWidth;
    
    if(NValid){
        if(WValid){
            if(TileTypeIsFloor(DConstructionTiles[ypos-1][xpos-1])||TileTypeIsFloorDamaged(DConstructionTiles[ypos-1][xpos-1])){
                ExpandUnclaimed(xpos-1, ypos-1);
            }
        }
        if(TileTypeIsFloor(DConstructionTiles[ypos-1][xpos])||TileTypeIsFloorDamaged(DConstructionTiles[ypos-1][xpos])){
            ExpandUnclaimed(xpos, ypos-1);
        }
        if(EValid){
            if(TileTypeIsFloor(DConstructionTiles[ypos-1][xpos+1])||TileTypeIsFloorDamaged(DConstructionTiles[ypos-1][xpos+1])){
                ExpandUnclaimed(xpos+1, ypos-1);
            }
        }
    }
    if(WValid){
        if(TileTypeIsFloor(DConstructionTiles[ypos][xpos-1])||TileTypeIsFloorDamaged(DConstructionTiles[ypos][xpos-1])){
            ExpandUnclaimed(xpos-1, ypos);
        }
    }
    if(EValid){
        if(TileTypeIsFloor(DConstructionTiles[ypos][xpos+1])||TileTypeIsFloorDamaged(DConstructionTiles[ypos][xpos+1])){
            ExpandUnclaimed(xpos+1, ypos);
        }
    }
    if(SValid){
        if(WValid){
            if(TileTypeIsFloor(DConstructionTiles[ypos+1][xpos-1])||TileTypeIsFloorDamaged(DConstructionTiles[ypos+1][xpos-1])){
                ExpandUnclaimed(xpos-1, ypos+1);
            }
        }
        if(TileTypeIsFloor(DConstructionTiles[ypos+1][xpos])||TileTypeIsFloorDamaged(DConstructionTiles[ypos+1][xpos])){
            ExpandUnclaimed(xpos, ypos+1);
        }
        if(EValid){
            if(TileTypeIsFloor(DConstructionTiles[ypos+1][xpos+1])||TileTypeIsFloorDamaged(DConstructionTiles[ypos+1][xpos+1])){
                ExpandUnclaimed(xpos+1, ypos+1);
            }
        }
    }

}

bool CApplicationData::CleanUpWallEdges(){
    bool WallsRemoved = false;
    
    for(int YPos = 1; YPos + 1< DMapHeight; YPos++){
        for(int XPos = 1; XPos + 1 < DMapWidth; XPos++){
            if(TileTypeIsWall(DConstructionTiles[YPos][XPos])){
                int WallCount = 0, FloorCount = 0;
                bool RemoveWall = false;
                for(int YOff = -1; YOff < 2; YOff++){
                    for(int XOff = -1; XOff < 2; XOff++){
                        if(TileTypeIsWall(DConstructionTiles[YPos + YOff][XPos + XOff])){
                            WallCount++;   
                        }
                        else if(TileTypeIsFloor(DConstructionTiles[YPos + YOff][XPos + XOff])){
                            FloorCount++;   
                        }
                    }
                }
                if((1 == WallCount)&&(8 == FloorCount)){
                    RemoveWall = true;    
                }
                if((2 == WallCount)&&(7 == FloorCount)){
                    RemoveWall = true;    
                }
                if((3 == WallCount)&&(6 == FloorCount)){
                    if(!(TileTypeIsWall(DConstructionTiles[YPos - 1][XPos]) && TileTypeIsWall(DConstructionTiles[YPos + 1][XPos])) && !(TileTypeIsWall(DConstructionTiles[YPos][XPos - 1]) && TileTypeIsWall(DConstructionTiles[YPos][XPos + 1]))){
                        RemoveWall = true;    
                    }
                }
                if((4 == WallCount)&&(5 == FloorCount)){
                    if(!(TileTypeIsWall(DConstructionTiles[YPos - 1][XPos]) && TileTypeIsWall(DConstructionTiles[YPos + 1][XPos])) && !(TileTypeIsWall(DConstructionTiles[YPos][XPos - 1]) && TileTypeIsWall(DConstructionTiles[YPos][XPos + 1]))){
                        RemoveWall = true;    
                    }                    
                }
                
                if(RemoveWall){
                    WallsRemoved = true;
                    switch(DTerrainMap.TileType(XPos, YPos)){
                        case pcBlue:    DConstructionTiles[YPos][XPos] = cttBlueFloor;
                                        break;
                        case pcRed:     DConstructionTiles[YPos][XPos] = cttRedFloor;
                                        break;
                        case pcYellow:  DConstructionTiles[YPos][XPos] = cttYellowFloor;
                                        break;
                        default:        DConstructionTiles[YPos][XPos] = cttNone;
                                        break;
                    }
                }
            }
        }
    }
    return WallsRemoved;
}

bool CApplicationData::BoundsCheck(int &xindex, int &yindex){
    bool ValidBounds = true;
    if(0 > xindex){
        xindex = 0; 
        ValidBounds = false;
    }
    if(DMapWidth <= xindex){
        xindex = DMapWidth - 1;
        ValidBounds = false;
    }
    if(0 > yindex){
        yindex = 0;
        ValidBounds = false;
    }
    if(DMapHeight <= yindex){
        yindex = DMapHeight - 1;
        ValidBounds = false;
    }
    return ValidBounds;
}

void CApplicationData::PlayTickTockSound(){
    int MSUntilDeadline = MiliSecondsUntilDeadline(DCurrentStageTimeout);
    int LastTickTockMS = -MiliSecondsUntilDeadline(DLastTickTockTime);
    bool PlaySound = false;
    bool PlayTick = true;
    
    if(5000 <= MSUntilDeadline){
        if((MSUntilDeadline/1000) != ((MSUntilDeadline + LastTickTockMS)/1000)){
            PlaySound = true;
            PlayTick = (MSUntilDeadline/1000) & 0x01;
        }
    }
    else if(0 <= MSUntilDeadline){
        if((5000 < MSUntilDeadline + LastTickTockMS)||((MSUntilDeadline/500) != ((MSUntilDeadline + LastTickTockMS)/500))){
            PlaySound = true;
            PlayTick = (MSUntilDeadline/500) & 0x01;
        }
    }
    else{
        if((0 < MSUntilDeadline + LastTickTockMS) ||((MSUntilDeadline/250) != ((MSUntilDeadline + LastTickTockMS)/250))){
            PlaySound = true;
            PlayTick = (MSUntilDeadline/500) & 0x01;
        }
    }
    if(PlaySound){
        DSoundMixer.PlayClip(DSoundClipIndices[PlayTick ? sctTick : sctTock], DSoundEffectVolume, 0.0);
        gettimeofday(&DLastTickTockTime, NULL);
    }    
}

int CApplicationData::DetermineConquered(){
    int LivingPlayers = 0;
    
    for(int ColorIndex = 0; ColorIndex < pcMax; ColorIndex++){
        std::vector< bool >::iterator SurroundedIterator;
        int CurrentCastles = 0;
        
        SurroundedIterator = DSurroundedCastles[ColorIndex].begin();
        while(DSurroundedCastles[ColorIndex].end() != SurroundedIterator){
            if(*SurroundedIterator){
                CurrentCastles++;
            }
            SurroundedIterator++;
        }
        DSurroundedCastleCounts[ColorIndex] = CurrentCastles;
        if(CurrentCastles){
            DPlayerIsConquered[ColorIndex] = false;
            LivingPlayers++;
        }
        else{
            DPlayerIsConquered[ColorIndex] = true;       
        }
    }
    return LivingPlayers;
}

void CApplicationData::ResetMap(){
    for(int Index = 0; Index < DConstructionTiles.size(); Index++){
        std::vector< EConstructionTileType >::iterator Iterator;
        std::vector< int >::iterator HitIterator;
        
        DConstructionTiles[Index].resize(DMapWidth);
        Iterator = DConstructionTiles[Index].begin();
        while(DConstructionTiles[Index].end() != Iterator){
            *Iterator = cttNone;
            Iterator++;
        }
        
        DHitsTaken[Index].resize(DMapWidth);
        HitIterator = DHitsTaken[Index].begin();
        while(DHitsTaken[Index].end() != HitIterator){
            *HitIterator = 0;
            HitIterator++;
        }
    }
    for(int ColorIndex = 0; ColorIndex < pcMax; ColorIndex++){
        DCannonLocations[ColorIndex].clear();
        DPlayerIsAI[ColorIndex] = true;   
        DAITargetX[ColorIndex] = -1;
        DAITargetY[ColorIndex] = -1;
        
        for(std::vector< bool >::iterator SurroundedIterator = DSurroundedCastles[ColorIndex].begin(); DSurroundedCastles[ColorIndex].end() != SurroundedIterator; SurroundedIterator++){
            *SurroundedIterator = false;
        }        
    }
    DPlayerIsAI[DPlayerColor] = false;
    
    DNextGameMode = gmSelectCastle;
    for(int Index = 0; Index < pcMax; Index++){
        DCompletedStage[Index] = false;   
    }
    gettimeofday(&DCurrentStageTimeout,NULL);
    DCurrentStageTimeout.tv_sec += SELECT_TIME;
}

void CApplicationData::LoadTerrainMap(int index){

    if((0 > index)||(DTerrainMaps.size() <= index)){
        return;   
    }
    DTerrainMap = DTerrainMaps[index];

    DMapWidth = DTerrainMap.Width();
    DMapHeight = DTerrainMap.Height();
    DTileWidth = D2DTerrainTileset.TileWidth();
    DTileHeight = D2DTerrainTileset.TileHeight();
    DCanvasWidth = DMapWidth * DTileWidth;
    DCanvasHeight = DMapHeight * DTileHeight;
    
    ResizeCanvases();

    
    DInitialVelocities.resize(IntegerSquareRoot(DCanvasWidth * DCanvasWidth + DCanvasHeight * DCanvasHeight) + 1);
    for(int Index = 0; Index < DInitialVelocities.size(); Index++){
        DInitialVelocities[Index] = sqrt(STANDARD_GRAVITY * Index / 2);
    }

    DPlayerCount = DTerrainMap.PlayerCount();
    for(int Index = 0; Index < pcMax; Index++){
        DPlayerIsAlive[Index] = Index < DPlayerCount;   
        DSurroundedCastles[Index].clear();
        DCastleLocations[Index].clear();
    }
    for(int Index = 0; Index < DTerrainMap.CastleCount(); Index++){
        SMapLocation TempLocation;
            
        DTerrainMap.CastleLocation(Index, TempLocation.DXIndex, TempLocation.DYIndex);
        DSurroundedCastles[DTerrainMap.TileType(TempLocation.DXIndex, TempLocation.DYIndex)].push_back(false);
        DCastleLocations[DTerrainMap.TileType(TempLocation.DXIndex, TempLocation.DYIndex)].push_back(TempLocation);
    }
    DConstructionTiles.resize(DMapHeight);
    DHitsTaken.resize(DMapHeight);

    ResetMap();
    
    DTerrainMap.Draw2DMap(D2DTerrainPixmap, DDrawingContext);
    for(int Index = 0; Index < D3DTerrainPixmaps.size(); Index++){
        DTerrainMap.Draw3DMap(D3DTerrainPixmaps[Index], DDrawingContext, 0, ANIMATION_TIMESTEPS, Index);
    }
}

void CApplicationData::ResizeCanvases(){
    // Resize the canvas
    gtk_drawing_area_size(GTK_DRAWING_AREA(DDrawingArea), DCanvasWidth * DScaling, DCanvasHeight * DScaling);
    
    if(NULL != DDoubleBufferPixmap){
        gint CurWidth, CurHeight;
        
        gdk_pixmap_get_size(DDoubleBufferPixmap, &CurWidth, &CurHeight); 
        if((DCanvasWidth * DScaling != CurWidth)||(DCanvasHeight * DScaling != CurHeight)){
            g_object_unref(DDoubleBufferPixmap);
            DDoubleBufferPixmap = NULL;
        }
    }
    if(NULL == DDoubleBufferPixmap){
        DDoubleBufferPixmap = gdk_pixmap_new(DDrawingArea->window, DCanvasWidth * DScaling, DCanvasHeight * DScaling, -1);
    }
    if(NULL != DWorkingBufferPixmap){
        gint CurWidth, CurHeight;
        
        gdk_pixmap_get_size(DWorkingBufferPixmap, &CurWidth, &CurHeight); 
        if((DCanvasWidth != CurWidth)||(DCanvasHeight != CurHeight)){
            g_object_unref(DWorkingBufferPixmap);
            DWorkingBufferPixmap = NULL;
        }
    }
    if(NULL == DWorkingBufferPixmap){
        DWorkingBufferPixmap = gdk_pixmap_new(DDrawingArea->window, DCanvasWidth, DCanvasHeight, -1);
    }
    if(NULL != DPreviousWorkingBufferPixmap){
        gint CurWidth, CurHeight;
        
        gdk_pixmap_get_size(DPreviousWorkingBufferPixmap, &CurWidth, &CurHeight); 
        if((DCanvasWidth != CurWidth)||(DCanvasHeight != CurHeight)){
            g_object_unref(DPreviousWorkingBufferPixmap);
            DPreviousWorkingBufferPixmap = NULL;
        }
    }
    if(NULL == DPreviousWorkingBufferPixmap){
        DPreviousWorkingBufferPixmap = gdk_pixmap_new(DDrawingArea->window, DCanvasWidth, DCanvasHeight, -1);
    }
    if(NULL != DWorkingPixbuf){
        g_object_unref(DWorkingPixbuf);
        DWorkingPixbuf = NULL;
    }
    if(NULL != DBannerPixmap){
         g_object_unref(DBannerPixmap);
         DBannerPixmap = NULL;
    }
    if(NULL != DMessagePixmap){
         g_object_unref(DMessagePixmap);
         DMessagePixmap = NULL;
    }
    if(NULL != D2DTerrainPixmap){
        gint CurWidth, CurHeight;
        
        gdk_pixmap_get_size(D2DTerrainPixmap, &CurWidth, &CurHeight); 
        if((DCanvasWidth != CurWidth)||(DCanvasHeight != CurHeight)){
            g_object_unref(D2DTerrainPixmap);
            D2DTerrainPixmap = NULL;
        }
    }
    if(NULL == D2DTerrainPixmap){
        D2DTerrainPixmap = gdk_pixmap_new(DDrawingArea->window, DCanvasWidth, DCanvasHeight, -1);
    }
    while(D3DTerrainPixmaps.size() < ANIMATION_TIMESTEPS){
        D3DTerrainPixmaps.push_back(NULL);
    }
    for(int Index = 0; Index < D3DTerrainPixmaps.size(); Index++){
        if(NULL != D3DTerrainPixmaps[Index]){
            gint CurWidth, CurHeight;
            
            gdk_pixmap_get_size(D3DTerrainPixmaps[Index], &CurWidth, &CurHeight); 
            if((DCanvasWidth != CurWidth)||(DCanvasHeight != CurHeight)){
                g_object_unref(D3DTerrainPixmaps[Index]);
                D3DTerrainPixmaps[Index] = NULL;
            }
        }
        if(NULL == D3DTerrainPixmaps[Index]){
            D3DTerrainPixmaps[Index] = gdk_pixmap_new(DDrawingArea->window, DCanvasWidth, DCanvasHeight, -1);
        }   
    }
}

int CApplicationData::Init(int argc, char *argv[]){
    DIR *MapDirectory;
    struct dirent *DirectoryEntry;
    std::string MapPath = "maps";
    // This is called in all GTK applications. Arguments are parsed from the 
    // command line and are returned to the application. All GTK+ specific 
    // arguments are removed from the argc/argv list.
    gtk_init(&argc, &argv);
    
    
    // Create a new main window 
    DMainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    // When the window is given the "delete-event" signal (this is given by the 
    // window manager, usually by the "close" option, or on the titlebar), we 
    // ask it to call the delete_event () function as defined above. The data 
    // passed to the callback function is NULL and is ignored in the callback 
    // function. 
    g_signal_connect(DMainWindow, "delete-event", G_CALLBACK(MainWindowDeleteEventCallback), this);
    
    // Here we connect the "destroy" event to a signal handler. This event 
    // occurs when we call gtk_widget_destroy() on the window, or if we return 
    // FALSE in the "delete-event" callback. 
    g_signal_connect(DMainWindow, "destroy", G_CALLBACK(MainWindowDestroyCallback), this);
    
    g_signal_connect(DMainWindow, "key-press-event", G_CALLBACK(MainWindowKeyPressEventCallback), this);
    
    
    // Sets the border width of the window. 
    gtk_container_set_border_width(GTK_CONTAINER(DMainWindow), 10);
    
    // Creates a drawing surface
    DDrawingArea = gtk_drawing_area_new();

    gtk_drawing_area_size(GTK_DRAWING_AREA(DDrawingArea), INITIAL_MAP_WIDTH, INITIAL_MAP_HEIGHT);
    
    // Add drawing surface to main window
    gtk_container_add(GTK_CONTAINER(DMainWindow), DDrawingArea);
    
    gtk_signal_connect(GTK_OBJECT(DDrawingArea), "expose_event", G_CALLBACK(DrawingAreaExposeCallback), this);
    gtk_signal_connect(GTK_OBJECT(DDrawingArea), "button_press_event", G_CALLBACK(DrawingAreaButtonPressEventCallback), this);
    gtk_signal_connect(GTK_OBJECT(DDrawingArea), "motion_notify_event", G_CALLBACK(DrawingAreaMotionNotifyEventCallback), this);
    
    gtk_widget_set_events(DDrawingArea, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);
    
    DBlankCursor = gdk_cursor_new(GDK_BLANK_CURSOR);
    // Show all widgets so they are displayed
    gtk_widget_show(DDrawingArea);
    gtk_widget_show(DMainWindow);
    
    gdk_window_set_cursor(DDrawingArea->window, DBlankCursor); 
    
    //MainData.DDrawingContext = gdk_gc_new(MainData.DDrawingArea->window);
    DDrawingContext = DDrawingArea->style->fg_gc[gtk_widget_get_state(DDrawingArea)];
    
    

    if(!D2DTerrainTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/2DTerrain.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DTerrainTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DTerrain.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DFloorTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DFloor.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DWallTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DWall.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DCannonTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DCannon.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DCannonballTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DCannonball.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DExplosionTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DExplosion.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DBurnTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DBurn.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DCannonPlumeTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DCannonPlume.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D3DCastleTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/3DCastle.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D2DCastleCannonTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/2DCastleCannon.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }    
    if(!D2DCastleSelectTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/2DCastleSelect.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!D2DWallFloorTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/2DWallFloor.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!DDigitTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/Digits.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!DBrickTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/Bricks.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!DMortarTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/Mortar.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!DTargetTileset.LoadTileset(DDrawingArea->window, DDrawingContext, "data/Target.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!DWhiteFont.LoadFont(DDrawingArea->window, DDrawingContext, "data/FontKingthingsWhite.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!DBlackFont.LoadFont(DDrawingArea->window, DDrawingContext, "data/FontKingthingsBlack.dat")){
        printf("Failed to load tileset.\n");
        return -1;
    }
    if(!DSoundMixer.LoadLibrary("data/SoundClips.dat")){
        printf("Failed to load sound clips.\n");
        return -1;
    }
    
    DExplosionSteps = D3DExplosionTileset.TileCount() / etMax;
    DBurnSteps = D3DBurnTileset.TileCount() / btMax;
    DCannonPlumeSteps = D3DCannonPlumeTileset.TileCount() / dMax;
    
    D2DCastleIndices[pcNone] = D2DCastleCannonTileset.FindTile("castle-none");
    D2DCastleIndices[pcBlue] = D2DCastleCannonTileset.FindTile("castle-blue");
    D2DCastleIndices[pcRed] = D2DCastleCannonTileset.FindTile("castle-red");
    D2DCastleIndices[pcYellow] = D2DCastleCannonTileset.FindTile("castle-yellow");
    D2DFloorIndices[pcNone] = D2DWallFloorTileset.FindTile("floor-even");
    D2DFloorIndices[pcBlue] = D2DWallFloorTileset.FindTile("floor-blue");
    D2DFloorIndices[pcRed] = D2DWallFloorTileset.FindTile("floor-red");
    D2DFloorIndices[pcYellow] = D2DWallFloorTileset.FindTile("floor-yellow");
    D2DWallIndices[pcNone] = D2DWallFloorTileset.FindTile("bad-0");
    D2DWallIndices[pcBlue] = D2DWallFloorTileset.FindTile("blue-0");
    D2DWallIndices[pcRed] = D2DWallFloorTileset.FindTile("red-0");
    D2DWallIndices[pcYellow] = D2DWallFloorTileset.FindTile("yellow-0");
    D2DWallIndices[pcMax] = D2DWallFloorTileset.FindTile("good-0");
    D2DWallIndices[pcMax+1] = D2DWallFloorTileset.FindTile("good-alt-0");
    D2DCannonIndices[pcNone] = D2DCastleCannonTileset.FindTile("cannon");
    D2DCannonIndices[pcBlue] = D2DCastleCannonTileset.FindTile("cannon-blue-1");
    D2DCannonIndices[pcRed] = D2DCastleCannonTileset.FindTile("cannon-red-1");
    D2DCannonIndices[pcYellow] = D2DCastleCannonTileset.FindTile("cannon-yellow-1");
    D2DSelectColorIndices[pcNone] = 0;
    D2DSelectColorIndices[pcBlue] = D2DCastleSelectTileset.FindTile("blue-0");
    D2DSelectColorIndices[pcRed] = D2DCastleSelectTileset.FindTile("red-0");
    D2DSelectColorIndices[pcYellow] = D2DCastleSelectTileset.FindTile("yellow-0");
    D2DDamagedGroundIndex = D2DTerrainTileset.FindTile("hole");
    
    D3DFloorIndices[pcNone] = 0;
    D3DFloorIndices[pcBlue] = D3DFloorTileset.FindTile("floor-blue");
    D3DFloorIndices[pcRed] = D3DFloorTileset.FindTile("floor-red");
    D3DFloorIndices[pcYellow] = D3DFloorTileset.FindTile("floor-yellow");
    
    D3DTargetIndices[pcNone] = 0;
    D3DTargetIndices[pcBlue] = DTargetTileset.FindTile("blue-target");
    D3DTargetIndices[pcRed] = DTargetTileset.FindTile("red-target");
    D3DTargetIndices[pcYellow] = DTargetTileset.FindTile("yellow-target");
    
    D3DCastleIndices[pcNone] = D3DCastleTileset.FindTile("castle-none");
    D3DCastleIndices[pcBlue] = D3DCastleTileset.FindTile("castle-blue-0");
    D3DCastleIndices[pcRed] = D3DCastleTileset.FindTile("castle-red-0");
    D3DCastleIndices[pcYellow] = D3DCastleTileset.FindTile("castle-yellow-0");

    D3DDamagedWallIndices[pcNone] = 0;
    D3DDamagedWallIndices[pcBlue] = D3DWallTileset.FindTile("blue-damaged-0");
    D3DDamagedWallIndices[pcRed] = D3DWallTileset.FindTile("red-damaged-0");
    D3DDamagedWallIndices[pcYellow] = D3DWallTileset.FindTile("yellow-damaged-0");
    for(int Index = 0; Index < 16; Index++){
        char Name[16];
        sprintf(Name,"blue-%d-0",Index);
        D3DWallIndices[pcNone][Index] = D3DWallTileset.FindTile(Name);
        D3DWallIndices[pcBlue][Index] = D3DWallTileset.FindTile(Name);
        sprintf(Name,"red-%d-0",Index);
        D3DWallIndices[pcRed][Index] = D3DWallTileset.FindTile(Name);
        sprintf(Name,"yellow-%d-0",Index);
        D3DWallIndices[pcYellow][Index] = D3DWallTileset.FindTile(Name);
        
    }
    D3DDamagedGroundIndex = D3DTerrainTileset.FindTile("hole-0");
    
    
    D3DExplosionIndices[etWallExplosion0] = D3DExplosionTileset.FindTile("explosion-0");
    D3DExplosionIndices[etWallExplosion1] = D3DExplosionTileset.FindTile("explosion-alt-0");
    D3DExplosionIndices[etWaterExplosion0] = D3DExplosionTileset.FindTile("water-explosion-0");
    D3DExplosionIndices[etWaterExplosion1] = D3DExplosionTileset.FindTile("water-explosion-alt-0");
    D3DExplosionIndices[etGroundExplosion0] = D3DExplosionTileset.FindTile("ground-explosion-0");
    D3DExplosionIndices[etGroundExplosion1] = D3DExplosionTileset.FindTile("ground-explosion-alt-0");

    D3DBurnIndices[btRubbleBurn0] = D3DBurnTileset.FindTile("rubbleburn-0");
    D3DBurnIndices[btRubbleBurn1] = D3DBurnTileset.FindTile("rubbleburn-alt-0");
    D3DBurnIndices[btHoleBurn0] = D3DBurnTileset.FindTile("holeburn-0");
    D3DBurnIndices[btHoleBurn1] = D3DBurnTileset.FindTile("holeburn-alt-0");
    
    D3DCannonPlumeIndices[dNorth] = D3DCannonPlumeTileset.FindTile("north-0");
    D3DCannonPlumeIndices[dNorthEast] = D3DCannonPlumeTileset.FindTile("northeast-0");
    D3DCannonPlumeIndices[dEast] = D3DCannonPlumeTileset.FindTile("east-0");
    D3DCannonPlumeIndices[dSouthEast] = D3DCannonPlumeTileset.FindTile("southeast-0");
    D3DCannonPlumeIndices[dSouth] = D3DCannonPlumeTileset.FindTile("south-0");
    D3DCannonPlumeIndices[dSouthWest] = D3DCannonPlumeTileset.FindTile("southwest-0");
    D3DCannonPlumeIndices[dWest] = D3DCannonPlumeTileset.FindTile("west-0");
    D3DCannonPlumeIndices[dNorthWest] = D3DCannonPlumeTileset.FindTile("northwest-0");
    
    DSoundClipIndices[sctTick] = DSoundMixer.FindClip("tick");
    DSoundClipIndices[sctTock] = DSoundMixer.FindClip("tock");
    DSoundClipIndices[sctCannon0] = DSoundMixer.FindClip("cannon0");
    DSoundClipIndices[sctCannon1] = DSoundMixer.FindClip("cannon1");
    DSoundClipIndices[sctPlace] = DSoundMixer.FindClip("place");
    DSoundClipIndices[sctTriumph] = DSoundMixer.FindClip("triumph");
    DSoundClipIndices[sctExplosion0] = DSoundMixer.FindClip("explosion0");
    DSoundClipIndices[sctExplosion1] = DSoundMixer.FindClip("explosion1");
    DSoundClipIndices[sctExplosion2] = DSoundMixer.FindClip("explosion2");
    DSoundClipIndices[sctExplosion3] = DSoundMixer.FindClip("explosion3");
    DSoundClipIndices[sctGroundExplosion0] = DSoundMixer.FindClip("groundexplosion0");
    DSoundClipIndices[sctGroundExplosion1] = DSoundMixer.FindClip("groundexplosion1");
    DSoundClipIndices[sctWaterExplosion0] = DSoundMixer.FindClip("waterexplosion0");
    DSoundClipIndices[sctWaterExplosion1] = DSoundMixer.FindClip("waterexplosion1");
    DSoundClipIndices[sctWaterExplosion2] = DSoundMixer.FindClip("waterexplosion2");
    DSoundClipIndices[sctWaterExplosion3] = DSoundMixer.FindClip("waterexplosion3");
    DSoundClipIndices[sctReady] = DSoundMixer.FindClip("ready");
    DSoundClipIndices[sctAim] = DSoundMixer.FindClip("aim");
    DSoundClipIndices[sctFire] = DSoundMixer.FindClip("fire");
    DSoundClipIndices[sctCeasefire] = DSoundMixer.FindClip("ceasefire");
    DSoundClipIndices[sctTransition] = DSoundMixer.FindClip("transition");
    
    DSongIndices[stLoss] = DSoundMixer.FindSong("loss");
    DSongIndices[stWin] = DSoundMixer.FindSong("win");
    DSongIndices[stMenu] = DSoundMixer.FindSong("menu");
    DSongIndices[stRebuild] = DSoundMixer.FindSong("rebuild");
    DSongIndices[stPlace] = DSoundMixer.FindSong("place");
    
    
    DBrickIndices[bbtTopCenter] = DBrickTileset.FindTile("brick-tc");
    DBrickIndices[bbtTopRight] = DBrickTileset.FindTile("brick-tr");
    DBrickIndices[bbtRight0] = DBrickTileset.FindTile("brick-r0");
    DBrickIndices[bbtRight1] = DBrickTileset.FindTile("brick-r1");
    DBrickIndices[bbtBottomRight] = DBrickTileset.FindTile("brick-br");
    DBrickIndices[bbtBottomCenter] = DBrickTileset.FindTile("brick-bc");
    DBrickIndices[bbtBottomLeft] = DBrickTileset.FindTile("brick-bl");
    DBrickIndices[bbtLeft0] = DBrickTileset.FindTile("brick-l0");
    DBrickIndices[bbtLeft1] = DBrickTileset.FindTile("brick-l1");
    DBrickIndices[bbtTopLeft] = DBrickTileset.FindTile("brick-tl");
    DBrickIndices[bbtSingle] = DBrickTileset.FindTile("brick-single");
    
    
    DMortarIndices[bmtTopCenter] = DMortarTileset.FindTile("mortar-tc");
    DMortarIndices[bmtTopRight0] = DMortarTileset.FindTile("mortar-tr0");
    DMortarIndices[bmtTopRight1] = DMortarTileset.FindTile("mortar-tr1");
    DMortarIndices[bmtTopRight2] = DMortarTileset.FindTile("mortar-tr2");
    DMortarIndices[bmtTopLeft0] = DMortarTileset.FindTile("mortar-tl0");
    DMortarIndices[bmtTopLeft1] = DMortarTileset.FindTile("mortar-tl1");
    DMortarIndices[bmtTopLeft2] = DMortarTileset.FindTile("mortar-tl2");
    DMortarIndices[bmtBottomCenter] = DMortarTileset.FindTile("mortar-bc");
    DMortarIndices[bmtBottomRight0] = DMortarTileset.FindTile("mortar-br0");
    DMortarIndices[bmtBottomRight1] = DMortarTileset.FindTile("mortar-br1");
    DMortarIndices[bmtBottomRight2] = DMortarTileset.FindTile("mortar-br2");
    DMortarIndices[bmtBottomLeft0] = DMortarTileset.FindTile("mortar-bl0");
    DMortarIndices[bmtBottomLeft1] = DMortarTileset.FindTile("mortar-bl1");
    DMortarIndices[bmtBottomLeft2] = DMortarTileset.FindTile("mortar-bl2");
    DMortarIndices[bmtRightCenter] = DMortarTileset.FindTile("mortar-rc");
    DMortarIndices[bmtRightBottom0] = DMortarTileset.FindTile("mortar-rb0");
    DMortarIndices[bmtRightBottom1] = DMortarTileset.FindTile("mortar-rb1");
    DMortarIndices[bmtRightBottom2] = DMortarTileset.FindTile("mortar-rb2");
    DMortarIndices[bmtRightTop0] = DMortarTileset.FindTile("mortar-rt0");
    DMortarIndices[bmtRightTop1] = DMortarTileset.FindTile("mortar-rt1");
    DMortarIndices[bmtRightTop2] = DMortarTileset.FindTile("mortar-rt2");
    DMortarIndices[bmtLeftCenter] = DMortarTileset.FindTile("mortar-lc");
    DMortarIndices[bmtLeftBottom0] = DMortarTileset.FindTile("mortar-lb0");
    DMortarIndices[bmtLeftBottom1] = DMortarTileset.FindTile("mortar-lb1");
    DMortarIndices[bmtLeftBottom2] = DMortarTileset.FindTile("mortar-lb2");
    DMortarIndices[bmtLeftTop0] = DMortarTileset.FindTile("mortar-lt0");
    DMortarIndices[bmtLeftTop1] = DMortarTileset.FindTile("mortar-lt1");
    DMortarIndices[bmtLeftTop2] = DMortarTileset.FindTile("mortar-lt2");
    
    
    MapDirectory = opendir(MapPath.c_str());
    if(NULL == MapDirectory){
        printf("Failed to open directory.\n");
        return -1;
    }
    while((DirectoryEntry = readdir( MapDirectory ))){
        std::string Filename = MapPath + "/";
        Filename += DirectoryEntry->d_name;
        if(Filename.rfind(".map") == (Filename.length() - 4)){
            CTerrainMap TempMap;
            
            if(!TempMap.LoadMap(&D2DTerrainTileset, &D3DTerrainTileset, Filename)){
                printf("Failed to load map \"%s\".\n",Filename.c_str());
                continue;
            }
            DTerrainMaps.push_back(TempMap);
        }
    }
    closedir(MapDirectory);
    if(0 == DTerrainMaps.size()){
        printf("No maps loaded.\n");
        return 0;
    }
    DScaling = 1;
    
    LoadTerrainMap(0);
    ChangeMode(gmMainMenu);
    DrawMenu();
    
    gdk_draw_pixmap(DPreviousWorkingBufferPixmap, DDrawingContext, DWorkingBufferPixmap, 0, 0, 0, 0, -1, -1);
    gdk_draw_pixmap(DDoubleBufferPixmap, DDrawingContext, DWorkingBufferPixmap, 0, 0, 0, 0, -1, -1);
    gdk_draw_pixmap(DDrawingArea->window, DDrawingContext, DDoubleBufferPixmap, 0, 0, 0, 0, -1, -1);

    
    gettimeofday(&DNextExpectedTimeout, NULL);
    DNextExpectedTimeout.tv_usec += TIMEOUT_INTERVAL * 1000;
    if(1000000 <= DNextExpectedTimeout.tv_usec){
        DNextExpectedTimeout.tv_usec %= 1000000;
        DNextExpectedTimeout.tv_sec++;
    }
    g_timeout_add(TIMEOUT_INTERVAL, TimeoutCallback, this);
    // All GTK applications must have a gtk_main(). Control ends here and waits 
    // for an event to occur (like a key press or mouse event). 
    gtk_main ();
    return 0;
}


int main(int argc, char *argv[]){
    CApplicationData MainData;
    int ReturnValue;

    printf("Copyright (c) 2015, Christopher Nitta\n");
    printf("All rights reserved.\n\n");
    printf("All source material (source code, images, sounds, etc.) have been provided to\n");
    printf("University of California, Davis students of course ECS 160 for educational\n");
    printf("purposes. It may not be distributed beyond those enrolled in the course without\n"); 
    printf("prior permission from the copyright holder.\n\n");          
    printf("Some sound files, sound fonts, and midi files have been included that were\n");
    printf("freely available via internet sources. They have been included in this\n");
    printf("distribution for educational purposes only and this copyright notice does not\n"); 
    printf("attempt to claim any ownership of this material.\n");
    
    ReturnValue = MainData.Init(argc, argv);

    return ReturnValue;
}

