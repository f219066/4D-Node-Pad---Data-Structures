#include<iostream>
#include<conio.h>
#include<iomanip>
#include<string> // string library 
#include<fstream> // for file handling
#include<thread> // for sleep break  
#include"trietree.h" // implementation of trie tree
#include<vector> // specify data type
#include<algorithm> // sortings 
#include<windows.h> // for colours and sleep

using namespace std;



class FourDNode
{
public:
    char data;
    FourDNode* right;
    FourDNode* left;
    FourDNode* up;
    FourDNode* down;
};
FourDNode** line_head = NULL, ** line_tail = NULL;

int head_size = 0, tail_size = 0;
FourDNode** resize(FourDNode**, int&);
int k = 7;
void Print_Horizontal_Line();

// stack adt for

class Stack
{
    char data;
    bool isDelete;
public:
    Stack();
    void setIsDelete(bool);
    void setData(char);
    char getData();
    bool getIsDelete();
    void Push(char, bool);
};
Stack::Stack()
{
    isDelete = false;
    this->data = 'x';
}
void Stack::Push(char ptr, bool isDelete)
{
    this->isDelete = isDelete;
    this->data = ptr;
}
void Stack::setIsDelete(bool a)
{
    this->isDelete = a;
}
void Stack::setData(char a)
{
    this->data = a;
}
char Stack::getData()
{
    return data;
}
bool Stack::getIsDelete()
{
    return isDelete;
}

Stack* resize(Stack*, int&);

class List
{

    FourDNode* head, * tail, * line_temp;
    float count, break_lines;
    Stack* undo_obj, redo;
    int undo_size;
    bool redo_check;
    friend Stack* resize(Stack*, int&);
    string copiedText;


public:

    List();
    FourDNode* GetHead();
    FourDNode* GetTail();
    void Create_Node(char, bool);
    void Delete_Node(bool);
    void Display_List();
    void Push_Stack(char, bool);
    void Pop_Stack();
    void Find_Word();
    void Replace_Word();
    void WordSuggestion(const string& currentWord);
    void CountTotalWords();
    void CopySelection(int start, int end);
    void PasteSelection(int index);


};


struct TrieNode* getNode();
void insert(struct TrieNode* root, const string& word);
void search(TrieNode* root, const string& prefix, vector<string>& suggestions);
void deleteTrie(struct TrieNode* node);


FourDNode* List::GetHead()
{
    return head;
}
FourDNode* List::GetTail()
{
    return tail;
}
List::List()
{
    head = NULL;
    tail = NULL;
    line_temp = NULL;
    undo_obj = NULL;
    count = 0;
    break_lines = 1;
    undo_size = 0;
    redo_check = false;
}


void List::Create_Node(char data, bool undo_check)
{
    // Create a new node
    FourDNode* new_node = new FourDNode;
    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->up = NULL;
    new_node->down = NULL;

    // If the list is empty, make the new node the head and tail
    if (head == NULL)
    {
        head = new_node;
        tail = new_node;
        count++;

        line_head = resize(line_head, head_size);
        line_head[head_size - 1] = head;
        line_temp = head;

        Push_Stack(data, false);

        // Open the file in append mode
        ofstream file("F:\\DataStructures Final Project\\notepad.txt", ios::app);
        if (file.is_open())
        {
            // Write the data to the file
            file << data;
            // Close the file
            file.close();
        }
        else
        {
            cout << "Unable to open the file." << endl;
        }

        return;
    }

    // Insert the new node at the end of the list
    tail->right = new_node;
    new_node->left = tail;
    tail = new_node;
    count++;

    if (undo_check)

        Push_Stack(data, false);

    // Handle line breaks and updating line_head and line_tail
    if (tail_size != 0)
    {
        if (line_temp != nullptr && line_temp->data != '\n')
        
        {
            tail->up = line_temp; // tailup of new
            line_temp->down = tail; // line temp down = current node
        }
        if (line_temp != nullptr) {
            line_temp = line_temp->right;
        }
    }

    if ((count / 60) == break_lines) // no of completed line matches calculated one then it's full 
    {
        FourDNode* temp = new_node;

        Create_Node('\n', true);  // if not full creates a new line and calculates break lines

        break_lines = break_lines + 1;
    }
    else if (data == '\n')
    {
        if (line_temp != nullptr && line_temp->right != nullptr)
        {
            line_temp = line_temp->right;
        }
        else
        {

            line_temp = nullptr;

        }

        line_tail = resize(line_tail, tail_size);
        line_tail[tail_size - 1] = tail;
        line_head = resize(line_head, head_size);
        line_head[head_size - 1] = line_temp;
    }

    count++;
    break_lines++;

    // Open the file in append mode
    ofstream file("F:\\DataStructures Final Project\\notepad.txt", ios::app);
    if (file.is_open())  // after the insertion is done the data is stored in a txt file.
    {
        // Write the data to the file
        file << data;
        // Close the file
        file.close();
    }
    else
    {

        cout << "Unable to open the file." << endl;
    }

    // Update the up and down pointers
    if (tail_size > 0)
    {
        tail->up = line_tail[tail_size - 1];
        line_tail[tail_size - 1]->down = tail;
    }

   
}

