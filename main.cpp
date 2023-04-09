#include <iostream>
#include "driver.hpp"
#include "semantic_symbol_struct.h"
#include "semantic.h"


bool lexError=false;
bool synError=false;
pNode root;
pTable table=initTable();


int  main (int argc, char *argv[])
{
    int res = 0;
    driver drv;
    int mode=0;
    int in,out;
    for (int i = 1; i < argc; ++i){
        if (argv[i] == std::string ("-p"))
            drv.trace_parsing = true;
        else if (argv[i] == std::string ("-s"))
            drv.trace_scanning = true;
        else if (mode==0){
            in=i; mode==1;
        }
        else if(mode==1){
            drv.out=argv[out];
        }
    }
    drv.parse (argv[in]);      //printTreeInfo(root,0);
    if(!lexError && !synError){
        

        printTreeInfo(root,0);
        traverseTree(root);
        // printf("hello.world");
        // deleteTable(table);

    }


    return res;
}
