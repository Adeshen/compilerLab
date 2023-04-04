#include "driver.hpp"
#include "parser.hpp"



extern bool yy_flex_debug;
extern FILE * yyin;
extern FILE * yyout;
extern pNode root;

driver::driver ()
: trace_parsing (false), trace_scanning (false)
{
    variables["one"] = 1;
    variables["two"] = 2;
}

int
driver::parse (const std::string &f)
{
    file = f;
    scan_begin ();
    // parse.set_debug_level (trace_parsing);
    int res = yyparse ();
    scan_end ();
    //printTreeInfo(root,0);
    
    return res;
}

void
driver::scan_begin ()
{
    yy_flex_debug = trace_scanning;
    if (file.empty () || file == "-")
        yyin = stdin;
    else if (!(yyin = fopen (file.c_str (), "rw")))
    {
        std::cerr << "cannot open " << file << ": " << errno << '\n';
        exit (EXIT_FAILURE);
    }
    //out=="./hello.txt";
    if (out.empty () || file == "-")
        yyout = stdout;
    else if (!(yyout = fopen (out.c_str (), "w")))
    {
        std::cerr << "cannot open " << out << ": " << errno << '\n';
        exit (EXIT_FAILURE);
    }


}

void
driver::scan_end ()
{
    fclose (yyin);
    //fclose (yyout);
}