static List obj;

// we are using trie tree to make the suggestions work

void List::WordSuggestion(const string& currentWord)
{
    // Read suggestions from suggestions.txt file
    ifstream inputFile("F:\\DataStructures Final Project\\finaloutfile.txt");

    if (!inputFile.is_open())
    {
        cout << "Unable to open suggestions.txt file." << endl;
        return;
    }

    // Build Trie from suggestions in the file
    TrieNode* root = getNode();

    string suggestion;

    while (inputFile >> suggestion)
    {
        insert(root, suggestion);
    }
    inputFile.close();

    vector<string> suggestions;

    search(root, currentWord, suggestions);

    // Check if there are no suggestions
    if (suggestions.empty())
    {

        cout << "\nNo suggestions found." << endl;
        // Deallocate memory
        deleteTrie(root);
        this_thread::sleep_for(chrono::seconds(2));
        return;
    }

    // Sort suggestions alphabetically
    sort(suggestions.begin(), suggestions.end());

    // Display suggestions
    cout << "Suggestions: " << endl;
    for (size_t i = 0; i < suggestions.size(); i++)
    {
        cout << i << ". " << suggestions[i] << endl;
    }
    cout << endl;

    // Wait for the user to choose a suggestion
    int choice;
    do 
    {

        cout << "Choose a suggestion (0-" << suggestions.size() - 1 << "): ";
        cin >> choice;
        cout << endl;

    } while (choice < 0 || choice > static_cast<int>(suggestions.size()) - 1);

    string chosenWord = suggestions[choice];

    if (!chosenWord.empty())
    {
        for (int i = currentWord.size(); i < chosenWord.size(); i++)
        {
            obj.Create_Node(chosenWord[i], true);
        }
    }

    // Deallocate memory
    deleteTrie(root);
}



// deletion in notepad 
// i am deleting from the tail only 
// as i was facing some sort of issues

void List::Delete_Node(bool undo_check) // delets the node from the tail only --> for example you abcd is in file after hitting delete d will be deleted then c then b then a.
{
    if (head == NULL)
        return;
    if (tail->left == NULL)

        head = NULL;

    if (tail->data == '\n')// removing break lines head size and tail siz 
    {
        break_lines--;

        head_size--;

        tail_size--; // represents size reduction becz of deletion 
    }
    if (tail == NULL)
        return;

    if (head != NULL)
    {
        if (undo_check)  //

            Push_Stack(tail->data, true);

        FourDNode* temp = tail->left;

        temp->right = NULL;

        tail = temp;

        count--;
    }
}

// displays th list 


void List::Display_List()

{
    FourDNode* temp = head;

    while (temp != NULL)
    {
        cout << temp->data;
        temp = temp->right;
    }
}



void List::Push_Stack(char ptr, bool isDelete)
{
    undo_obj = resize(undo_obj, undo_size);
    undo_obj[undo_size - 1].setData(ptr);
    undo_obj[undo_size - 1].setIsDelete(isDelete);
    redo_check = false;
}



void List::Pop_Stack()
{
    if (undo_size == 0)
        return;

    if ((undo_obj[undo_size - 1].getIsDelete()))


        Create_Node(undo_obj[undo_size - 1].getData(), true);

    else

        Delete_Node(true);

    undo_size = undo_size - 2;

    if (undo_size < 0)

        undo_size = 0;

    redo_check = true;
}


