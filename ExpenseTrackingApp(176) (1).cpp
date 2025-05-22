#include <iostream>
#include <cstdlib>

using namespace std;

// Linked list node to hold each field of an expense
struct FieldNode {
    string label;
    string info;
    FieldNode* nextField;

    FieldNode(string l, string i) {
        label = l;
        info = i;
        nextField = nullptr;
    }
};

// Doubly linked list node to hold each complete expense entry
struct RecordNode {
    string expenseID;
    FieldNode* fieldList;
    RecordNode* previous;
    RecordNode* following;

    RecordNode(string id) {
        expenseID = id;
        fieldList = nullptr;
        previous = following = nullptr;
    }
};

class BudgetManager {
private:
    RecordNode* start;
    RecordNode* end;
    int idCounter;

    double convertToDouble(const string& val) {
        double num = 0.0, fraction = 1.0;
        bool decimal = false;
        for (char ch : val) {
            if (ch == '.') {
                decimal = true;
                continue;
            }
            if (isdigit(ch)) {
                if (decimal) {
                    fraction *= 0.1;
                    num += (ch - '0') * fraction;
                } else {
                    num = num * 10 + (ch - '0');
                }
            }
        }
        return num;
    }

    string numberToString(int number) {
        if (number == 0) return "0";
        string str;
        bool neg = number < 0;
        if (neg) number = -number;
        while (number) {
            str = char('0' + number % 10) + str;
            number /= 10;
        }
        return neg ? "-" + str : str;
    }

public:
    BudgetManager() : start(nullptr), end(nullptr), idCounter(1) {}

    ~BudgetManager() {
        RecordNode* current = start;
        while (current) {
            RecordNode* toDelete = current;
            current = current->following;
            FieldNode* field = toDelete->fieldList;
            while (field) {
                FieldNode* temp = field;
                field = field->nextField;
                delete temp;
            }
            delete toDelete;
        }
    }

    void insertExpense() {
        string uniqueID = "E-" + numberToString(idCounter++);
        RecordNode* newRecord = new RecordNode(uniqueID);

        string item, type, cost, timestamp;
        cout << "Enter Item Name: "; getline(cin, item);
        cout << "Enter Category: "; getline(cin, type);
        cout << "Enter Price: "; getline(cin, cost);
        cout << "Enter Date (DD-MM-YYYY): "; getline(cin, timestamp);

        newRecord->fieldList = new FieldNode("ID", uniqueID);
        newRecord->fieldList->nextField = new FieldNode("Name", item);
        newRecord->fieldList->nextField->nextField = new FieldNode("Category", type);
        newRecord->fieldList->nextField->nextField->nextField = new FieldNode("Price", cost);
        newRecord->fieldList->nextField->nextField->nextField->nextField = new FieldNode("Date", timestamp);

        if (!start) start = end = newRecord;
        else {
            end->following = newRecord;
            newRecord->previous = end;
            end = newRecord;
        }

        cout << "\nExpense Added! ID: " << uniqueID << endl;
    }

    void updateExpense() {
        string id;
        cout << "Enter Expense ID to Modify: ";
        getline(cin, id);

        RecordNode* node = start;
        while (node && node->expenseID != id) node = node->following;

        if (!node) {
            cout << "No record found with ID: " << id << endl;
            return;
        }

        FieldNode* toClear = node->fieldList->nextField;
        while (toClear) {
            FieldNode* temp = toClear;
            toClear = toClear->nextField;
            delete temp;
        }

        string item, type, cost, timestamp;
        cout << "Enter New Item Name: "; getline(cin, item);
        cout << "Enter New Category: "; getline(cin, type);
        cout << "Enter New Price: "; getline(cin, cost);
        cout << "Enter New Date (DD-MM-YYYY): "; getline(cin, timestamp);

        node->fieldList->nextField = new FieldNode("Name", item);
        node->fieldList->nextField->nextField = new FieldNode("Category", type);
        node->fieldList->nextField->nextField->nextField = new FieldNode("Price", cost);
        node->fieldList->nextField->nextField->nextField->nextField = new FieldNode("Date", timestamp);

        cout << "\nRecord Updated!\n";
    }

