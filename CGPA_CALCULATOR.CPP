
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <limits>
#include <cmath>
using namespace std;

const int MAX_COURSES = 50;
const int MAX_SEMESTERS = 8;
const int MAX_CREDIT_HOURS = 30;

// Function to safely read integer input
int readInt(const string& prompt, int minimum, int maximum) {

    while (true) {
        cout << prompt;

        string line;
        getline(cin, line);

        stringstream input(line);
        int value;
        char extra;

        if ((input >> value) && !(input >> extra) &&
            value >= minimum && value <= maximum) {

            return value;
        }

        cout << "Invalid input. Enter a whole number from "
             << minimum << " to " << maximum << ".\n";
    }
}

// Function to validate grade points
double readGradePoint(const string& prompt) {

    while (true) {
        cout << prompt;

        string line;
        getline(cin, line);

        stringstream input(line);
        double value;
        char extra;

        if ((input >> value) && !(input >> extra) &&
            isfinite(value) && value >= 0.0 && value <= 4.0) {

            return value;
        }

        cout << "Invalid grade point. Enter a number from "
             << "0.00 to 4.00.\n";
    }
}

// Function to validate course names
string readCourseName(const string& prompt) {

    while (true) {
        cout << prompt;

        string name;
        getline(cin, name);

        if (name.find_first_not_of(" \t\r\n") != string::npos) {
            return name;
        }

        cout << "Course name cannot be empty. Try again.\n";
    }
}

// Function to calculate GPA
double calculateGPA(double qualityPoints, int creditHours) {

    if (creditHours <= 0)
        return 0.0;

    return qualityPoints / creditHours;
}

// Function to display semester report
void displayReport(int semester, int courseCount,
                   const string names[],
                   const double grades[],
                   const int credits[],
                   double qualityPoints,
                   int totalCredits) {

    cout << "\n==============================================================\n";
    cout << "                    SEMESTER " << semester << " REPORT\n";
    cout << "==============================================================\n";

    cout << left << setw(24) << "Course"
         << right << setw(10) << "Credits"
         << setw(10) << "Grade"
         << setw(14) << "Points" << endl;

    cout << "--------------------------------------------------------------\n";

    for (int i = 0; i < courseCount; i++) {

        cout << left << setw(24) << names[i]
             << right << setw(10) << credits[i]
             << setw(10) << fixed << setprecision(2) << grades[i]
             << setw(14) << grades[i] * credits[i] << endl;
    }

    cout << "--------------------------------------------------------------\n";

    cout << "Total Credit Hours: " << totalCredits << endl;

    cout << "Total Quality Points: "
         << fixed << setprecision(2) << qualityPoints << endl;

    cout << "Semester GPA: "
         << fixed << setprecision(2)
         << calculateGPA(qualityPoints, totalCredits) << endl;

    cout << "==============================================================\n";
}

int main() {

    double overallQualityPoints = 0.0;
    int overallCreditHours = 0;

    cout << "==============================================================\n";
    cout << "             UNIVERSITY GPA & CGPA CALCULATOR\n";
    cout << "==============================================================\n";

    cout << "Assumption: 4.00 grading scale.\n";
    cout << "All entered courses count toward GPA.\n";

    // Input number of semesters
    int semesterCount = readInt(
        "Enter number of semesters (1-8): ",
        1, MAX_SEMESTERS
    );

    // Process each semester
    for (int s = 0; s < semesterCount; s++) {

        string names[MAX_COURSES];
        double grades[MAX_COURSES];
        int credits[MAX_COURSES];

        double semesterQualityPoints = 0.0;
        int semesterCreditHours = 0;

        cout << "\n-------------------- SEMESTER "
             << s + 1 << " --------------------\n";

        // Input number of courses
        int courseCount = readInt(
            "Enter number of courses (1-50): ",
            1, MAX_COURSES
        );

        // Input course details
        for (int i = 0; i < courseCount; i++) {

            cout << "\nCourse " << i + 1 << endl;

            names[i] = readCourseName("Enter course name: ");

            grades[i] = readGradePoint(
                "Enter grade point (0.00-4.00): "
            );

            credits[i] = readInt(
                "Enter credit hours (1-30): ",
                1, MAX_CREDIT_HOURS
            );

            // Calculate quality points
            semesterQualityPoints += grades[i] * credits[i];

            semesterCreditHours += credits[i];
        }

        // Add semester data to overall totals
        overallQualityPoints += semesterQualityPoints;
        overallCreditHours += semesterCreditHours;

        // Display semester report
        displayReport(
            s + 1,
            courseCount,
            names,
            grades,
            credits,
            semesterQualityPoints,
            semesterCreditHours
        );
    }

    // Calculate final CGPA
    double cgpa = calculateGPA(
        overallQualityPoints,
        overallCreditHours
    );

    // Display final academic report
    cout << "\n==============================================================\n";
    cout << "                    FINAL ACADEMIC REPORT\n";
    cout << "==============================================================\n";

    cout << "Total Semesters: " << semesterCount << endl;

    cout << "Total Credit Hours: "
         << overallCreditHours << endl;

    cout << "Total Quality Points: "
         << fixed << setprecision(2)
         << overallQualityPoints << endl;

    cout << "CUMULATIVE CGPA: "
         << fixed << setprecision(2)
         << cgpa << " / 4.00" << endl;

    cout << "==============================================================\n";

    cout << "Thank you for using the calculator!\n";

    return 0;
}