void List::Find_Word()  // function to find word 
{
    int count_words = 0;

    string temp;

    cout << "Enter Word to Find: ";

    getline(cin, temp);  // Get the word to find from the user

    char* arr = new char[temp.length() + 1];  // Allocate memory for a character array

    for (int i = 0; i < temp.length(); i++)

        arr[i] = temp[i];  // Copy characters from the string to the character array

    arr[temp.length()] = '\0';  // Add null terminator at the end of the character array

    FourDNode* t1 = head;
    FourDNode* t2;
    bool fh = true;

    while (t1 != NULL)
    {
        if (t1->data == arr[0])  // Check if the first character matches
        {
            t2 = t1;
            for (int i = 0; i < temp.length(); i++)
            {
                if (t2 != NULL)
                {
                    if (t2->data != arr[i])  // Check if subsequent characters match
                    {
                        fh = false;
                        break;
                    }
                }
                else
                    break;

                t2 = t2->right;
            }

            if (fh)  // If all characters match, increment the word count
            {
                count_words++;
            }
        }

        t1 = t1->right;
        fh = true;
    }

    cout << endl << endl;

    cout << count_words << " result" << (count_words > 1 ? "s" : "") << " found for \"" << temp << "\"." << endl;

    cout << endl << "Press Enter to Continue...";
    getchar();
}


// function to cuclate total words inserted
// basically we are reading from the file where we have inserted the words

void List::CountTotalWords()
{
    ifstream file("F:\\DataStructures Final Project\\notepad.txt");

    if (file.is_open())
    {
        string word;

        int wordCount = 0;


        while (file >> word)
        {
            wordCount++;
        }

        cout << "Total words in the file: " << wordCount << endl;

        file.close();
    }
    else
    {
        cout << "Failed to open the file." << endl;
    }
}




// Function to copy the selected portion of the text
void List::CopySelection(int start, int end)
{
    // Validate the start and end indices
    if (start < 0 || end >= count || start > end) 
    {
        cout << "Invalid selection." << endl;
        return;
    }

    // Clear the copied text
    copiedText.clear();

    // Traverse the list to find the selected nodes
    FourDNode* temp = head;
    int currentIndex = 0;
    while (temp != nullptr) 
    {
        if (currentIndex >= start && currentIndex <= end)
        {
            copiedText += temp->data;
        }
        temp = temp->right;
        currentIndex++;
    }

    cout << "Selection copied." << endl;
}

// Function to paste the copied selection at the specified index
void List::PasteSelection(int index)
{
    // Validate the index
    if (index < 0 || index > count)
    {
        cout << "Invalid paste index." << endl;
        return;
    }

    // Check if there's anything copied
    if (copiedText.empty())
    {
        cout << "No text is copied." << endl;
        return;
    }

    // Traverse the list to the paste index
    FourDNode* temp = head;
    int currentIndex = 0;

    while (currentIndex < index)
    {
        temp = temp->right;
        currentIndex++;
    }

    // Insert the copied text at the paste index
    for (char c : copiedText)
    {
        Create_Node(c, true);
    }

    cout << "Selection pasted." << endl;
}




void List::Replace_Word()
{
    // Declare variables
    string temp, replacement;
    Stack* obj = NULL;
    int obj_size = 0;

    // Prompt user for word to replace and replacement word
    cout << "Enter Word to Replace: ";
    getline(cin, temp);
    cout << "Enter the Required Word: ";
    getline(cin, replacement);

    // Calculate string lengths
    const unsigned int string_length = temp.length();

    const unsigned int replace_length = replacement.length();

    // Create character arrays for word and replacement
    char* arr = new char[string_length];

    for (unsigned int i = 0; i < string_length; i++)

        arr[i] = temp[i];

    char* replaced_word = new char[replace_length];

    for (unsigned int i = 0; i < replace_length; i++)

        replaced_word[i] = replacement[i];

    // Initialize pointers and flags
    FourDNode* t1 = head;
    FourDNode* t2 = NULL;
    FourDNode* t3 = head;
    bool fh = false;
    bool check = false;
    List new_obj;

    // Iterate through the linked list
    while (t1 != NULL)
    {
        // Check if the first character matches the word to replace
        if (t1->data == arr[0])
        {
            fh = true;
            t2 = t1;

            // Check if subsequent characters match the word to replace
            for (int i = 0; i < temp.length(); i++)
            {
                if (t2 != NULL)
                {
                    if (t2->data != arr[i])
                    {
                        fh = false;
                        break;
                    }
                }
                else
                    break;

                t2 = t2->right;
            }

            // Set the check flag if all characters match
            if (fh)
                check = true;

            fh = true;
        }

        t3 = head;

        // If check flag is true, perform the replacement
        if (check)
        {
            // Iterate through the linked list
            while (t3 != NULL)
            {
                if (t3 == t1)
                {
                    // Add the characters of the replacement word to new_obj
                    for (unsigned int i = 0; i < replace_length; i++)
                    {
                        new_obj.Create_Node(replaced_word[i], true);
                    }

                    // Skip the characters of the word to replace
                    for (unsigned int i = 0; i < string_length; i++)
                    {
                        t3 = t3->right;
                    }
                }

                // Add the remaining characters of the original linked list to new_obj
                new_obj.Create_Node(t3->data, true);
                t3 = t3->right;
            }

            // Update the head, tail, t1, and t3 pointers with the ones from new_obj
            this->head = new_obj.GetHead();

            this->tail = new_obj.GetTail();

            t1 = new_obj.GetHead();

            t3 = new_obj.GetHead();
            break;
        }

        t1 = t1->right;
        check = false;
        fh = true;
    }

    // Print a horizontal line
    Print_Horizontal_Line();

    // Display the updated linked list if check is true
    if (check)
    {
        this->Display_List();

        Print_Horizontal_Line();

        cout << "The word has been replaced" << endl;
    }
    else
        cout << "There is no " << temp << "\" in the document.";

    cout << endl << endl << "Press Enter to Continue...";
    getchar();
}


