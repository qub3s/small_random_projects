#include <iostream>
#include <stdlib.h>  // atio


class matrix{
    public:
        int x_dim;
        int y_dim;
        int *mat;

    matrix( int x, int y ){
        x_dim = x;
        y_dim = y;

        mat = new int[x_dim*y_dim];
    }

    matrix( int y ){
        x_dim = 1;
        y_dim = y;

        mat = new int[x_dim*y_dim];
    }

    // adds matrices
    matrix add( matrix o ){
        if( o.x_dim != x_dim || o.y_dim != y_dim ){
            throw std::logic_error( "Dimensions for addition have to be the same !" );
        }

        matrix m = matrix(x_dim, y_dim);

        for( int i = 0; i < x_dim*y_dim; i++ ){
            m.at_unsafe(i) = o.at_unsafe(i) + this->at_unsafe(i);
        }

        return m;

    }

    void transpose(){
        int tmp = x_dim;
        x_dim = y_dim;
        y_dim = tmp;
    }

    matrix mult( matrix o ){
        if( x_dim != o.y_dim ){
            throw std::logic_error( "Wrong Dimensions to Mulitpy !" );
        }

        matrix m = matrix(o.x_dim, y_dim);

        for( int y = 0; y < y_dim; y++ ){
            for( int x = 0; x < o.x_dim; x++){
                for( int z = 0; z < x_dim; z++){
                    m.at(x,y) += this->at(z,y) * o.at(x,z);
                }
            }
        }

        return m;
    }

    // no bounds checking
    int& at_unsafe(int x, int y){
        int &ret = mat[x + y * x_dim];
        return ret;
    }

    // no bounds checking
    int& at_unsafe(int x){
        int &ret = mat[x];
        return ret;
    }

    // bounds checking
    int& at(int x, int y){
        if( x < 0 || x >= x_dim || y < 0 || y >= y_dim ){
            throw std::logic_error( "Out of bounds" );
        }

        int &ret = mat[x + y * x_dim];
        return ret;
    }

    // bounds checking
    int& at(int x){
        if( x < 0 || x >= x_dim*y_dim ){
            throw std::logic_error( "Out of bounds" );
        }

        int &ret = mat[x];
        return ret;
    }

    // prints out
    void out(){
        for( int y = 0; y < y_dim; y++ ){
            for( int x = 0; x < x_dim; x++){
                std::cout << mat[y*x_dim + x] << " ";
            }
            std::cout << "\n";
        }
    }
};

int main(int argc, char* argv[]){   
    matrix a = matrix(2,3);
    matrix b = matrix(2,3);
    int c = 0;

    for(int y = 0; y < 6; y++){
        a.at(y) = c;
        b.at(y) = c;
        c += 1;
    }

    b.transpose();
    
    a.out();
    std::cout << "\n";
    b.out();
    std::cout << "\n";
    a.mult(b).out();
}
