#include "httplib.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <cstdlib>
#include <cstdio>
using namespace std;
// ===============================
// TEXT PROCESSING FUNCTIONS
// ===============================
// Convert text to lowercase
string lowerCase(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });
    return text;
}
// Remove extra spaces
string trim(string text) {
    size_t start = text.find_first_not_of(" \t\n\r");

    if (start == string::npos)
        return "";

    size_t end = text.find_last_not_of(" \t\n\r");

    return text.substr(start, end - start + 1);
}
// Split skills separated by commas
vector<string> splitSkills(string input) {
    vector<string> skills;
    size_t start = 0;

    while (start < input.length()) {
        size_t comma = input.find(',', start);

        string skill = (comma == string::npos)
            ? input.substr(start)
            : input.substr(start, comma - start);

        skill = trim(skill);

        if (!skill.empty())
            skills.push_back(skill);

        if (comma == string::npos)
            break;

        start = comma + 1;
    }

    return skills;
}
// Escape special characters for JSON
string jsonEscape(string text) {
    string result;

    for (unsigned char c : text) {
        switch (c) {
            case '"':
                result += "\\\"";
                break;

            case '\\':
                result += "\\\\";
                break;

            case '\n':
                result += "\\n";
                break;

            case '\r':
                result += "\\r";
                break;

            case '\t':
                result += "\\t";
                break;

            default:
                if (c < 0x20) {
                    result += ' ';
                } else {
                    result += static_cast<char>(c);
                }
        }
    }

    return result;
}