double checkSimilarity(const string& file1, const string& file2)
{
    // Read the contents of file1
    ifstream inFile1(file1);

    string line;

    string content1;

    while (getline(inFile1, line))
    {
        content1 += line;
    }
    inFile1.close();

    // Read the contents of file2
    ifstream inFile2(file2);

    string content2;

    while (getline(inFile2, line))
    {
        content2 += line;
    }
    inFile2.close();

    // Check the similarity
    double matches = 0;
    size_t minSize = min(content1.size(), content2.size());

    for (size_t i = 0; i < minSize; ++i)
    {
        if (content1[i] == content2[i])
        {
            matches++;
        }
    }

    // Calculate the percentage of match
    double similarityPercentage = (matches / content1.size()) * 100;

    return similarityPercentage;
}




FourDNode** resize(FourDNode** p, int& size)
{
    FourDNode** temp = new FourDNode * [size + 1];

    for (int i = 0; i < size; i++)
        temp[i] = p[i];
    size++;
    delete[]p;
    return temp;
}
Stack* resize(Stack* ptr, int& size)
{
    Stack* temp = new Stack[size + 1];

    for (int i = 0; i < size; i++)

        temp[i] = ptr[i];

    size++;
    delete[]ptr;
    return temp;

}

void Print_Horizontal_Line()
{
    string a(75, '-');
    cout << endl << endl << a << endl << endl;
}


void Main_Menu();
void Notepad();
void Notepad_visuals();

