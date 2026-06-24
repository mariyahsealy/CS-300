//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Mariyah Sealy
// Course      : CS 300
// Description : ABCU Course Advising Program using Binary Search Tree
//============================================================================
// copied from previous projects so they all have same look

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

//============================================================================
// Data Structures
//============================================================================

// Structure to hold course information
struct Course {
    string courseNumber;   // unique course identifier 
    string courseTitle;    // full name of the course
    vector<string> prerequisites; // list of prerequisite course numbers
};

// Node structure for the Binary Search Tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Constructor initializes a node with a course and null children
    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

//============================================================================
// Binary Search Tree Class
//============================================================================

class BinarySearchTree {

private:
    Node* root; // pointer to the root node of the BST

    // Private recursive helper methods
    void insertNode(Node* node, Course course);
    void inOrderPrint(Node* node);
    Node* searchNode(Node* node, string courseNumber);
    void destroyTree(Node* node);

public:
    BinarySearchTree();
    ~BinarySearchTree();
    void Insert(Course course);
    void PrintAll();
    Course Search(string courseNumber);
};

// Constructor: initialize root to nullptr
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

// Destructor: recursively delete all nodes to free memory
BinarySearchTree::~BinarySearchTree() {
    destroyTree(root);
}

// Private helper: post order deletion of all nodes
void BinarySearchTree::destroyTree(Node* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

// Public Insert: starts the recursive insert from root
void BinarySearchTree::Insert(Course course) {
    if (root == nullptr) {
        // Tree is empty; new course becomes the root
        root = new Node(course);
    }
    else {
        // Delegate to recursive helper
        insertNode(root, course);
    }
}

// Private recursive Insert: places course in correct BST position
// BST ordering is alphanumeric by courseNumber (left = smaller, right = larger)
void BinarySearchTree::insertNode(Node* node, Course course) {
    if (course.courseNumber < node->course.courseNumber) {
        // Course belongs in the left subtree
        if (node->left == nullptr) {
            node->left = new Node(course); // empty spot found
        }
        else {
            insertNode(node->left, course); // keep searching left
        }
    }
    else {
        // Course belongs in the right subtree
        if (node->right == nullptr) {
            node->right = new Node(course); // empty spot found
        }
        else {
            insertNode(node->right, course); // keep searching right
        }
    }
}

// Public PrintAll: triggers in order traversal from root
// In-order traversal of a BST always visits nodes in sorted order
void BinarySearchTree::PrintAll() {
    inOrderPrint(root);
}

// Private recursive in order traversal: left -> current -> right
// This prints all courses in alphanumeric order by course number
void BinarySearchTree::inOrderPrint(Node* node) {
    if (node == nullptr) {
        return; // base case: reached a leaf's null child
    }
    inOrderPrint(node->left);  // visit left subtree first
    // Print course number and title for each node visited
    cout << node->course.courseNumber << ", "
        << node->course.courseTitle << endl;
    inOrderPrint(node->right); // then visit right subtree
}

// Public Search: finds and returns a course by course number
Course BinarySearchTree::Search(string courseNumber) {
    // Convert search input to uppercase for case insensitive matching
    transform(courseNumber.begin(), courseNumber.end(),
        courseNumber.begin(), ::toupper);

    Node* result = searchNode(root, courseNumber);
    if (result != nullptr) {
        return result->course;
    }
    // Return empty course if not found (caller checks courseNumber.empty())
    return Course();
}

// Private recursive search: traverses BST to find matching course number
Node* BinarySearchTree::searchNode(Node* node, string courseNumber) {
    if (node == nullptr) {
        return nullptr; // reached end without finding a match
    }
    if (node->course.courseNumber == courseNumber) {
        return node; // found the matching node
    }
    if (courseNumber < node->course.courseNumber) {
        return searchNode(node->left, courseNumber);  // search left
    }
    else {
        return searchNode(node->right, courseNumber); // search right
    }
}

//============================================================================
// Helper Functions
//============================================================================

// Trim leading and trailing whitespace from a string
// Needed to clean up tokens read from the CSV file
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return str.substr(start, end - start + 1);
}

