#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>

using namespace std;

// Function to get current date in (YYYY-MM-DD) format
string getCurrentDate()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
       << setfill('0') << setw(2) << ltm->tm_mday;
    return ss.str();
}

// Converts string date to tm structure
tm stringToTm(const string &dateStr)
{
    tm date = {};
    int year, month, day;

    stringstream ss(dateStr);
    char delimiter;

    if (ss >> year >> delimiter >> month >> delimiter >> day)
    {
        date.tm_year = year - 1900; // tm_year is the number of years since 1900
        date.tm_mon = month - 1;    // tm_mon is 0 based (0-11)
        date.tm_mday = day;
    }

    return date;
}

// Calculate overdue days between two dates
int calculateOverdueDays(const string &borrowDateStr, const string &returnDateStr)
{
    tm borrowDate = stringToTm(borrowDateStr);
    tm returnDate = stringToTm(returnDateStr);

    time_t borrowTime = mktime(&borrowDate);
    time_t returnTime = mktime(&returnDate);

    double difference = difftime(returnTime, borrowTime);
    int differenceInDays = difference / (60 * 60 * 24);

    int allowedBorrowingPeriod = 7; // 7 days
    if (differenceInDays > allowedBorrowingPeriod)
    {
        return differenceInDays - allowedBorrowingPeriod; // Days overdue
    }
    else
    {
        return 0; // No overdue
    }
}

struct Member
{
    string name;
    string id;
    string gmail;
};

struct Book
{
    string id;
    string name;
    string pageCount;
    string authorFirstName;
    string authorLastName;
    string bookType;
    string status;     // "Available" or "Borrowed"
    string borrowDate; // Date when the book was borrowed
};

struct Librarian
{
    string id;
    string name;
    string address;
    string email;
    int salary;
};

vector<Member> members;
Member loggedInMember;

vector<Book> books;

vector<Librarian> librarians;
Librarian loggedInLibrarian;

// Displays menu options
int libraryOptions()
{
    cout << "1. Admin login" << endl;
    cout << "2. Details Of All Library Staff" << endl;
    cout << "3. Member Registration" << endl;
    cout << "4. Member Login" << endl;
    cout << "5. Show All Books" << endl;
    cout << "6. Search Book" << endl;
    cout << "7. Borrow Book" << endl;
    cout << "8. Return Book" << endl;
    cout << "9. Exit" << endl;
    cout << "--- Choose any one option ---" << endl;
    cout << "Select one option: ";

    int selectedOption;
    cin >> selectedOption;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return selectedOption;
}

// Load books from a CSV file
void loadBooksFromCSV()
{
    string filePath = "books.csv";
    ifstream inFile(filePath);
    string line;

    while (getline(inFile, line))
    {
        stringstream ss(line);
        Book book;
        getline(ss, book.id, ',');
        getline(ss, book.name, ',');
        getline(ss, book.pageCount, ',');
        getline(ss, book.authorFirstName, ',');
        getline(ss, book.authorLastName, ',');
        getline(ss, book.bookType, ',');
        book.status = "Available";

        books.push_back(book);
    }

    inFile.close();
}

// Save books to a CSV file
void saveBooksToCSV()
{
    string filePath = "books.csv";
    ofstream outFile(filePath);

    if (!outFile.is_open())
    {
        cerr << "Error opening file to save data." << endl;
        return;
    }

    for (const auto &book : books)
    {
        outFile << book.id << "," << book.name << "," << book.pageCount << ","
                << book.authorFirstName << "," << book.authorLastName << ","
                << book.bookType << "," << book.status << endl;
    }

    outFile.close();
}

