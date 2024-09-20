#include <stdexcept>

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

    matrix(){}

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

    // subtracts matrices
    matrix sub( matrix o ){
        if( o.x_dim != x_dim || o.y_dim != y_dim ){
            throw std::logic_error( "Dimensions for addition have to be the same !" );
        }

        matrix m = matrix(x_dim, y_dim);

        for( int i = 0; i < x_dim*y_dim; i++ ){
            m.at_unsafe(i) = o.at_unsafe(i) - this->at_unsafe(i);
        }

        return m;

    }

    void transpose(){
        int tmp = x_dim;
        x_dim = y_dim;
        y_dim = tmp;
    }

    // multiplies matrices
    matrix mult( matrix o ){
        if( x_dim != o.y_dim ){
            throw std::logic_error( "Wrong Dimensions to Mulitpy !" );
        }

        matrix m = matrix(o.x_dim, y_dim);

        for( int y = 0; y < y_dim; y++ ){
            for( int x = 0; x < o.x_dim; x++){
                for( int z = 0; z < x_dim; z++){
                    m.at_unsafe(x,y) += this->at_unsafe(z,y) * o.at_unsafe(x,z);
                }
            }
        }

        return m;
    }
    
    // multiplies matrices by value
    matrix mult( double o ){
        matrix m = matrix(x_dim, y_dim);

        for( int x = 0; x < x_dim * x_dim; x++ ){
            m.at_unsafe(x) = this->at_unsafe(x) * o;
        }

        return m;
    }

    // multiplies matrices
    matrix div( matrix o ){
        if( x_dim != o.y_dim ){
            throw std::logic_error( "Wrong Dimensions to Mulitpy !" );
        }

        matrix m = matrix(o.x_dim, y_dim);
        for( int y = 0; y < y_dim; y++ ){
            for( int x = 0; x < o.x_dim; x++){
                for( int z = 0; z < x_dim; z++){
                    m.at_unsafe(x,y) += this->at_unsafe(z,y) * ( 1 / o.at_unsafe(x,z) );
                }
            }
        }

        return m;
    }

    // divides matrice by double
    matrix div( double o ){
        o = 1/o;
        return this->mult(o);
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
};
