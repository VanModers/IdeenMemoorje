#include "Header/FormGenerator.h"

std::vector<string> FormGenerator::SplitString(string str, string seperator) {
	string temp = str;
	std::vector<string> splittedString;
	size_t pos = str.find(seperator);
	while (pos != std::string::npos) {
		splittedString.push_back(temp.substr(0, pos));
		temp.erase(0, pos + 1);
		pos = temp.find(seperator);
	}
	splittedString.push_back(temp.substr(0, temp.size()));

	return splittedString;
}

std::istream& safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case std::streambuf::traits_type::eof():
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

void FormGenerator::generateHTMLFile(string path, std::vector<string> options, std::vector<string> explanationText) {
	// options vector contains content of "Form.txt" which is used to create the html file
	if (options.size() > 0) {
		bool inCheckbox = false;
		int checkboxCount = 0;
		vector<string> checkboxVariables;

		// writing head of html file
		ofstream outfile(path);
		outfile << "<!DOCTYPE html><html>" << endl;
		outfile << "<head><meta http-equiv='content-type' content='text/html; charset=utf-8'><link rel='stylesheet' href='form.css'></head>" << endl;
		outfile << "<body style='background-color:yellowgreen;'>" << endl;
		outfile << "<h1>" << options[0] << "</h1>" << endl;

		outfile << "<form method = 'post' action='form'><br>" << endl;
		int variableID = 0;
		bool namefield = false;
		bool endOfCheckbox;
		// loping through all elements defined in options array
		for (int i = 1; i < options.size() - 1; i += 2) {
			outfile << "<br>" << endl;

			// translation into html code:

			endOfCheckbox = false;
			if (options[i] == "inputfield") {
				outfile << "<span>" << options[i + 1] << ":</span><br>" << endl;
				outfile << "<input type='text' placeholder='' name='var" << to_string(variableID) << "' required><br>" << endl;
				variableID++;
			}
			if (options[i] == "namefield") {
				namefieldPos = variableID;
				cout << namefieldPos << endl;
				namefield = true;
				outfile << "<span>" << options[i + 1] << ":</span><br>" << endl;
				outfile << "<input type='text' placeholder='' id='name' name='var" << to_string(variableID) << "' required><br>" << endl;
				variableID++;
			}
			else if (options[i] == "radio") {
				std::vector<string> labels = SplitString(options[i + 1], ",");
				outfile << "<span>" << labels[0] << "</span><br>" << endl;
				for (int j = 1; j < labels.size(); j++) {
					outfile << "<input type='radio' name='var" << to_string(variableID) << "' value='" << labels[j] << "' required>" << labels[j] << "<br>" << endl;
				}
				variableID++;
			}
			else if (options[i] == "textarea") {
				std::vector<string> elements = SplitString(options[i + 1], ",");
				outfile << "<span>" << elements[0] << ":</span><br>" << endl;
				outfile << "<textarea name='var" << to_string(variableID) << "' rows='10' cols='30' maxlength='" << elements[2] << "' ";
				if (elements[1] == "TRUE")
					outfile << "required ></textarea><br>" << endl;
				else
					outfile << "></textarea><br>" << endl;
				variableID++;
			}
			else if (options[i] == "number") {
				std::vector<string> elements = SplitString(options[i + 1], ",");
				outfile << "<span>" << elements[0] << ":</span><br>" << endl;
				outfile << "<input type='number' name='var" << to_string(variableID) << "' min='" << elements[1] << "' max='" << elements[2] << "'";
				if (!inCheckbox)
					outfile << " required><br>" << endl;
				else
					outfile << " ><br>" << endl;
				variableID++;
			}
			else if (options[i] == "drop-down") {
				std::vector<string> elements = SplitString(options[i + 1], ",");
				outfile << "<span>" << elements[0] << ":</span><br>" << endl;
				outfile << "<select name='var" << to_string(variableID) << "'>" << endl;
				for (int i = 1; i < elements.size(); i++) {
					outfile << "<option value='" << elements[i] << "'>" << elements[i] << "</option>" << endl;
				}
				outfile << "</select>" << endl;
				variableID++;
			}
			else if (options[i] == "drop-down-from-TextFile") {
				std::vector<string> elements = SplitString(options[i + 1], ",");
				outfile << "<span>" << elements[0] << ":</span><br>" << endl;
				ifstream ifs(elements[1], ifstream::in);

				outfile << "<select name='var" << to_string(variableID) << "'>" << endl;
				string str = "";
				while (getline(ifs, str)) {
					outfile << "<option value='" << str << "'>" << str << "</option>" << endl;
					str = "";
				}

				outfile << "</select>" << endl;
				variableID++;
			}
			else if (options[i] == "checkbox-display") {
				checkboxCount++;
				inCheckbox = true;
				outfile << "<span>" << options[i + 1] << ":</span><br>" << endl;
				outfile << "<input type='checkbox' name='var" << to_string(variableID) << "' id='checkbox" << to_string(variableID) << "' value='true'>" << endl;
				outfile << "<div id='div" << to_string(checkboxCount) << "'>" << endl;

				checkboxVariables.push_back("checkbox" + to_string(variableID));
				variableID++;
			}
			else if (options[i] == "checkbox-end") {
				endOfCheckbox = true;
				inCheckbox = false;
				outfile << "</div>" << endl;
				i--;
			}
			else if (options[i] == "checkbox") {
				std::vector<string> labels = SplitString(options[i + 1], ",");
				outfile << "<span>" << labels[0] << ":</span><br>" << endl;
				for (int j = 1; j < labels.size(); j++) {
					outfile << "<input type='checkbox' name='var" << to_string(variableID) << "' value='" << labels[j] << "'>" << labels[j] << "<br>" << endl;
				}
				variableID++;
			}

			if (variableID - 1 < explanationText.size() && !endOfCheckbox)
				outfile << "<span class='right'>" << explanationText[variableID - 1] << "</span>" << endl;

		}

		NumberOfVariables = variableID;

		outfile << "<br>" << endl;
		outfile << "<button type='submit'>" << options[options.size() - 1] << "</button>" << endl;
		outfile << "</form>" << endl;

		if (namefield) {
			ifstream ifs(cstrpath + "/formscript.txt", ifstream::in);
			string str = "";
			while (!safeGetline(ifs, str).eof()) {
				outfile << str << endl;
			}
		}

		outfile << "</body></html>" << endl;
		outfile.close();

		// Adding css data
		ofstream css(cstrpath + "/form.css");

		css << ".left {" << endl;
		css << "text-align: left;" << endl;
		css << "}" << endl;
		css << ".right {" << endl;
		css << "float:right;" << endl;
		css << "}" << endl;

		for (int i = 1; i <= checkboxCount; i++) {
			css << "#div" << to_string(i) << " {display:none}" << endl;
			css << "#" << checkboxVariables[i - 1] << ":checked + #div" << to_string(i) << " {display:block;}" << endl;
		}
		css.close();
	}
	else {
		cout << "Generation Form file failed!" << endl;
	}
}

