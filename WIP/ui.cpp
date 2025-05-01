#include <iostream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

/**
 * User menu for processing BMP images
 * @param User input filename
 * @returns nothing
 */ 
void display_menu (const string& filename)
{
    cout << "\nIMAGE PROCESSING MENU\n" << endl;
    cout << "0) Change image (current: " << filename << ")" << endl;
    cout << "1) Vignette" << endl;
    cout << "2) Clarendon" << endl;
    cout << "3) Grayscale" << endl;
    cout << "4) Rotate 90 degrees" << endl;
    cout << "5) Rotate multiple 90 degrees" << endl;
    cout << "6) Enlarge" << endl;
    cout << "7) High Contrast" << endl;
    cout << "8) Lighten" << endl;
    cout << "9) Darken" << endl;
    cout << "10) Black, white, red, green, blue\n\n" << endl;
    cout << "Enter menu selection (Q to quit): ";  
}

/**
 * Overwrite check to make sure user isn't overwriting original image
 * Helper function for main()
 * @param input_filename
 * @return string
 */ 
string valid_output (const string& input_filename)
{
    string output;
    cin >> output;
    while (output == input_filename)
    {
        cout << "Cannot overwrite original image file. Please enter a different file name: " << endl;
        cin >> output;
    }
    return output;
}

/**
 * Input check for scaling factors for 2, 8, 9. Ensures the factor is between 0 and 1
 * Helper function for main()
 * @return the double scaling factor
 */ 
double valid_sfactor()
{
    double scaling_factor;
    cout << "Enter scaling factor: " << endl;
    cin >> scaling_factor;        
    while (scaling_factor <= 0.0 || scaling_factor >= 1.0 || cin.fail())
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid scaling factor. Please enter a number greater than 0 and less than 1." << endl;
        cin >> scaling_factor;
    }
    return scaling_factor;
}

/**
 * Input check to ensure numbers greater than 0
 * Helper function for main()
 * @param User input for 5, 6
 * @return the integer for the user prompt in 5, 6
 */ 
int valid_integer(const string& prompt)
{
    int value;
    cout << prompt;
    cin >> value;
    
    while (value <= 0 || cin.fail())
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Please enter a number greater than 0. " << prompt << endl;
        cin >> value;
    }
    return value;
}
/**
 * Prompt for user input of output file name for all selections
 * Helper function for main()
 * @return nothing
 */ 
void output_prompt()
{
    cout << "Enter output BMP filename: " << endl;
}

/**
 * Intro message for user; provides user the selected operation
 * Helper function for main()
 * @param operation user selects from main menu
 * @return nothing
 */ 
void intro_msg(const string& operation)
{
    cout << "You've chosen the " << operation << " operation" << endl;
}
/**
 * Success message after operation applied to BMP image
 * Helper function for main()
 * @param operation user selects from main menu
 * @return nothing
 */ 
void success_msg(const string& operation)
{
    cout << "Successfully applied " << operation << "!" << endl;
}

const vector<string> operations =
{"change image", "vignette", "clarendon", "grayscale", "rotate 90 degrees", "rotate multiple 90 degrees", "enlarge", "high contrast", "lighten", "darken", "black, white, red, green, blue filters"};

int main()
{
    // Surfacing initial menu; Reading in input_filename; Calling read_image helper file 
    // provided in assignment; Declaring menu input variable for operation menu -
    // display_menu helper function
    string input_filename;
    cout << "CSPB 1300 Image Processing Application" << endl;
    cout << "Enter input BMP filename: " << endl;
    cin >> input_filename;
    vector<vector<Pixel>> image = read_image(input_filename);
    string input;
    bool quit = false;
    
    do
    {
        // Helper function to surface operation menu and check for user input to quit
        display_menu(input_filename);
        cin >> input;
        
        if (input == "Q" || input == "q")
        {
            cout << "Thank you for using my program!" << endl;
            cout << "Quiting..." << endl;
            quit = true;
            continue;
        }
        
        // Takes user input and converts to option integer
        // Used in intro_msg and success_msg helper function
        // Used for if/else if statements and associated operations
        // Checks to ensure valid input of operations menu
        int option = -1;
        if (input == "0") { option = 0; }
        else if (input == "1") { option = 1; }
        else if (input == "2") { option = 2; }
        else if (input == "3") { option = 3; }
        else if (input == "4") { option = 4; }
        else if (input == "5") { option = 5; }
        else if (input == "6") { option = 6; }
        else if (input == "7") { option = 7; }
        else if (input == "8") { option = 8; }
        else if (input == "9") { option = 9; }
        else if (input == "10") { option = 10; }
        
        if (option < 0 || option > operations.size())
        {
            cout << "Invalid selection. Please enter a number between 0 and " << operations.size() -1 << ", or Q to quit." << endl;
            continue;  
        }
        
        // Helper function
        intro_msg(operations.at(option));
        
        if (option == 0)
        {
            cout << "Enter new input BMP filename: " << endl; 
            cin >> input_filename;
            image = read_image(input_filename);  // Helper function provided
        }
        
        else if (option == 1)
        {
            // Helper function for user to input output file name
            output_prompt(); 
            // Helper function to check output not overwriting input file name
            string output = valid_output(input_filename);  
            // Calling process_1 function
            auto processed = process_1(image);
            // Calling write_image helper function provided in assignment
            write_image(output, processed); 
        }
        
        else if (option == 2)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Implementing double scaling_factor
            // Checking for valid scaling_factor input
            double scaling_factor = valid_sfactor();
            // Calling process_2 function
            auto processed = process_2(image, scaling_factor);
            write_image (output, processed);
        }
        
        else if (option == 3)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Calling process_3 function
            auto processed = process_3(image);
            write_image(output, processed);
        }
        
        else if (option == 4)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Calling process_4 function
            auto processed = process_4(image);
            write_image(output, processed);
        }
        
        else if (option == 5)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Implementing int rotations
            // Resurfaces prompt until valid integer is provided
            int rotations = valid_integer("Enter number of 90 degree rotations: ");
            // Calling process_5 function
            auto processed = process_5(image, rotations);
            write_image(output, processed);
        }
        
        else if (option == 6)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Implementing integers x_scale, y_scale
            // Using valid_integer helper function to check value is greater than zero
            int x_scale = valid_integer("Enter X scale: ");
            int y_scale = valid_integer("Enter Y scale: ");
            // Calling process_6 function
            auto processed = process_6(image, x_scale, y_scale);
            write_image(output, processed);
        }
        
        else if (option == 7)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Calling process_7 function
            auto processed = process_7(image);
            write_image(output, processed);
        }
        
        else if (option == 8)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Implementing double scaling_factor
            // Checking for valid scaling_factor input
            double scaling_factor = valid_sfactor();
            // Calling process_8 function
            auto processed = process_8(image, scaling_factor);
            write_image(output, processed);
        }
        
        else if (option == 9)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Implementing double scaling_factor
            // Checking for valid scaling_factor input
            double scaling_factor = valid_sfactor();
            // Calling process_9 function
            auto processed = process_9(image, scaling_factor);
            write_image(output, processed);
        }
        
        else if (option == 10)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Calling process_10 function
            auto processed = process_10(image);
            write_image(output, processed);
        }
        
        // Helper function letting user know which operation was completed on image
        success_msg(operations.at(option));
        
    } while (!quit);
    
    return 0;

}