// Login for a staff
void librarianLogin()
{
    Librarian librarian;
    cout << "Enter Admin ID: ";
    getline(cin, librarian.id);
    cout << "Enter Admin Name: ";
    getline(cin, librarian.name);
    cout << "Enter Admin Email: ";
    getline(cin, librarian.email);
    cout << "Enter Admin Salary: ";

    while (!(cin >> librarian.salary))
    {
        cin.clear();                                         // Clear error state
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard bad input
        cout << "Invalid input. Please enter a numeric value for salary: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the remainder of the line

    librarians.push_back(librarian);
    loggedInLibrarian = librarian;
    cout << "Staff Logged-in successfully." << endl;
}

// Display all librarians
void showLibrarians()
{
    if (loggedInLibrarian.id.empty())
    {
        cout << "To view your details please log in as a member of a staff." << endl;
        return;
    }

    cout << "--- Details of Admin ---" << endl;
    for (const auto &_ : librarians)
    {
        static_cast<void>(_); // Suppress compiler warning about unused variable or expression
        cout << "ID: " << loggedInLibrarian.id << ", Name: " << loggedInLibrarian.name
             << ", Address: " << loggedInLibrarian.address << ", Email: " << loggedInLibrarian.email
             << ", Salary: " << loggedInLibrarian.salary << endl;
    }
}

// Registration for members
void memberRegistration()
{
    Member member;
    cout << "Enter Your Name: ";
    getline(cin, member.name);
    cout << "Enter Your ID: ";
    getline(cin, member.id);
    cout << "Enter Your Email: ";
    getline(cin, member.gmail);
    members.push_back(member);
    cout << "Member added successfully." << endl;
}

// Member Login
void memberLogin()
{
    string id;
    cout << "Enter Your ID: ";
    getline(cin, id);

    for (const auto &member : members)
    {
        if (member.id == id)
        {
            loggedInMember = member;
            cout << "Login successful. Welcome, " << loggedInMember.name << "!" << endl;
            return;
        }
    }

    cout << "Invalid ID. Please try again." << endl;
    loggedInMember = {}; // Clear the logged-in member if login fails
}

// Display all books
void showBooks()
{
    cout << "--- List of all books ---" << endl;
    for (const auto &book : books)
    {
        cout << "ID: " << book.id << ", Name: " << book.name << ", Page Count: " << book.pageCount
             << ", Author: " << book.authorFirstName << " " << book.authorLastName
             << ", Type: " << book.bookType << ", Status: " << book.status << endl;
    }
}

// Search for a book
void searchBook()
{
    string id;
    cout << "Enter Book's ID to search: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    bool found = false;
    for (const auto &book : books)
    {
        if (book.id == id)
        {
            cout << "Book found:" << endl;
            cout << "ID: " << book.id << ", Name: " << book.name << ", Page Count: " << book.pageCount
                 << ", Author: " << book.authorFirstName << " " << book.authorLastName
                 << ", Type: " << book.bookType << ", Status: " << book.status << endl;
            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "No book with such Id has been found" << id << endl;
    }
}

// Borrow a book
void borrowBook()
{
    if (loggedInMember.name.empty())
    {
        cout << "Please log in your details." << endl;
        return;
    }

    cout << "--- Borrow a Book ---" << endl;
    string id, borrowDate;
    cout << "Enter Book ID: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Borrow Date (YYYY-MM-DD): ";
    getline(cin, borrowDate);

    bool found = false;
    for (auto &book : books)
    {
        if (book.id == id)
        {
            found = true;
            if (book.status == "Available")
            {
                book.status = "Borrowed";
                book.borrowDate = borrowDate;
                cout << "Book borrowed successfully on " << borrowDate << "." << endl;
            }
            else
            {
                cout << "Book is already borrowed." << endl;
            }
            break;
        }
    }

    if (!found)
    {
        cout << "No book's with such ID has been found" << id << endl;
    }
}

// Return a book
void returnBook()
{
    cout << "--- Return a Book ---" << endl;
    string id, returnDate;
    cout << "Enter Book ID to return: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter Return Date (YYYY-MM-DD): ";
    getline(cin, returnDate);

    bool found = false;
    for (auto &book : books)
    {
        if (book.id == id && book.status == "Borrowed")
        {
            found = true;
            int overdueDays = calculateOverdueDays(book.borrowDate, returnDate);

            if (overdueDays > 0)
            {
                int charges = overdueDays; // £1 per day
                cout << "Book is overdue by " << overdueDays << " days. Overdue charges: " << charges << " pounds." << endl;
            }
            else
            {
                cout << "Book is returned on time. No overdue charges." << endl;
            }

            book.status = "Available";
            book.borrowDate = ""; // Clear the borrow date
            cout << "Book returned successfully on " << returnDate << "." << endl;
            break;
        }
    }

    if (!found)
    {
        cout << "No borrowed book found with such ID " << id << endl;
    }
}

// Main home menu
void home()
{
    int option = libraryOptions();

    switch (option)
    {
    case 1:
        librarianLogin();
        break;
    case 2:
        showLibrarians();
        break;
    case 3:
        memberRegistration();
        break;
    case 4:
        memberLogin();
        break;
    case 5:
        showBooks();
        break;
    case 6:
        searchBook(); // Handles the book search option
        break;
    case 7:
        borrowBook();
        break;
    case 8:
        returnBook();
        break;
    case 9:
        cout << "Exit the program." << endl;
        exit(1);
    default:
        cout << "Invalid option. Please try again." << endl;
    }
}

// Main function
int main()
{
    cout << "* WELCOME *" << endl;
    loadBooksFromCSV();

    while (true)
    {
        cout << endl
             << "******** Library Management System ********" << endl;
        home();

        cout << endl
             << "Continue? (y/n): ";
        string yn;
        cin >> yn;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (yn != "y")
            break;
    }

    saveBooksToCSV(); // Save before exiting
    return 0;
}