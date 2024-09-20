#include "matrix_mult_2d.cpp"
#include <math.h>

const float bb_deep = 100;
const int max_lvl = 11;                                                                      // how deep the tree is
const int num_betsize = 6;                                                                  // how many betting sizes are beeing learned
const float betsize[num_betsize] = {0.3, 0.5, 0.7, 1, 2, INFINITY};                         // different pot sizes, compared to the current potsize
static int elementcount = 0;

class node{
    // function gettype to cast pointer
    public:
        bool isendnode;
};

class leaf_node : public node{
    public:
        float bbs_in_play;

    leaf_node(float bbs_in_play){
        this->isendnode = false;
        this->bbs_in_play = bbs_in_play;
        elementcount += 1;
    }
};

class preflop_node : public node{
    public:
        matrix positive_regret = matrix();  
        matrix negative_regret = matrix();   

        int num_choices;                      
        int player_id;                         
        
        node* childs;
        preflop_node* parent;
        
        preflop_node(){};

        // creates tree for preflop play
        preflop_node(float bbs_in_pot_by_bb, float bbs_in_pot_by_sb, int lvl, preflop_node* parent){
            this->isendnode = false;
            //elementcount += 1 ;
            bool smallblind = (lvl%2 == 0);
            
            this->player_id = (int)smallblind;
            this->parent = parent;
            
            // all in -> options: call, fold
            if( bbs_in_pot_by_sb >= bb_deep || bbs_in_pot_by_bb >= bb_deep ){
                this->num_choices = 2;
                this->childs = new node[this->num_choices];
                
                if(smallblind){
                    childs[0] = leaf_node(-bbs_in_pot_by_sb);
                }
                else{
                    childs[0] = leaf_node(-bbs_in_pot_by_bb);
                }

                childs[1] = leaf_node(bb_deep);        
            }

            // lvl 0 (normal node but call continues the tree
            else if (lvl == 0){  
                this->num_choices = num_betsize+2;
                this->childs = new node[this->num_choices];
                
                if(smallblind){
                    childs[0] = leaf_node(-bbs_in_pot_by_sb);
                }
                else{
                    childs[0] = leaf_node(-bbs_in_pot_by_bb);
                }
                
                this->childs[1] = preflop_node( bbs_in_pot_by_bb, bbs_in_pot_by_bb, lvl+1, this);          // call and continue the tree

                for(int i = 2; i < num_choices; i++){
                    bbs_in_pot_by_sb = bbs_in_pot_by_bb + ( bbs_in_pot_by_bb + bbs_in_pot_by_sb ) * betsize[i-2];
                    this->childs[i] = preflop_node(bbs_in_pot_by_bb, bbs_in_pot_by_sb, lvl+1, this);
                }
            }
            // lvl capping call/fold/all-in
            else if (lvl >= max_lvl){
                this->num_choices = 3;  
                this->childs = new node[this->num_choices];
                
                if(smallblind){
                    childs[0] = leaf_node(-bbs_in_pot_by_sb);
                    childs[1] = leaf_node(bbs_in_pot_by_bb);
                }
                else{
                    childs[0] = leaf_node(-bbs_in_pot_by_bb);
                    childs[1] = leaf_node(bbs_in_pot_by_sb);
                }
                                                    
                if(this->player_id == 0){
                    this->childs[2] = preflop_node(bbs_in_pot_by_bb, bb_deep , lvl+1, this);
                }
                else{
                    this->childs[2] = preflop_node(bb_deep, bbs_in_pot_by_sb, lvl+1, this);
                }
            }
            // all other
            else{
                this->num_choices = num_betsize+2;
                this->childs = new node[this->num_choices];
                
                if(smallblind){
                    childs[0] = leaf_node(-bbs_in_pot_by_sb);
                    childs[1] = leaf_node(bbs_in_pot_by_bb);
                }
                else{
                    childs[0] = leaf_node(-bbs_in_pot_by_bb);
                    childs[1] = leaf_node(bbs_in_pot_by_sb);
                }

                for(int i = 2; i < num_choices; i++){
                    if(this->player_id == 0){
                        bbs_in_pot_by_sb = bbs_in_pot_by_bb + ( bbs_in_pot_by_bb + bbs_in_pot_by_sb ) * betsize[i-2];
                        this->childs[i] = preflop_node(bbs_in_pot_by_bb, bbs_in_pot_by_sb, lvl+1, this);
                    }
                    else{
                        bbs_in_pot_by_bb = bbs_in_pot_by_sb + ( bbs_in_pot_by_bb + bbs_in_pot_by_sb ) * betsize[i-2];
                        this->childs[i] = preflop_node(bbs_in_pot_by_bb, bbs_in_pot_by_sb, lvl+1, this);
                    }
                }
            }
        }
};