void FormGenerator::generateHTMLCheckFile(string path, std::vector<string> variables, string lang) {
	bool inCheckbox = false;

	ifstream ifs(cstrpath + "/check" + lang + ".txt", ifstream::in);
	std::vector<string> check_options = std::vector<string>(0);
	string str = "";
	while (!safeGetline(ifs, str).eof()) {
		check_options.push_back(str);
		str = "";
	}
	if (check_options.size() < 4) {
		check_options.push_back("");
		check_options.push_back("");
		check_options.push_back("");
		check_options.push_back("");
	}

	if (form_options.size() > 0) {
		// writing head of html file
		ofstream outfile(path);
		outfile << "<!DOCTYPE html><html>" << endl;
		outfile << "<head><meta http-equiv='content-type' content='text/html; charset=utf-8'><link rel='stylesheet' href='leaderboard.css'></head>" << endl;
		outfile << "<body style='background-color:yellowgreen;'>" << endl;
		outfile << "<h1>" << check_options[0] << "</h1>" << endl;
		outfile << "<h1>" << form_options[0] << "</h1>" << endl;

		// loping through all elements defined in options array
		int variableID = 0;
		for (int i = 1; i < form_options.size() - 1; i += 2) {
			// translation into html code:
			if (!inCheckbox) {
				outfile << "<br>" << endl;
				if (form_options[i] == "inputfield" || form_options[i] == "namefield") {
					outfile << "<span>" << form_options[i + 1] << ":</span><br>" << endl;
					outfile << "<p>" << variables[variableID] << "</p>" << endl;
				}
				else if (form_options[i] == "radio") {
					std::vector<string> labels = SplitString(form_options[i + 1], ",");
					outfile << "<span>" << labels[0] << "</span><br>" << endl;
					outfile << "<p>" << variables[variableID] << "</p>" << endl;
				}
				else if (form_options[i] == "textarea") {
					std::vector<string> elements = SplitString(form_options[i + 1], ",");
					outfile << "<span>" << elements[0] << ":</span><br>" << endl;
					outfile << "<textarea name='var' rows='10' cols='30' readonly >" << variables[variableID] << "</textarea></form><br>" << endl;
				}
				else if (form_options[i] == "number") {
					std::vector<string> elements = SplitString(form_options[i + 1], ",");
					outfile << "<span>" << elements[0] << ":</span><br>" << endl;
					outfile << "<p>" << variables[variableID] << "</p>" << endl;
				}
				else if (form_options[i] == "drop-down" || form_options[i] == "drop-down-from-TextFile") {
					std::vector<string> elements = SplitString(form_options[i + 1], ",");
					outfile << "<span>" << elements[0] << ":</span><br>" << endl;
					outfile << "<p>" << variables[variableID] << "</p>" << endl;
				}
				else if (form_options[i] == "checkbox-display") {
					outfile << "<span>" << form_options[i + 1] << ":</span><br>" << endl;
					if (variables[variableID] == "false") {
						variables[variableID] = "false";
						inCheckbox = true;
					}
					outfile << "<p>" << variables[variableID] << "</p>" << endl;
				}
				else if (form_options[i] == "checkbox") {
					std::vector<string> labels = SplitString(form_options[i + 1], ",");
					outfile << "<span>" << labels[0] << "</span><br>" << endl;
					outfile << "<p>" << variables[variableID] << "</p>" << endl;
				}
			}

			if (form_options[i] == "checkbox-end") {
				inCheckbox = false;
				variableID--;
				i--;
			}

			variableID++;
		}

		outfile << "<br>" << endl;
		outfile << "<p>" << check_options[1] << "</p>" << endl;
		outfile << "<form action='/confirm' method='post'><button type='submit'>" << check_options[2] << "</button></form>" << endl;
		outfile << "<form action=\'javascript:history.go(-1)\' method='get'><button type='submit'>" << check_options[3] << "</button></form>" << endl;
		outfile << "</body></html>" << endl;
		outfile.close();
	}
	else {
		cout << "Generation check file failed!" << endl;
	}
}

FormGenerator::FormGenerator(std::vector<string> options, std::vector<string> explanationText, string path) {
	FormGenerator::cstrpath = path;
	FormGenerator::namefieldPos = -1;

	form_options = options;
	generateHTMLFile(path + "/form.html", options, explanationText);
}

FormGenerator::FormGenerator(string pathToForm, string pathToFormExplanation, string path) {
	FormGenerator::cstrpath = path;
	FormGenerator::namefieldPos = -1;

	ifstream ifs(pathToForm, ifstream::in);
	cout << "Form file opened\n";

	form_options = std::vector<string>(0);
	string str = "";
	while (!safeGetline(ifs, str).eof()) {
		form_options.push_back(str);
	}

	ifstream expl(pathToFormExplanation, ifstream::in);
	cout << "Form Explanation file opened\n";

	explanationText = std::vector<string>(0);
	while (!safeGetline(expl, str).eof()) {
		explanationText.push_back(str);
	}

	generateHTMLFile(path + "/form.html", form_options, explanationText);

	cout << "Ready" << endl;
}
