#include "Header/PDFCreator.h"

jmp_buf env;

void error_handler (HPDF_STATUS   error_no,
                    HPDF_STATUS   detail_no,
                    void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
            (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

PDFCreator::PDFCreator(string path) {
    PDFCreator::PDFPath = path;
}

string PDFCreator::generateTempPDFDocumentFromOptions(vector<string> options, vector<string> variables) {
    /* options vector contains content of "Form.txt" */
    /* variables vector contains data of the submission */

    HPDF_Doc pdf = HPDF_New (error_handler, NULL);
    if (!pdf) {
        printf ("ERROR: cannot create pdf object.\n");
    }

    if (setjmp(env)) {
        HPDF_Free (pdf);
    }

    /* set compression mode */
    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    HPDF_REAL height;
    HPDF_REAL width;
    HPDF_Font def_font;
    HPDF_Page page;

    page = HPDF_AddPage (pdf);
    height = HPDF_Page_GetHeight (page);
    width = HPDF_Page_GetWidth (page);
    HPDF_Page_SetLineWidth (page, 1);
    HPDF_Page_Rectangle (page, 50, 10, width-70, height-20);
    HPDF_Page_Stroke (page);

    FormGenerator form;

    const char *fontname;

    HPDF_UseUTFEncodings(pdf);

    // font is loaded
    fontname = HPDF_LoadTTFontFromFile(pdf, "Fonts/AbhayaLibre-Regular.ttf", HPDF_TRUE);
    def_font = HPDF_GetFont (pdf, fontname,  "UTF-8");
    HPDF_Page_SetFontAndSize (page, def_font, 14);
    HPDF_Page_BeginText (page);

    HPDF_Page_TextOut (page, 60, height - 40, (options[0]).c_str());

    int YPos = 120;
    int optionsID = 2;

    bool inCheckbox = false;
    for(int i = 0; i < variables.size(); i++) {
        if(options[optionsID-1] != "checkbox-end") {
            if(!inCheckbox) {
                if(options[optionsID-1] == "checkbox-display" && variables[i] == "false")
                    inCheckbox = true;

                // when page is full, new page is created
                if(YPos+30 >= height) {
                    HPDF_Page_EndText (page);
                    page = HPDF_AddPage (pdf);
                    HPDF_Page_SetLineWidth (page, 1);
                    HPDF_Page_Rectangle (page, 50, 10, width-70, height-20);
                    HPDF_Page_Stroke (page);
                    HPDF_Page_SetFontAndSize (page, def_font, 14);
                    HPDF_Page_BeginText (page);
                    YPos = 40;
                    cout << "New page" << endl;
                }

                vector<string> elements = form.SplitString(options[optionsID], ",");

                cout << elements[0] << endl;
                cout << variables[i] << endl;

                int sizeOfTextField = 20;

                if(elements[0] != "")
                    HPDF_Page_TextOut (page, 60, height - YPos, (elements[0]+":").c_str());

                /* making text fit inside page */

                int step = (width-90)/8;
                int j = 0;
                while(j < variables[i].size()) {
                    int len = step;
                    if((len + j) > variables[i].size())
                        len = variables[i].size() - j;

                    /* text gets cut in portions of len characters */
                    string temp = variables[i].substr(j, len);

                    /* check for end of the paragraph */
                    size_t pos = temp.find("\r\n");
                    if(pos == string::npos) {
                        /* if there is no paragraph end, the text is separated by spaces */
                        pos = temp.rfind(" ");
                        if(j+len < variables[i].size() && pos != string::npos && pos > 0) {
                            temp = temp.substr(0, pos);
                            j++;
                        }
                    }
                    else {
                        /* text up to the end of the paragraph is copied */
                        temp = temp.substr(0, pos);
                        j += 2;
                    }

                    /* temp is saved */
                    HPDF_Page_TextOut (page, 60, height - YPos - sizeOfTextField, temp.c_str());
                    sizeOfTextField += 18;
                    j += temp.size();
                    /* same process returns for next part of text */
                }

                YPos += (sizeOfTextField+20);
            }
            optionsID += 2;
        }
        else {
            optionsID++;
            i--;
            inCheckbox = false;
        }
    }

    // generate code for temporal storage
    gen = CodeGenerator(time(NULL), 10, "");
    std::string filename = gen.getCode();
    HPDF_SaveToFile (pdf, (PDFPath+filename+".pdf").c_str());

    /* clean up */
    HPDF_Free (pdf);

    return PDFPath+filename+".pdf";
}
