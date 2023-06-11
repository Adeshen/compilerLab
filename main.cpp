#include <iostream>
#include <stdio.h>
#include "driver.hpp"
#include "semantic_symbol_struct.h"
#include "semantic.h"
#include "inter.h"
#include "inter_struct.h"
#include "assembly.h"

bool lexError=false;
bool synError=false;
bool semError=false;


pNode root;
pTable table;


int  main (int argc, char *argv[])
{
    int res = 0;
    driver drv;
    int mode=0;
    int in=1,out=2;
    // for (int i = 1; i < argc; ++i){
    //     if (argv[i] == std::string ("-p"))
    //         drv.trace_parsing = true;
    //     else if (argv[i] == std::string ("-s"))
    //         drv.trace_scanning = true;
    //     else if (mode==0){
    //         in=i; mode==1;
    //     }
    //     else if(mode==1){
    //         drv.out=argv[out];
    //     }
    // }

    drv.parse (argv[in]);      //printTreeInfo(root,0);
    FILE* fw = fopen(argv[2], "wt+");
    // printf("%s\n",argv[2]);
    if(!lexError && !synError){
        // fw=NULL;
        // printTreeInfo(root,0);
        table=initTable();
        traverseTree(root);
        // printf("hello.world");
        // deleteTable(table);
        
        if(!semError){
            interCodeList = newInterCodeList();
            genInterCodes(root);        
            if (!interError) {
                //printInterCode(NULL, interCodeList);
                // printInterCode(fw, interCodeList);
                genAssemblyCode(fw);
            }
        }

        // deleteInterCodeList(interCodeList);
        deleteTable(table);
    }
    return res;
}