// Convert a string to uppercase so it can be used for case-insensitive comparisons
string toUpper(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

//============================================================================
// File Loading Function
//============================================================================

/**
 * LoadCourses: reads a CSV file and loads course data into the BST.
 *
 * The file format expected is:
 *   CourseNumber,CourseTitle[,Prerequisite1,Prerequisite2,...]
 *
 * Two passes are made:
 *   Pass 1: collect all valid course numbers for prerequisite validation
 *   Pass 2: build Course objects and insert them into the BST
 *
 * @param fileName  path to the CSV data file
 * @param bst       pointer to the BinarySearchTree to load into
 * @return true if loading succeeded, false otherwise
 */
bool LoadCourses(string fileName, BinarySearchTree* bst) {
    ifstream file(fileName);

    // Verify the file opened successfully
    if (!file.is_open()) {
        cout << "Error: Could not open file '" << fileName << "'. "
            << "Please check the file name and try again." << endl;
        return false;
    }

    // PASS 1: Collect all course numbers for prerequisite validation
    vector<string> allCourseNumbers;
    vector<string> rawLines;
    string line;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue; // skip blank lines

        stringstream ss(line);
        string token;
        vector<string> tokens;

        // Split line by comma into tokens
        while (getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }

        // Each valid line must have at least a course number and a title
        if (tokens.size() < 2) {
            cout << "Warning: Skipping malformed line (fewer than 2 fields): "
                << line << endl;
            continue;
        }

        allCourseNumbers.push_back(toUpper(tokens[0]));
        rawLines.push_back(line);
    }
    file.close();

    if (rawLines.empty()) {
        cout << "Error: No valid course data found in the file." << endl;
        return false;
    }

    // PASS 2: Build Course objects and insert into BST 
    int loadedCount = 0;

    for (const string& rawLine : rawLines) {
        stringstream ss(rawLine);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(trim(token));
        }

        Course course;
        course.courseNumber = toUpper(tokens[0]); // normalize to uppercase
        course.courseTitle = tokens[1];

        // Process any prerequisites listed after the title (index 2 onward)
        for (size_t i = 2; i < tokens.size(); ++i) {
            string prereq = toUpper(tokens[i]);
            if (prereq.empty()) continue;

            // Validate: prerequisite must exist as a course in the file
            if (find(allCourseNumbers.begin(), allCourseNumbers.end(), prereq)
                == allCourseNumbers.end()) {
                cout << "Warning: Prerequisite '" << prereq
                    << "' for course '" << course.courseNumber
                    << "' was not found in the course list." << endl;
            }
            course.prerequisites.push_back(prereq);
        }

        bst->Insert(course);
        loadedCount++;
    }

    cout << "Success: " << loadedCount
        << " courses loaded into the data structure." << endl;
    return true;
}

//============================================================================
// Display Functions
//============================================================================

// Print the full alphanumeric course list using BST in-order traversal
void PrintCourseList(BinarySearchTree* bst) {
    cout << endl;
    cout << "Here is a sample schedule:" << endl;
    cout << endl;
    bst->PrintAll();
    cout << endl;
}

// Look up a single course and print its title and prerequisites
void PrintCourseInformation(BinarySearchTree* bst) {
    string courseNumber;
    cout << "What course do you want to know about? ";
    cin >> courseNumber;
    courseNumber = toUpper(courseNumber); // normalize input

    Course course = bst->Search(courseNumber);

    if (course.courseNumber.empty()) {
        // Course was not found in the BST
        cout << "Error: Course '" << courseNumber
            << "' not found. Please check the course number and try again."
            << endl;
        return;
    }

    // Print course number and title
    cout << endl;
    cout << course.courseNumber << ", " << course.courseTitle << endl;

    // Print prerequisites, or None if there are no prerequisites
    cout << "Prerequisites: ";
    if (course.prerequisites.empty()) {
        cout << "None" << endl;
    }
    else {
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << course.prerequisites[i];
        }
        cout << endl;
    }
    cout << endl;
}

//============================================================================
// Main Program
//============================================================================

int main() {

    // Create the Binary Search Tree to hold all course data
    BinarySearchTree* bst = new BinarySearchTree();

    bool dataLoaded = false; // tracks whether courses have been loaded
    int choice = 0;

    cout << "Welcome to the ABCU Course Advising Program." << endl;
    cout << endl;

    // Main menu loop runs until user selects 9 to exit
    while (choice != 9) {

        // Display the menu options
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << endl;
        cout << "What would you like to do? ";

        // Input validation: handle non-integer input gracefully
        if (!(cin >> choice)) {
            cin.clear();                                   // clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard bad input
            cout << endl;
            cout << "Invalid input. Please enter a number (1, 2, 3, or 9)."
                << endl << endl;
            continue;
        }

        cout << endl;

        switch (choice) {

        case 1: {
            // Option 1: Prompt for file name and load courses into BST
            string fileName;
            cout << "Enter the file name that contains the course data: ";
            cin >> fileName;
            cout << endl;

            // Attempt to load; track success so other options can check
            dataLoaded = LoadCourses(fileName, bst);
            cout << endl;
            break;
        }

        case 2: {
            // Option 2: Print all courses in alphanumeric order
            // Guard: data must be loaded first
            if (!dataLoaded) {
                cout << "Please load the data structure first (Option 1)."
                    << endl << endl;
                break;
            }
            PrintCourseList(bst);
            break;
        }

        case 3: {
            // Option 3: Search for and print a specific course
            // Guard: data must be loaded first
            if (!dataLoaded) {
                cout << "Please load the data structure first (Option 1)."
                    << endl << endl;
                break;
            }
            PrintCourseInformation(bst);
            break;
        }

        case 9: {
            // Option 9: Exit the program
            cout << "Thank you for using the ABCU Advising Program. "
                << "Good bye!" << endl;
            break;
        }

        default: {
            // Handle any menu choice that is not 1, 2, 3, or 9
            cout << choice << " is not a valid option. "
                << "Please enter 1, 2, 3, or 9." << endl << endl;
            break;
        }
        }
    }

    // Clean up dynamically allocated BST before exiting
    delete bst;
    return 0;
}