    void removeExpense() {
        string id;
        cout << "Enter Expense ID to Remove: ";
        getline(cin, id);

        RecordNode* node = start;
        while (node && node->expenseID != id) node = node->following;

        if (!node) {
            cout << "No record found with ID: " << id << endl;
            return;
        }

        if (node->previous) node->previous->following = node->following;
        else start = node->following;

        if (node->following) node->following->previous = node->previous;
        else end = node->previous;

        FieldNode* field = node->fieldList;
        while (field) {
            FieldNode* temp = field;
            field = field->nextField;
            delete temp;
        }
        delete node;

        cout << "\nRecord Deleted Successfully!\n";
    }

    void listExpenses() {
        if (!start) {
            cout << "\nNo Expenses Recorded.\n";
            return;
        }

        RecordNode* node = start;
        while (node) {
            cout << "\n==============================\n";
            FieldNode* field = node->fieldList;
            while (field) {
                cout << field->label << ": " << field->info << endl;
                field = field->nextField;
            }
            cout << "==============================\n";
            node = node->following;
        }
    }

    void createReport() {
        if (!start) {
            cout << "\nNo data available to generate report!\n";
            return;
        }

        // Count nodes
        int total = 0;
        RecordNode* temp = start;
        while (temp) {
            total++;
            temp = temp->following;
        }

        RecordNode** array = new RecordNode*[total];
        temp = start;
        for (int i = 0; i < total; i++) {
            array[i] = temp;
            temp = temp->following;
        }

        // Bubble sort by price
        for (int i = 0; i < total - 1; i++) {
            for (int j = 0; j < total - i - 1; j++) {
                double priceA = 0, priceB = 0;
                FieldNode* p1 = array[j]->fieldList;
                FieldNode* p2 = array[j+1]->fieldList;
                while (p1) {
                    if (p1->label == "Price") { priceA = convertToDouble(p1->info); break; }
                    p1 = p1->nextField;
                }
                while (p2) {
                    if (p2->label == "Price") { priceB = convertToDouble(p2->info); break; }
                    p2 = p2->nextField;
                }
                if (priceA < priceB) swap(array[j], array[j+1]);
            }
        }

        cout << "\n======= SORTED EXPENSE REPORT =======\n";
        for (int i = 0; i < total; i++) {
            FieldNode* f = array[i]->fieldList;
            while (f) {
                cout << f->label << ": " << f->info << endl;
                f = f->nextField;
            }
            cout << "--------------------------------\n";
        }

        cout << "\n>>> HIGHEST EXPENSE <<<\n";
        FieldNode* top = array[0]->fieldList;
        while (top) {
            cout << top->label << ": " << top->info << endl;
            top = top->nextField;
        }

        delete[] array;
    }
};

void showMainMenu() {
    cout << "\n==== Expense Tracker ====" << endl;
    cout << "1. Manage Expenses" << endl;
    cout << "2. View All Entries" << endl;
    cout << "3. Generate Report" << endl;
    cout << "4. Exit" << endl;
    cout << "Choose: ";
}

void showExpenseMenu() {
    cout << "\n--- Expense Menu ---" << endl;
    cout << "1. Add Expense" << endl;
    cout << "2. Edit Expense" << endl;
    cout << "3. Delete Expense" << endl;
    cout << "4. Return" << endl;
    cout << "Select Option: ";
}

int main() {
    BudgetManager manager;
    int option;
    do {
        showMainMenu();
        cin >> option;
        cin.ignore();

        switch (option) {
            case 1: {
                int sub;
                do {
                    showExpenseMenu();
                    cin >> sub;
                    cin.ignore();
                    if (sub == 1) manager.insertExpense();
                    else if (sub == 2) manager.updateExpense();
                    else if (sub == 3) manager.removeExpense();
                } while (sub != 4);
                break;
            }
            case 2:
                manager.listExpenses();
                break;
            case 3:
                manager.createReport();
                break;
            case 4:
                cout << "Thank you for using Expense Tracker!" << endl;
                break;
            default:
                cout << "Invalid option. Try again." << endl;
        }
    } while (option != 4);

    return 0;
}
    