int main()
{
   
    cout << "   _   _ " << endl;
    cout << "  | \\ | |" << endl;
    cout << "  |  \\| |" << endl;
    cout << "  | \\   |" << endl;
    cout << "  |_|\\__|" << endl;

    Sleep(600);

     cout<<"\t"<< "  ____  " << endl;
  cout << "\t" << " / __ \\ " << endl;
  cout << "\t" << "| |  | |" << endl;
  cout << "\t" << "| |  | |" << endl;
  cout << "\t"<< "| |__| |" << endl;
  cout << "\t" << " \\____/ " << endl;

    Sleep(600);

    cout << "\t\t"  << " _______ " << endl;
    cout << "\t\t"  << "|__   __|" << endl;
    cout << "\t\t"  << "   | |   " << endl;
    cout << "\t\t"  << "   | |   " << endl;
    cout << "\t\t"  << "   | |   " << endl;
    cout << "\t\t"  << "   |_|   " << endl;

    Sleep(600);

    cout << "\t\t\t"  << " ______ " << endl;
    cout << "\t\t\t"  << "|  ____|" << endl;
    cout << "\t\t\t"  << "| |__   " << endl;
    cout << "\t\t\t"  << "|  __|  " << endl;
    cout << "\t\t\t"  << "| |____ " << endl;
    cout << "\t\t\t"  << "|______|" << endl;


    Sleep(600);

    cout << "\t\t\t\t"  << " _______ " << endl;
    cout << "\t\t\t\t"  << "|  _____|" << endl;
    cout << "\t\t\t\t"  << "|  |    |    " << endl;
    cout << "\t\t\t\t"  << "|  |____| " << endl;
    cout << "\t\t\t\t"  << "| |      " << endl;
    cout << "\t\t\t\t"  << "|_|      " << endl;

    Sleep(600);

    cout << "\t\t\t\t\t" << "    /\\    " << endl;
    cout << "\t\t\t\t\t" << "   /  \\   " << endl;
    cout << "\t\t\t\t\t" << "  ====== " << endl;
    cout << "\t\t\t\t\t" << " /      \\ " << endl;
    cout << "\t\t\t\t\t" << "/        \\ " << endl;

    Sleep(600);

    cout << "\t\t\t\t\t\t\t" << "  _____  " << endl;
    cout << "\t\t\t\t\t\t\t" << " |  __ \\ " << endl;
    cout << "\t\t\t\t\t\t\t" << " | |  | |" << endl;
    cout << "\t\t\t\t\t\t\t" << " | |  | |" << endl;
    cout << "\t\t\t\t\t\t\t" << " | |__| |" << endl;
    cout << "\t\t\t\t\t\t\t" << " |_____/ " << endl;

    Sleep(600);

    cout << endl;


    cout << "\t\t\t\t\t\t\t\t" << "  ++ " << endl;
    cout << "\t\t\t\t\t\t\t\t" << "  ++  " << endl;
    cout << "\t\t\t\t\t\t\t\t" << "++++++   " << endl;
    cout << "\t\t\t\t\t\t\t\t" << "  ++" << endl;
    cout << "\t\t\t\t\t\t\t\t" << "  ++" << endl;

    Sleep(600);

    cout << endl;

    cout << "\t\t\t\t\t\t\t\t\t"  << "  ++" << endl;
    cout << "\t\t\t\t\t\t\t\t\t"  << "  ++" << endl;
    cout << "\t\t\t\t\t\t\t\t\t"  << "++++++" << endl;
    cout << "\t\t\t\t\t\t\t\t\t"  << "  ++" << endl;
    cout << "\t\t\t\t\t\t\t\t\t"  << "  ++" << endl;


    Sleep(1000);
    system("cls");

    Main_Menu();
    cout << endl;
    return 0;
}



void Main_Menu()
{

    Notepad_visuals();

    char input;
    bool fh = true;

    while (fh)
    {
        input = _getch();

        switch (input)
        {
        case 27:
            fh = false;
            break;
        case 9:
        case 13:
            Notepad();
            break;
        default:
            break;
        }
        system("cls");
        Notepad_visuals();
    }
}




