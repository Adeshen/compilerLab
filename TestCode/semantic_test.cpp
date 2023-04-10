#include "../semantic_symbol_struct.h"
#include "../enum.hpp"
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

pType newTypeTest(Kind kind, ...) {
    va_list vaList;
    BasicType  hhxtype;
    va_start(vaList, 1);
    hhxtype= va_arg(vaList, BasicType );
    pType p;
    // p->u.basic =hhxtype;
    // int k=va_arg(vaList,int );

    va_end(vaList);
    return p;
}



#include <stdarg.h>



enum H{
    h,hh,hhh
};

H get_arg(int x,...){
    va_list vaList;
    va_start(vaList, x);

    H hh=static_cast<H>(va_arg(vaList, int));
    return hh;
}


int main(){

    get_arg(2,H::hh, H::h);
  
}


    // BasicType type=BasicType::FLOAT_TYPE;    
    // newType(Kind::BASIC,type);


    // newTypeTest(Kind::BASIC,64);
    // newTypeTest(Kind::BASIC,BasicType::FLOAT_TYPE);
