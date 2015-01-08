#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H

class CRandomNumberGenerator{
    protected:
        unsigned int DRandomSeedHigh;
        unsigned int DRandomSeedLow;
    public:
        CRandomNumberGenerator(){
            DRandomSeedHigh = 0x01234567;
            DRandomSeedLow = 0x89ABCDEF;
        };
        
        void Seed(unsigned int high, unsigned int low){
            if((high != low) && low && high){
                DRandomSeedHigh = high;
                DRandomSeedLow = low;   
            }
        };
        
        unsigned int Random(){
            DRandomSeedHigh = 36969 * (DRandomSeedHigh & 65535) + (DRandomSeedHigh >> 16);
            DRandomSeedLow = 18000 * (DRandomSeedLow & 65535) + (DRandomSeedLow >> 16);
            return (DRandomSeedHigh << 16) + DRandomSeedLow;
        };
};

#endif
