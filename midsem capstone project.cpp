#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> // for replace
#include <cstring>   // for strcpy, strcmp
#include <cstdlib>   // for system("cls") on Windows

using namespace std;

const int MAX_STUDENTS = 500;
const int MAX_ATTENDANCE = 500;

struct Student
{
    char index[15];
    char name[60];
};

struct Attendance
{
    char index[15];
    char status; // P / A / L
};

Student students[MAX_STUDENTS];
int num_students = 0;

Attendance current_attendance[MAX_ATTENDANCE];
int num_present = 0;

string current_session = "";

void clear_screen()
{
    system("cls");
}

void pause()
{
    cout << "\nPress Enter to continue...";
    cin.get();
    cin.get();
}

void load_students()
{
    ifstream file("students.txt");
    if (!file)
        return;

    num_students = 0;
    while (file && num_students < MAX_STUDENTS)
    {
        Student &s = students[num_students];
        file.getline(s.index, 15, ',');
        file.getline(s.name, 60);
        if (strlen(s.index) > 0)
            num_students++;
    }
    file.close();
}

void save_students()
{
    ofstream file("students.txt");
    if (!file)
    {
        cout << "Cannot save students!\n";
        return;
    }
    for (int i = 0; i < num_students; i++)
    {
        file << students[i].index << "," << students[i].name << "\n";
    }
    file.close();
}

void register_student()
{
    if (num_students >= MAX_STUDENTS)
    {
        cout << "Maximum students reached!\n";
        return;
    }

    Student &s = students[num_students];
    cout << "Enter index number (e.g. EE20001): ";
    cin >> s.index;
    cin.ignore();

    cout << "Enter full name: ";
    cin.getline(s.name, 60);

    num_students++;
    save_students();
    cout << "\nStudent added.\n";
}

void view_students()
{
    if (num_students == 0)
    {
        cout << "No students registered yet.\n";
        return;
    }

    cout << "\nRegistered students:\n";
    cout << "------------------------\n";
    for (int i = 0; i < num_students; i++)
    {
        cout << students[i].index << "  -  " << students[i].name << "\n";
    }
    cout << "------------------------\n";
}

void search_student()
{
    char idx[15];
    cout << "Enter index number to search: ";
    cin >> idx;
    cin.ignore();

    for (int i = 0; i < num_students; i++)
    {
        if (strcmp(students[i].index, idx) == 0)
        {
            cout << "\nFound:\n";
            cout << "Index: " << students[i].index << "\n";
            cout << "Name : " << students[i].name << "\n";
            return;
        }
    }
    cout << "Student not found.\n";
}

bool student_exists(const char *idx)
{
    for (int i = 0; i < num_students; i++)
    {
        if (strcmp(students[i].index, idx) == 0)
            return true;
    }
    return false;
}

void create_session()
{
    char course[20], date[12], time[10];
    int hours;

    cout << "Course code (e.g. EEE227): ";
    cin >> course;
    cin.ignore();

    cout << "Date (YYYY-MM-DD): ";
    cin.getline(date, 12);

    cout << "Start time (HH:MM): ";
    cin.getline(time, 10);

    cout << "Duration (hours): ";
    cin >> hours;
    cin.ignore();

    current_session = string(course) + "_" + date + "_" + time + ".txt";
    replace(current_session.begin(), current_session.end(), ':', '-');

    num_present = 0;
    cout << "\nNew session created: " << current_session << "\n";
}

void mark_attendance()
{
    if (current_session.empty())
    {
        cout << "No session created yet. Create one first.\n";
        return;
    }

    cout << "\nMark attendance for session: " << current_session << "\n";
    cout << "Enter student index (or 'done' to finish)\n";

    while (true)
    {
        char idx[15];
        cout << "Index: ";
        cin >> idx;
        cin.ignore();

        string input = idx;
        if (input == "done" || input == "exit")
            break;

        if (!student_exists(idx))
        {
            cout << "Student not found.\n";
            continue;
        }

        char status;
        cout << "Status (P=Present, A=Absent, L=Late): ";
        cin >> status;
        cin.ignore();

        status = toupper(status);
        if (status != 'P' && status != 'A' && status != 'L')
        {
            cout << "Invalid status.\n";
            continue;
        }

        strcpy(current_attendance[num_present].index, idx);
        current_attendance[num_present].status = status;
        num_present++;

        cout << "Recorded.\n";
    }

    // save
    ofstream file("data/sessions/" + current_session);
    if (!file)
    {
        cout << "Cannot save session!\n";
        return;
    }

    for (int i = 0; i < num_present; i++)
    {
        file << current_attendance[i].index << "," << current_attendance[i].status << "\n";
    }
    file.close();

    cout << "Session saved.\n";
}

void view_attendance()
{
    if (current_session.empty())
    {
        cout << "No session active.\n";
        return;
    }

    ifstream file("data/sessions/" + current_session);
    if (!file)
    {
        cout << "Session file not found.\n";
        return;
    }

    cout << "\nAttendance for session: " << current_session << "\n";
    cout << "------------------------------------\n";

    int p = 0, a = 0, l = 0;
    char line[100];
    while (file.getline(line, 100))
    {
        char *idx = strtok(line, ",");
        char *stat = strtok(NULL, ",");
        if (!idx || !stat)
            continue;

        cout << idx << "  ->  " << stat << "\n";

        if (stat[0] == 'P')
            p++;
        else if (stat[0] == 'A')
            a++;
        else if (stat[0] == 'L')
            l++;
    }
    file.close();

    cout << "\nSummary:\n";
    cout << "Present : " << p << "\n";
    cout << "Absent  : " << a << "\n";
    cout << "Late    : " << l << "\n";
    cout << "Total   : " << (p + a + l) << "\n";
}

int main()
{
    // create folders if needed
    system("if not exist data\\sessions mkdir data\\sessions");

    load_students();

    int choice;
    do
    {
        clear_screen();
        cout << "====================================\n";
        cout << "   DIGITAL ATTENDANCE SYSTEM\n";
        cout << "====================================\n";
        cout << "1. Register new student\n";
        cout << "2. View all students\n";
        cout << "3. Search student\n";
        cout << "4. Create new lecture session\n";
        cout << "5. Mark attendance\n";
        cout << "6. View current attendance\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        clear_screen();

        if (choice == 1)
            register_student();
        else if (choice == 2)
            view_students();
        else if (choice == 3)
            search_student();
        else if (choice == 4)
            create_session();
        else if (choice == 5)
            mark_attendance();
        else if (choice == 6)
            view_attendance();
        else if (choice == 0)
            cout << "Goodbye!\n";
        else
            cout << "Wrong choice.\n";

        if (choice != 0)
            pause();

    } while (choice != 0);

    return 0;
}