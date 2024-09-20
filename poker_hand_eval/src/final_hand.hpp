# pragma once

class final_hand{
    public:
        // highcard -> 0; Royal Flush -> 9
        int strength;
        int c[5];

        final_hand(int* cv, int s);

        bool operator > (final_hand x){
            if (strength > x.strength)
                return true;

            if( strength < x.strength)
                return false;

            for(int k = 0; k < 5; k++){
                if (c[k] > x.c[k])
                    return true;

                if( x.c[k] < c[k])
                    return false;
            }
            return false;
        }

        void show();
};