// Join skills into one string
string joinSkills(const vector<string>& skills) {
    string result;

    for (size_t i = 0; i < skills.size(); i++) {
        if (i > 0)
            result += ", ";

        result += skills[i];
    }

    return result;
}
// ===============================
// RESUME DETAILS EXTRACTION
// ===============================
// Extract student name
string extractName(const string& text) {
    istringstream stream(text);
    string line;

    while (getline(stream, line)) {
        line = trim(line);

        if (!line.empty()) {
            return line;
        }
    }

    return "Not found";
}
// Extract email
string extractEmail(const string& text) {
    regex pattern(
        R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})"
    );

    smatch match;

    if (regex_search(text, match, pattern)) {
        return match.str();
    }

    return "Not found";
}
// Extract CGPA
string extractCGPA(const string& text) {
    regex pattern(
        R"((?:CGPA|GPA)\s*:\s*([0-9]+(?:\.[0-9]+)?(?:\s*/\s*[0-9]+)?))",
        regex_constants::icase
    );

    smatch match;

    if (regex_search(text, match, pattern)) {
        return trim(match[1].str());
    }

    return "Not found";
}
// ===============================
// CAREER ASSISTANT
// ===============================
string careerReply(
    string question,
    string name,
    string role,
    string skills,
    string missingSkills
) {
    string q = lowerCase(question);

    string student = trim(name).empty()
        ? "there" : trim(name);

    if (trim(role).empty())
        role = "your selected career";


    if (q.find("hello") != string::npos ||
        q.find("hi") != string::npos ||
        q.find("hey") != string::npos) {

        return "Hello " + student +
            "! I am your C++ Career Assistant. "
            "I can guide you about skills, career paths, interviews, "
            "resume improvement and learning plans.";
    }


    if (q.find("skill") != string::npos ||
        q.find("learn") != string::npos ||
        q.find("gap") != string::npos) {

        string answer = "For " + role +
            ", build strong fundamentals and practise them through small projects.";

        if (!trim(missingSkills).empty()) {
            answer += "\n\nBased on your latest analysis, focus on: " +
                trim(missingSkills) + ".";
        }

        if (!trim(skills).empty()) {
            answer += "\n\nYou have entered these skills: " +
                trim(skills) +
                ". Strengthen them with practical exercises.";
        } else {
            answer += "\n\nEnter your current skills in the Skills Analysis "
                "section and analyse your profile for more relevant guidance.";
        }

        answer += "\n\nSuggested order: fundamentals -> problem solving "
            "-> practical project -> GitHub documentation.";

        return answer;
    }


    if (q.find("career") != string::npos ||
        q.find("path") != string::npos ||
        q.find("job role") != string::npos ||
        q.find("which job") != string::npos) {

        string path;

        if (role == "Software Developer") {
            path = "Software Developer: programming fundamentals, data "
                "structures and algorithms, SQL, Git and projects.";
        }
        else if (role == "Web Developer") {
            path = "Web Developer: HTML, CSS, JavaScript, responsive design, "
                "basic backend concepts and practical projects.";
        }
        else if (role == "Data Analyst") {
            path = "Data Analyst: Excel, SQL, Python, data cleaning, "
                "visualisation and portfolio projects.";
        }
        else if (role == "Embedded Systems Engineer") {
            path = "Embedded Systems Engineer: C/C++, digital electronics, "
                "microcontrollers, embedded C and hardware interfacing.";
        }
        else if (role == "Electronics Engineer") {
            path = "Electronics Engineer: circuit fundamentals, analog and "
                "digital electronics, measurement tools and lab projects.";
        }
        else {
            path = "Explore software, web development, data analysis and "
                "electronics/embedded roles. Try small projects before choosing.";
        }

        return "Your selected target is " + role +
            ".\n\n" + path +
            "\n\nChoose one direction to focus on for 6-8 weeks, then build "
            "a project that demonstrates your skills.";
    }


    if (q.find("interview") != string::npos ||
        q.find("placement") != string::npos) {

        return "A practical interview preparation plan:\n"
            "1. Revise programming fundamentals and explain your code clearly.\n"
            "2. Practise arrays, strings, searching and sorting.\n"
            "3. Prepare SQL and basic computer science concepts for software roles.\n"
            "4. Practise a short self-introduction and project explanation.\n"
            "5. Take mock interviews and review your mistakes.\n\n"
            "For this project, explain the problem, C++ backend, HTML interface, "
            "skill-matching logic and current limitations.";
    }


    if (q.find("resume") != string::npos ||
        q.find("ats") != string::npos ||
        q.find("cv") != string::npos) {

        return "To improve your resume:\n"
            "1. Keep the layout clear and concise.\n"
            "2. Add relevant technical skills that you can explain.\n"
            "3. Describe projects using the problem, your contribution and result.\n"
            "4. Use simple headings and consistent formatting.\n"
            "5. Check spelling and tailor your resume to the role.\n\n"
            "The ATS-style score in this project is an educational estimate, "
            "not a score from a real recruitment system.";
    }


    if (q.find("project") != string::npos) {

        return "Your Smart Resume Analyzer demonstrates a student profile, "
            "C++ skill comparison, an educational ATS-style estimate, "
            "illustrative company suggestions and a learning roadmap. "
            "For your presentation, explain how the HTML page sends data "
            "to the C++ server and how the server returns JSON.";
    }


    if (q.find("cgpa") != string::npos ||
        q.find("gpa") != string::npos ||
        q.find("marks") != string::npos) {

        return "Enter your CGPA accurately. Employers may also assess coding "
            "ability, projects, communication and role-specific skills. "
            "Always check each employer's official eligibility criteria.";
    }


    if (q.find("roadmap") != string::npos ||
        q.find("plan") != string::npos ||
        q.find("schedule") != string::npos) {

        return "A simple 4-week starter plan:\n"
            "Week 1: revise the fundamentals of " + role + ".\n"
            "Week 2: learn one important tool or topic for this role.\n"
            "Week 3: build a small practical project.\n"
            "Week 4: test it, document it and practise explaining it.\n\n"
            "Spend time practising, not only watching tutorials.";
    }


    if (q.find("company") != string::npos) {

        return "The company names shown in your analysis are illustrative "
            "suggestions based on simple skill rules. They are not confirmed "
            "vacancies or guarantees of eligibility. Check official careers "
            "pages before applying.";
    }


    return "I can help with skills to learn, career paths, interview preparation, "
        "resume improvement, projects and learning roadmaps. Please ask a "
        "specific question about " + role + ".";
}
// ===============================
// MAIN SERVER
// ===============================
int main() {
    httplib::Server server;
    // ===============================
    // HOMEPAGE
    // ===============================
    auto homepage = [](const httplib::Request& req,
                       httplib::Response& res) {

        ifstream file("public/index.html");

        if (!file.is_open()) {
            res.status = 404;
            res.set_content(
                "Error: public/index.html not found.",
                "text/plain"
            );
            return;
        }

        string html(
            (istreambuf_iterator<char>(file)),
            istreambuf_iterator<char>()
        );

        res.set_content(html, "text/html");
    };


    server.Get("/", homepage);
    server.Get("/index.html", homepage);
    // ===============================
    // PDF RESUME UPLOAD
    // ===============================
    server.Post("/upload-resume",

        [](const httplib::Request& req,
           httplib::Response& res) {

        if (!req.form.has_file("resume")) {

            res.status = 400;

            res.set_content(
                "{\"error\":\"Please select a PDF resume.\"}",
                "application/json"
            );

            return;
        }
        const auto& file = req.form.get_file("resume");
        // Save uploaded PDF
        string filename = "resume_upload_temp.pdf";
        {
            ofstream output(filename, ios::binary);

            if (!output) {

                res.status = 500;

                res.set_content(
                    "{\"error\":\"Could not save PDF.\"}",
                    "application/json"
                );

                return;
            }

            output.write(
                file.content.data(),
                file.content.size()
            );
        }
        // Extract PDF text using Poppler
        int result = system(
            "pdftotext -layout resume_upload_temp.pdf resume_upload_temp.txt"
        );
        remove("resume_upload_temp.pdf");
        if (result != 0) {
            remove("resume_upload_temp.txt");
            res.status = 500;
            res.set_content(
                "{\"error\":\"PDF text extraction failed. Check Poppler installation.\"}",
                "application/json"
            );

            return;
        }
        // Read extracted text
        ifstream input("resume_upload_temp.txt");
        string content(
            (istreambuf_iterator<char>(input)),
            istreambuf_iterator<char>()
        );

        input.close();

        remove("resume_upload_temp.txt");


        if (content.empty()) {

            res.status = 400;

            res.set_content(
                "{\"error\":\"No readable text found in this PDF.\"}",
                "application/json"
            );

            return;
        }
        // ===============================
        // EXTRACT STUDENT DETAILS
        // ===============================

        string studentName = extractName(content);

        string email = extractEmail(content);

        string cgpa = extractCGPA(content);
        // ===============================
        // EXTRACT TECHNICAL SKILLS
        // ===============================
        vector<string> knownSkills = {

            "C++", "Python", "Java", "JavaScript",
            "HTML", "CSS", "SQL", "MySQL", "MongoDB",
            "Excel", "Matlab", "Arduino", "Embedded C",
            "Verilog", "VHDL", "Linux", "Git", "GitHub",
            "React", "Node.js", "communication",
            "leadership", "problem solving",
            "data structures", "OOPS",
            "machine learning", "data analysis",
            "Power BI", "Figma"
        };
        vector<string> foundSkills;

        string lowerContent = lowerCase(content);
        for (const string& skill : knownSkills) {

            if (lowerContent.find(lowerCase(skill))
                != string::npos) {

                foundSkills.push_back(skill);
            }
        }
        // ===============================
        // CREATE JSON RESPONSE
        // ===============================

        string json = "{";

        json += "\"name\":\"" +
            jsonEscape(studentName) + "\",";

        json += "\"email\":\"" +
            jsonEscape(email) + "\",";

        json += "\"cgpa\":\"" +
            jsonEscape(cgpa) + "\",";

        json += "\"skills\":\"" +
            jsonEscape(joinSkills(foundSkills)) + "\",";

        json += "\"extractedChars\":" +
            to_string(content.size());

        json += "}";


        res.set_content(json, "application/json");

    });
    // ===============================
    // SKILLS ANALYSIS
    // ===============================
    server.Post("/analyze",
        [](const httplib::Request& req,
           httplib::Response& res) {

        string yourInput =
            req.get_param_value("yourSkills");

        string jobInput =
            req.get_param_value("jobSkills");


        vector<string> yourSkills =
            splitSkills(yourInput);

        vector<string> jobSkills =
            splitSkills(jobInput);
        vector<string> matched;
        vector<string> missing;
        for (const string& jobSkill : jobSkills) {
            bool found = false;
            for (const string& yourSkill : yourSkills) {
                if (lowerCase(jobSkill) ==
                    lowerCase(yourSkill)) {
                    found = true;
                    break;
                }
            }
            if (found)
                matched.push_back(jobSkill);
            else
                missing.push_back(jobSkill);
        }
        double percentage = 0.0;
        if (!jobSkills.empty()) {
            percentage =
                100.0 * matched.size() / jobSkills.size();
        }
        string json =
            "{\"percentage\":" + to_string(percentage) +

            ",\"matched\":\"" +
            jsonEscape(joinSkills(matched)) +

            "\",\"missing\":\"" +
            jsonEscape(joinSkills(missing)) +

            "\",\"matchedCount\":" +
            to_string(matched.size()) +

            ",\"totalCount\":" +
            to_string(jobSkills.size()) + "}";
        res.set_content(json, "application/json");
    });
    // ===============================
    // CAREER ASSISTANT CHAT
    // ===============================
    server.Post("/chat",
        [](const httplib::Request& req,
           httplib::Response& res) {

        string question =
            req.get_param_value("question");

        string name =
            req.get_param_value("name");

        string role =
            req.get_param_value("role");

        string skills =
            req.get_param_value("skills");

        string missingSkills =
            req.get_param_value("missing");
        if (trim(question).empty()) {
            res.status = 400;
            res.set_content(
                "{\"reply\":\"Please enter a question.\"}",
                "application/json"
            );
            return;
        }
        string reply = careerReply(
            question,
            name,
            role,
            skills,
            missingSkills
        );
        string json =
            "{\"reply\":\"" +
            jsonEscape(reply) +
            "\"}";
        res.set_content(json, "application/json");
    });
    // ===============================
    // START SERVER
    // ===============================
    cout << "Smart Resume Analyzer is starting..." << endl;
    cout << "Website: http://localhost:8080" << endl;
    if (!server.listen("127.0.0.1", 8080)) {

        cout << "Could not start the server." << endl;

        cout << "Check whether port 8080 is already in use."
             << endl;

        return 1;
    }
    return 0;
}

