#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H

/**
 * CRandomNumberGenerator class, used to generate a random number
 */

class CRandomNumberGenerator{
    protected:
		/**
		 * This is the seed that generates the upper half of the random number.
		 */
        unsigned int DRandomSeedHigh;
        /**
         * This is the seed that generates the lower half of the random number.
         */
        unsigned int DRandomSeedLow;
        
    public:
		/**
		 * The default constructor just initializes DRandomSeedHigh and DRandomSeedLow to fixed hex values.
		 */
        CRandomNumberGenerator(){
            DRandomSeedHigh = 0x01234567;
            DRandomSeedLow = 0x89ABCDEF;
        };
        
        /** Generates a random number using the 2 seeds.
         * 
         * This sets DRandomSeedHigh and DrandomseedLow.
         *
         * @param low the new value of DramdomseedLow
         * @param high the new value of DrandomseedHigh
         *  
         * Low and high must not be the same value, and they must both exist
         */
        void Seed(unsigned int high, unsigned int low){
            if((high != low) && low && high){
                DRandomSeedHigh = high;
                DRandomSeedLow = low;   
            }
        };
        
        /**
         * Changes the value of DRandomHigh and DrandomLow.
         * 
         * Returns an int where DRandomSeedHigh affects the upper 16 bits and DRandomlow affects all 32 bits
         */
        unsigned int Random(){
			/* 
			 * multiplies the lower 16 digits by a set number (different for low and high)
			 * adds this to the upper 16 digits shifted right by 16 bit
			 */
            DRandomSeedHigh = 36969 * (DRandomSeedHigh & 65535) + (DRandomSeedHigh >> 16);
            DRandomSeedLow = 18000 * (DRandomSeedLow & 65535) + (DRandomSeedLow >> 16);
            return (DRandomSeedHigh << 16) + DRandomSeedLow;
        };
};

#endif
