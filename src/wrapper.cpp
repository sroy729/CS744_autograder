#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;
int main(int argc, char* argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <executable>" << endl;
        return 1;
    }

    // Set the name of the executable to run
    string executable = argv[1];

    // Set the name of the file to capture the error
    string errorFileName = "output/runError.txt";

    // Run the executable and redirect stderr to the error file
    string command = executable + " 2> " + errorFileName;
    int exitCode = system(command.c_str());

    // Check if the execution was successful
    if (exitCode == 0) {
        // cout << "Execution successful!" << endl;
    } else {
        // cerr << "Execution failed with exit code: " << exitCode << endl;

        // Read and print the error from the error file
        ifstream errorFile(errorFileName);
        // if (errorFile.is_open()) {
            // cout << "Error details:" << endl;
            // cout << errorFile.rdbuf();
        errorFile.close();
        // } else {
        //     cerr << "Unable to open the error file." << endl;
        // }
    }

    return 0;
}