void Notepad()
{
    system("cls");
    char input;
    obj.Display_List();
    bool fh = true;
    string currentWord; // Store the current word
    string copiedText;
    while (fh)
    {
        system("cls");
        cout << "\t\tYou have been directed into Notepad\n" << endl;
        cout << "\t\tChoose What you want to do!.?\n" << endl;



        cout << "\033[1;32m*\033[0m . Delete a Word                          Backspace " << endl;
        cout << "\033[1;32m*\033[0m . Word Suggestion                        . for suggestions " << endl;
        cout << "\033[1;32m*\033[0m . Find a Word                            ` (Key under ESC)" << endl;
        cout << "\033[1;32m*\033[0m . Replace a Word                         #" << endl;
        cout << "\033[1;32m*\033[0m . Counting the total words               @" << endl;
        cout << "\033[1;32m*\033[0m . Check Similarity Of two Files          %" << endl;
        cout << "\033[1;32m*\033[0m . Copy & Paste Text (not working)        & to copy and ( to paste" << endl;
        cout << "\033[1;32m*\033[0m . Undo your Action                       Tab" << endl;
        cout << endl;
        cout << "\t\t<-------------------Notepad----------------->" << endl << endl;
        cout << endl;

        obj.Display_List();

        input = _getch();

        if (input == 8)
            obj.Delete_Node(true);

        else if (input == 13)
           

            obj.Create_Node('\n', true);


        else if (input == 9 || input == 11)
            obj.Pop_Stack();


        else if (input == 38)
        {
            cout << "Enter the text to copy: ";
            getline(cin, copiedText);

            cout << "Text copied successfully." << endl;
            cout << "Press any key to continue..." << endl;
            _getch();
        }
        else if (input == 40)
        {
            if (!copiedText.empty())
            {
                cout << "Copied text: " << copiedText << endl;
                cout << "Enter the position to paste: ";
                int position;
                cin >> position;
                cin.ignore(); // Ignore the newline character

                // Call the PasteSelection function with the specified position
                obj.PasteSelection(position);
                cout << "Text pasted successfully." << endl;
            }
            else
            {
                cout << "No text is copied. Please copy some text first." << endl;
            }

            cout << "Press any key to continue..." << endl;
            _getch();
        }



        else if (input == 96)
        {
            obj.Display_List();
            Print_Horizontal_Line();
            obj.Find_Word();
        }
        else if (input == 35)
        {
            obj.Display_List();
            Print_Horizontal_Line();
            obj.Replace_Word();
        }

        else if (input == 37)

        {
            string file1 = "F:\\DataStructures Final Project\\fileone.txt";
            string file2 = "F:\\DataStructures Final Project\\filetwo.txt";

            // Ask the user if they want to write to the files
            char writeFiles;
            cout << "Do you want to write to the files first? (y/n): ";
            cin >> writeFiles;

            if (writeFiles == 'y' || writeFiles == 'Y')
            {
                // Write to file1
                ofstream outFile1(file1);
                string content;
                cout << "Enter the content for file1 (end with a blank line):\n";
                cin.ignore();  // Ignore any remaining newline character
                while (getline(cin, content) && !content.empty()) 
                {
                    outFile1 << content << endl;
                }
                outFile1.close();

                // Write to file2
                ofstream outFile2(file2);
                cout << "Enter the content for file2 (end with a blank line):\n";
                while (getline(cin, content) && !content.empty())
                {
                    outFile2 << content << endl;
                }
                outFile2.close();
            }

            // Check similarity and output the percentage of match
            double similarity = checkSimilarity(file1, file2);
            cout << "Similarity: " << similarity << "%\n";

            cout << "Press any key to continue..." << endl;
            _getch();

        }


        else if (input == 64)
        {
            obj.Display_List();
            obj.CountTotalWords();  // Call the CountTotalWords function
            cout << "Press any key to continue..." << endl;
            _getch();
        }




        else if (input == 27)
        {
            return;
        }
        else if (input == '.')
        {
            obj.WordSuggestion(currentWord);  // Call the WordSuggestion function with the current word as prefix
        }


        else
        {
            obj.Create_Node(input, true);

            if (input == ' ')
            {
                currentWord = "";
            }
            else
            {
                currentWord += input; // Update the current word
            }
        }
        obj.Display_List();
    }
}


void Notepad_visuals()
{
    cout << "\033[1;33m**************************************************\033[0m" << endl;
    cout << "\033[1;33m*\033[0m            \033[1;36mWelcome to Advanced Notepad++        \033[0m\033[1;33m*\033[0m" << endl;
    cout << "\033[1;33m**************************************************\033[0m" << endl;
    cout << endl;
    cout << "\t\t  Instructions:-" << endl;
    cout << "\033[1;34m------------------------------------------------------------\033[0m" << endl;
    cout << "\033[1;32m*\033[0m . Open Notepad                           Hit Enter" << endl;
    cout << "\033[1;32m*\033[0m . Delete a Word                          Hit Backspace " << endl;
    cout << "\033[1;32m*\033[0m . Word Suggestion                        Hit . for suggestions " << endl;
    cout << "\033[1;32m*\033[0m . Find a Word                            Hit ` (Key under ESC)" << endl;
    cout << "\033[1;32m*\033[0m . Replace a Word                         Hit #" << endl;
    cout << "\033[1;32m*\033[0m . Counting the total words               Hit @" << endl;
    cout << "\033[1;32m*\033[0m . Check Similarity Of two Files          Hit %" << endl;
    cout << "\033[1;32m*\033[0m . Copy & Paste Text (not working)        Hit & to copy and ( to paste" << endl;
    cout << "\033[1;32m*\033[0m . Undo your Action                       Hit Tab" << endl;
    cout << "\033[1;32m*\033[0m . Exit Notepad                           Hit Esc" << endl;
    cout << "\033[1;32m*\033[0m . Return to Menu                         Hit Esc" << endl;
    cout << "\033[1;34m************************************************************\033[0m" << endl;
    cout << "\n" << endl;
    cout << endl;
    cout << "\033[1;31m **Disclaimer: All the functionalities would work once you enter the notepad\033[0m" << endl;

}


