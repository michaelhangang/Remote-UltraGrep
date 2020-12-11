// UltraGrep.cpp : This file contains the 'main' function. Program execution begins and ends there.
//Author : Gang Han
//Data: 2020 11.13
#pragma once
#include "Win32Thread.hpp"
#include "Adapter.cpp"


class UltraGrep {
    // Check if the folder exists.
    void isFolderExist(path const& folder) {
        if (!exists(folder)) {
            throw exception("The folder doesn't exist!");
        }
    }

    // Procee the extention list and store in adapter vector
    vector<string> processExt(string const& ext) {
        istringstream iss(ext);
        string line;
        vector<string> exts;
        while (getline(iss, line, '.')) {
            if (line != "") {
                exts.push_back("." + line);
            }
        }
        return exts;
    }

    vector<string> processCommend(string const& command) {
        istringstream iss(command);
        string line;
        vector<string> commands;
        while (getline(iss, line, ' ')) {
            if (line != "") {
                auto trimmedString = trim(line);
                commands.push_back(trimmedString);
            }
        }
       /* if (commands.size() != 3)
            throw exception("Missing commands.");*/
      
        return commands;
    }

    void scan_files(path const& f, string& expr, map <path, vector<string>>& report, short& totalMatches, vector<string> const& exts, Adapter& adapter, bool v = false) {
        cout << '\n';
        for (auto& d : recursive_directory_iterator(f)) {
            if (v)
                adapter.print_scanning(d);
            if (!is_directory(d.status())) {
                for (auto& ext : exts) {
                    if (d.path().extension() == ext) {
                        adapter.push_task(Task(d, expr, report, totalMatches, v));
                        adapter.wake_one();
                    }
                }
            }
        }
    }
    const string WHITESPACE = " \n\r\t\f\v";
    // Trim leading  whitespace
    string ltrim(const string& s)
    {
        size_t start = s.find_first_not_of(WHITESPACE);
        return (start == string::npos) ? "" : s.substr(start);
    }

    // Trim trailing whitespace
    string rtrim(const string& s)
    {
        size_t end = s.find_last_not_of(WHITESPACE);
        return (end == string::npos) ? "" : s.substr(0, end + 1);
    }

    // Trim leading and trailing whitespace
    string trim(const string& s)
    {
        return rtrim(ltrim(s));
    }
public:
    string  process(string const& msg)
    {
        Threading::pointer_type cppThread (new CppThread());
        //Threading::pointer_type cppThread(new Win32Thread());
        Adapter adapter(*cppThread);
       
        //  processCommend(command);
        adapter.create_thread();

        path folder = "";
        locale loc;
        map <path, vector<string>> report;
        short totalMatches = 0;

        LARGE_INTEGER singleStart, singleStop, frequency;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&singleStart);

        auto commands = processCommend(msg);
        for (string::size_type i = 0; i < commands[0].length(); ++i)
            commands[0][i] = tolower(commands[0][i], loc);
            
        folder = commands[0];
        isFolderExist(folder);
        auto exts = processExt(commands[2]);
        scan_files(folder, commands[1], report, totalMatches, exts, adapter);
        // Wake all the sleeping threads 
        adapter.wake_all();  
        // Clean up the threads
        adapter.clean();       
        QueryPerformanceCounter(&singleStop);
        double singleElapsed = (singleStop.QuadPart - singleStart.QuadPart) / double(frequency.QuadPart);
        ostringstream oss;    
        // Report         
        if (report.size() > 0) {   
            oss << '\n';        
            oss << "Grep Report:\n\n";          
            for (auto& item : report) {        
                oss << item.first << '\n';                
                for (auto& m : item.second) {               
                    oss << m << '\n';                   
                }
                    oss << "\n\n";
                }               
            oss << "Files with matches = " << to_string(report.size()) <<"\n";               
            oss << "Total number of matches = " << to_string(totalMatches) <<'\n';          
            oss << "Scan Completed in " << singleElapsed << 's' << endl;
        }
        return oss.str();
    }     
};