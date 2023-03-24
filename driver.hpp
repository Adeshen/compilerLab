

#ifndef DRIVER_HH
#define DRIVER_HH
# include <string>
# include <map>
# include "ASTNodes.hpp"



class driver
{
public:
    driver ();
    std::map<std::string, int> variables;
    int result;

    // Run the parser on file F. Return 0 on success.
    int parse (const std::string& f);
    // The name of the file being parsed.
    std::string file="",out="";
    // Whether to generate parser debug traces.
    bool trace_parsing;

// To encapsulate the coordination with the Flex scanner, it is useful to have member functions
// to open and close the scanning phase.
// Handling the scanner.
    void scan_begin ();
    void scan_end ();
    // Whether to generate scanner debug traces.
    bool trace_scanning;
    // The token’s location used by the scanner.
};
#endif // ! DRIVER_HH