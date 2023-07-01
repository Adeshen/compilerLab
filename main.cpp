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
    int in=1,out_ir=-1,out_s=-1;
    for (int i = 1; i < argc; ++i){
        if (argv[i] == std::string ("-ir")){
            out_ir=i+1;
        }
        else if (argv[i] == std::string ("-s")){
            out_s=i+1;
        }
    }
   
    if(out_ir<0&&out_s<0){

        printf(
            "————help document of the compile about cmm——————  \n\n  "
"parser <output_file> [-ir <ir_path>] [-s <s_file>]  \n\n"
"parameter: \n"
"  -ir assign the path of ir file, with the suffix .ir \n\n"
"  -s assign the path of assembly file with the suffix .s \n\n"
"reference case: \n"
" ./parser Test4/example.cmm -ir ResultUltimate/example.cmm.ir -s ResultUltimate/example.cmm.s \n\n"
        );
        return 0;
    }


    drv.parse (argv[in]);      //printTreeInfo(root,0);
    // FILE* fw = fopen(argv[2], "wt+");
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
                // printInterCode(NULL, interCodeList);

                if(out_ir>0){
                    FILE* fw = fopen(argv[out_ir], "wt+");
                    printInterCode(fw, interCodeList);
                }
                if(out_s>0){
                    FILE* fw = fopen(argv[out_s], "wt+");
                    genAssemblyCode(fw);
                }
                
            }
        }

        // deleteInterCodeList(interCodeList);
        deleteTable(table);
    }
    return res;
}
