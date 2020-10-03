#include "FormGenerator.h"
#include "hpdf.h"
#include <setjmp.h>
#include <time.h>       /* time */

class PDFCreator {

    public:

        string PDFPath;
        
        vector<string> TempFiles;
        
        CodeGenerator gen;

        PDFCreator(string PDFPath);

        string generateTempPDFDocumentFromOptions(vector<string> options, vector<string> variables);

};
