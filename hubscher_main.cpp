/*
main.cpp
CSPB 1300 Image Processing Application

PLEASE FILL OUT THIS SECTION PRIOR TO SUBMISSION

- Your name:
    <Jason Hubscher>

- All project requirements fully met? (YES or NO):
    <Yes>

- If no, please explain what you could not get to work:
    <N/A>

- Did you do any optional enhancements? If so, please explain:
    <No>
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <cctype>
using namespace std;

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION BELOW                                    //
//***************************************************************************************************//

// Pixel structure
struct Pixel
{
    // Red, green, blue color values
    int red;
    int green;
    int blue;
};

/**
 * Gets an integer from a binary stream.
 * Helper function for read_image()
 * @param stream the stream
 * @param offset the offset at which to read the integer
 * @param bytes  the number of bytes to read
 * @return the integer starting at the given offset
 */ 
int get_int(fstream& stream, int offset, int bytes)
{
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < bytes; i++)
    {   
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}

/**
 * Reads the BMP image specified and returns the resulting image as a vector
 * @param filename BMP image filename
 * @return the image as a vector of vector of Pixels
 */
vector<vector<Pixel>> read_image(string filename)
{
    // Open the binary file
    fstream stream;
    stream.open(filename, ios::in | ios::binary);

    // Get the image properties
    int file_size = get_int(stream, 2, 4);
    int start = get_int(stream, 10, 4);
    int width = get_int(stream, 18, 4);
    int height = get_int(stream, 22, 4);
    int bits_per_pixel = get_int(stream, 28, 2);

    // Scan lines must occupy multiples of four bytes
    int scanline_size = width * (bits_per_pixel / 8);
    int padding = 0;
    if (scanline_size % 4 != 0)
    {
        padding = 4 - scanline_size % 4;
    }

    // Return empty vector if this is not a valid image
    if (file_size != start + (scanline_size + padding) * height)
    {
        return {};
    }

    // Create a vector the size of the input image
    vector<vector<Pixel>> image(height, vector<Pixel> (width));

    int pos = start;
    // For each row, starting from the last row to the first
    // Note: BMP files store pixels from bottom to top
    for (int i = height - 1; i >= 0; i--)
    {
        // For each column
        for (int j = 0; j < width; j++)
        {
            // Go to the pixel position
            stream.seekg(pos);

            // Save the pixel values to the image vector
            // Note: BMP files store pixels in blue, green, red order
            image[i][j].blue = stream.get();
            image[i][j].green = stream.get();
            image[i][j].red = stream.get();

            // We are ignoring the alpha channel if there is one

            // Advance the position to the next pixel
            pos = pos + (bits_per_pixel / 8);
        }

        // Skip the padding at the end of each row
        stream.seekg(padding, ios::cur);
        pos = pos + padding;
    }

    // Close the stream and return the image vector
    stream.close();
    return image;
}

/**
 * Sets a value to the char array starting at the offset using the size
 * specified by the bytes.
 * This is a helper function for write_image()
 * @param arr    Array to set values for
 * @param offset Starting index offset
 * @param bytes  Number of bytes to set
 * @param value  Value to set
 * @return nothing
 */
void set_bytes(unsigned char arr[], int offset, int bytes, int value)
{
    for (int i = 0; i < bytes; i++)
    {
        arr[offset+i] = (unsigned char)(value>>(i*8));
    }
}

/**
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<Pixel>>& image)
{
    // Get the image width and height in pixels
    int width_pixels = image[0].size();
    int height_pixels = image.size();

    // Calculate the width in bytes incorporating padding (4 byte alignment)
    int width_bytes = width_pixels * 3;
    int padding_bytes = 0;
    padding_bytes = (4 - width_bytes % 4) % 4;
    width_bytes = width_bytes + padding_bytes;

    // Pixel array size in bytes, including padding
    int array_bytes = width_bytes * height_pixels;

    // Open a file stream for writing to a binary file
    fstream stream;
    stream.open(filename, ios::out | ios::binary);

    // If there was a problem opening the file, return false
    if (!stream.is_open())
    {
        return false;
    }

    // Create the BMP and DIB Headers
    const int BMP_HEADER_SIZE = 14;
    const int DIB_HEADER_SIZE = 40;
    unsigned char bmp_header[BMP_HEADER_SIZE] = {0};
    unsigned char dib_header[DIB_HEADER_SIZE] = {0};

    // BMP Header
    set_bytes(bmp_header,  0, 1, 'B');              // ID field
    set_bytes(bmp_header,  1, 1, 'M');              // ID field
    set_bytes(bmp_header,  2, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE+array_bytes); // Size of BMP file
    set_bytes(bmp_header,  6, 2, 0);                // Reserved
    set_bytes(bmp_header,  8, 2, 0);                // Reserved
    set_bytes(bmp_header, 10, 4, BMP_HEADER_SIZE+DIB_HEADER_SIZE); // Pixel array offset

    // DIB Header
    set_bytes(dib_header,  0, 4, DIB_HEADER_SIZE);  // DIB header size
    set_bytes(dib_header,  4, 4, width_pixels);     // Width of bitmap in pixels
    set_bytes(dib_header,  8, 4, height_pixels);    // Height of bitmap in pixels
    set_bytes(dib_header, 12, 2, 1);                // Number of color planes
    set_bytes(dib_header, 14, 2, 24);               // Number of bits per pixel
    set_bytes(dib_header, 16, 4, 0);                // Compression method (0=BI_RGB)
    set_bytes(dib_header, 20, 4, array_bytes);      // Size of raw bitmap data (including padding)                     
    set_bytes(dib_header, 24, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 28, 4, 2835);             // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 32, 4, 0);                // Number of colors in palette
    set_bytes(dib_header, 36, 4, 0);                // Number of important colors

    // Write the BMP and DIB Headers to the file
    stream.write((char*)bmp_header, sizeof(bmp_header));
    stream.write((char*)dib_header, sizeof(dib_header));

    // Initialize pixel and padding
    unsigned char pixel[3] = {0};
    unsigned char padding[3] = {0};

    // Pixel Array (Left to right, bottom to top, with padding)
    for (int h = height_pixels - 1; h >= 0; h--)
    {
        for (int w = 0; w < width_pixels; w++)
        {
            // Write the pixel (Blue, Green, Red)
            pixel[0] = image[h][w].blue;
            pixel[1] = image[h][w].green;
            pixel[2] = image[h][w].red;
            stream.write((char*)pixel, 3);
        }
        // Write the padding bytes
        stream.write((char *)padding, padding_bytes);
    }

    // Close the stream and return true
    stream.close();
    return true;
}

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION ABOVE                                    //
//***************************************************************************************************//

/**
 * Applies a vignette effect by darkening the image based on distance from the center.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @return the processed 2D vector of Pixels after applying vignette effect
 */
vector<vector<Pixel>> process_1(const vector<vector<Pixel>>& image)
{
    // Get the number of rows/columns from the input 2D vector (remember: num_rows is height, num_columns is width)   
    int height = image.size();
    int width = image[0].size();
            
    // Define a new 2D vector the same size as the input 2D vector 
    vector<vector<Pixel>> new_image(height, vector<Pixel> (width));

    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {   
        // For each of the columns in the input 2D vector
        for (int col = 0; col < width; col++)
        {
            // Get the color values for the pixel located at this row and column in the input 2D vector
            int red, green, blue;
            red = image[row][col].red;
            green = image[row][col].green;
            blue = image[row][col].blue;

            // Perform the operation on the color values 
            double distance = sqrt(pow((col - width/2.0), 2) + pow((row - height/2.0), 2));
            double scaling_factor = (height - distance) / height;

            // Set new RGB values
            int newRed, newGreen, newBlue;
            newRed = red * scaling_factor;
            newGreen = green * scaling_factor;
            newBlue = blue * scaling_factor;
            
            // Save the new color values to the corresponding pixel located at this row and column in the new 2D vector
            new_image[row][col] = {newRed, newGreen, newBlue};
        }
    }

    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

/**
 * Applies a clarendon-like filter to lighten or darken pixels based on average brightness.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @param scaling_factor a double value between 0 and 1 to scale pixel brightness
 * @return the processed 2D vector of Pixels after applying clarendon filter
 */
vector<vector<Pixel>> process_2(const vector<vector<Pixel>>& image, double scaling_factor)
{
    // Get the number of rows/columns from the input 2D vector; User input for scaling_factor; Define a new 2D vector the same size as the input 2D vector.
    
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel> (width));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < width; col++)
        {
            // Get the color values for the pixel located at this row and column in the input 2D vector
            int red, green, blue;
            red = image[row][col].red;
            green = image[row][col].green;
            blue = image[row][col].blue;
            
            // Average these values
            double average_value = (red + green + blue) / 3.0;
            
            // If the cell is light, make it lighter
            int newRed, newGreen, newBlue;
            
            if (average_value >= 170)
            {
                newRed = (255 - (255 - red) * scaling_factor);
                newGreen = (255 - (255 - green) * scaling_factor);
                newBlue = (255 - (255 - blue) * scaling_factor);
            }
            else if (average_value < 90)
            {
                newRed = red * scaling_factor;
                newGreen = green * scaling_factor;
                newBlue = blue * scaling_factor;
            }
            else
            {
                newRed = red;
                newGreen = green;
                newBlue = blue;
            }
           
            // Set new pixel to new color values
            new_image[row][col] = {newRed, newGreen, newBlue};     
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

/**
 * Converts the image to grayscale by averaging red, green, and blue values of each pixel.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @return the processed 2D vector of Pixels in grayscale
 */
vector<vector<Pixel>> process_3(vector<vector<Pixel>>& image)
{
    // Get the number of rows/columns from the input 2D vector; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel> (width));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < width; col++)
        {
            // What are the R,G,B values
            int red, green, blue;
            red = image[row][col].red;
            green = image[row][col].green;
            blue = image[row][col].blue;
            
            // Average those values to get the grey value
            double gray_value = (red + green + blue) / 3.0;
            
            // Set new color values to all be our grey value
            int newRed, newGreen, newBlue;
            newRed = gray_value;
            newGreen = gray_value;
            newBlue = gray_value;
            
            // Set new pixel to new color values
            new_image[row][col] = {newRed, newGreen, newBlue};
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

/**
 * Rotates the image 90 degrees clockwise.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @return the rotated 2D vector of Pixels
 */
vector<vector<Pixel>> process_4(vector<vector<Pixel>>& image)
{
    // Get the number of rows/columns from the input 2D vector; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(width, vector<Pixel> (height));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < width; col++)
        {
            // Rotate the image by 90 degrees clockwise
            new_image.at(col).at(height - 1 - row) = image.at(row).at(col);
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

/**
 * Rotates the input image 90 degrees clockwise.
 * Helper function for main() and other rotation operations
 * @param image the original input 2D vector of Pixels
 * @return the rotated 2D vector of Pixels
 */
vector<vector<Pixel>> rotate_by_90(const vector<vector<Pixel>>& image)
{
    // Get the number of rows/columns from the input 2D vector; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(width, vector<Pixel> (height));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {
        // For each of the colums in the input 2D vector
        {
            for (int col = 0; col < width; col++)
            {
                // Rotate the image by 90 degrees clockwise
                new_image.at(col).at(height - 1 - row) = image.at(row).at(col);
            }
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

/**
 * Rotates the image by a user-specified multiple of 90 degrees.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @param value the number of 90-degree rotations to apply
 * @return the rotated 2D vector of Pixels
 */
vector<vector<Pixel>> process_5(vector<vector<Pixel>>& image, int value)
{
    // Get the number of rows/columns from the input 2D vector; Allows user input for desired 90 degree rotations; Define value, angle variable; Define a new 2D vector the same size as the input 2D vector.  Calls helper function rotate_by_90;
    int height = image.size();
    int width = image[0].size();
    int angle = (value * 90) % 360;
    vector<vector<Pixel>> new_image(height, vector<Pixel> (width));
    
    // If angle is not a multiple of 90 then error
    if (angle % 90 != 0)
    {
        cout << "Angle must be a multiple of 90 degrees" << endl;
        return image;
    }
    else if (angle == 0)
    {
        return image;
    }
    else if (angle == 90)
    {
        return rotate_by_90(image);
    }
    else if (angle == 180)
    {
        return rotate_by_90(rotate_by_90(image));
    }
    else
    {
        return rotate_by_90(rotate_by_90(rotate_by_90(image)));
    }
}

/**
 * Enlarges the image by scaling rows and columns based on x and y scale factors.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @param x_scale integer scale factor in horizontal direction
 * @param y_scale integer scale factor in vertical direction
 * @return the enlarged 2D vector of Pixels
 */
vector<vector<Pixel>> process_6(vector<vector<Pixel>>& image, int x_scale, int y_scale)
{
    // Get the number of rows/columns from the input 2D vector; Declared variables for x_scale, y_scale; User input for added height/width; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(y_scale * height, vector<Pixel> (x_scale * width));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < (y_scale * height); row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < (x_scale * width); col++)
        {
            int src_row = row / y_scale;
            int src_col = col / x_scale;
            new_image.at(row).at(col) = image.at(src_row).at(src_col);
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

/**
 * Applies a high contrast filter by setting each pixel to black or white based on brightness.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @return the processed 2D vector of Pixels after high contrast filter
 */
vector<vector<Pixel>> process_7(vector<vector<Pixel>>& image)
{
    // Get the number of rows/columns from the input 2D vector; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel> (width));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < width; col++)
        {
            // Get values for RGB
            int red, green, blue;
            red = image[row][col].red;
            green = image[row][col].green;
            blue = image[row][col].blue;
            
            // Average values to get the gray value
            double gray_value = (red + green + blue) / 3.0;
            
            int newRed, newGreen, newBlue;
            if (gray_value >= 255.0 / 2.0)
            {
                newRed = 255;
                newGreen = 255;
                newBlue = 255;
            }
            else
            {
                newRed = 0;
                newGreen = 0;
                newBlue = 0;
            }
            
            // Store new pixel values
            new_image[row][col] = {newRed, newGreen, newBlue};
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
    
}

/**
 * Lightens the image by scaling RGB values toward white based on a scaling factor.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @param scaling_factor a double value between 0 and 1 to lighten the image
 * @return the processed 2D vector of Pixels after lightening
 */
vector<vector<Pixel>> process_8(vector<vector<Pixel>>& image, double scaling_factor)
{
    // Get the number of rows/columns from the input 2D vector; Ask user for a scaling factor between 0 and 1; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel> (width));

    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < width; col++)
        {
            // Set original RGB values
            int red, green, blue;
            red = image[row][col].red;
            green = image[row][col].green;
            blue = image[row][col].blue;
            
            // Set new scaled RGB values
            int newRed, newGreen, newBlue;
            newRed = (255 - (255 - red) * scaling_factor);
            newGreen = (255 - (255 - green) * scaling_factor);
            newBlue = (255 - (255 - blue) * scaling_factor);
            
            // Store new scaled RGB values in vector
            new_image[row][col] = {newRed, newGreen, newBlue};
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

/**
 * Darkens the image by scaling RGB values toward black based on a scaling factor.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @param scaling_factor a double value between 0 and 1 to darken the image
 * @return the processed 2D vector of Pixels after darkening
 */
vector<vector<Pixel>> process_9(vector<vector<Pixel>>& image, double scaling_factor)
{
    // Get the number of rows/columns from the input 2D vector; Ask user for a scaling factor between 0 and 1; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel> (width));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < width; col++)
        {
            // Set original RGB values
            int red, green, blue;
            red = image[row][col].red;
            green = image[row][col].green;
            blue = image[row][col].blue;
            
            // Set new scaled RGB values
            int newRed, newGreen, newBlue;
            newRed = red * scaling_factor;
            newGreen = green * scaling_factor;
            newBlue = blue * scaling_factor;
            
            // Store new scaled RGB values in vector
            new_image[row][col] = {newRed, newGreen, newBlue};
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

/**
 * Filters the image to black, white, red, green, or blue based on pixel brightness and max color.
 * Helper function for main()
 * @param image the original input 2D vector of Pixels
 * @return the processed 2D vector of Pixels with color classification
 */
vector<vector<Pixel>> process_10(vector<vector<Pixel>>& image)
{
    // Get the number of rows/columns from the input 2D vector; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel> (width));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < height; row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < width; col++)
        {
            // Set original RGB values
            int red, green, blue;
            red = image[row][col].red;
            green = image[row][col].green;
            blue = image[row][col].blue;
            
            // Finding the max color
            char max_color;
            if (blue > green && blue > red)
            {
                max_color = 'b';
            }
            else if (green > blue && green > red)
            {
                max_color = 'g';
            }
            else
            {
                max_color = 'r';
            }
            
            // Set new RGB values
            int newRed, newGreen, newBlue;
            int brightness = red + green + blue;
            if (brightness >= 550)
            {
                newRed = newGreen = newBlue = 255;
            }
            else if (brightness <= 150)
            {
                newRed = newGreen = newBlue = 0;
            }
            else if (max_color == 'r')
            {
                newRed = 255; newGreen = 0; newBlue = 0;
            }
            else if (max_color == 'g')
            {
                newRed = 0; newGreen = 255; newBlue = 0;
            }
            else
            {
                newRed = 0; newGreen = 0; newBlue = 255;
            }
            
            // Store new scaled RGB values in vector
            new_image[row][col] = {newRed, newGreen, newBlue};
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}
                 
/**
 * User menu for processing BMP images
 * @param the currently loaded filename
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
 * Ensures output filename is not the same as the input filename
 * and doesn't overwrite an existing file.
 * Helper function for main()
 * @param input_filename the current input image filename
 * @return a unique output filename with a .bmp extension
 */
string valid_output(const string& input_filename)
{
    string output;
    cin >> output;

    // Loop until valid, unique output filename is provided
    while (true)
    {
        // Prevent overwriting the original input file
        if (output == input_filename)
        {
            cout << "Cannot overwrite original image file. Please enter a different file name: ";
            cin >> output;
            continue;
        }

        // Add .bmp extension if missing
        if (output.length() < 4 || 
            !(tolower(output[output.length() - 4]) == '.' &&
              tolower(output[output.length() - 3]) == 'b' &&
              tolower(output[output.length() - 2]) == 'm' &&
              tolower(output[output.length() - 1]) == 'p'))
        {
            output += ".bmp";
        }

        // Check if file already exists
        ifstream file_check(output);
        if (file_check.good())
        {
            cout << "\nA file with that name already exists. Please enter a unique filename: \n";
            cin >> output;
            file_check.close();
            continue;
        }

        break; // valid and unique filename
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
 * Input check to ensure numbers are integers greater than 0
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
        cout << "Invalid input. Please enter an integer greater than 0. " << prompt << endl;
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
    cout << "Enter lowercase output filename : " << endl;
}

/**
 * Intro message for user; provides user the selected operation
 * Helper function for main()
 * @param operation user selects from main menu
 * @return nothing
 */ 
void intro_msg(const string& operation)
{
    cout << "\nYou've chosen the " << operation << " operation" << endl;
}

/**
 * Ensures the provided filename ends with a .bmp extension (case-insensitive)
 * Appends .bmp if missing
 * @param filename the input or output filename provided by the user
 * @return a string with guaranteed .bmp extension
 */
string ensure_bmp(const string& filename)
{
    if (filename.size() < 4 ||
        !(tolower(filename[filename.size() - 1]) == 'p' &&
          tolower(filename[filename.size() - 2]) == 'm' &&
          tolower(filename[filename.size() - 3]) == 'b' &&
          filename[filename.size() - 4] == '.'))
    {
        return filename + ".bmp";
    }
    return filename;
}

/**
 * Success/Error message after operation applied to BMP image
 * Helper function for main()
 * @param output the output BMP filename entered by the user
 * @param processed the processed 2D vector of Pixels after applying the operation
 * @param operation the operation description selected from the main menu
 * @return nothing
 */
void process_and_write(const string& output, vector<vector<Pixel>>& processed_image, const string& operation)
{
    if (write_image(output, processed_image)) 
    {
        cout << "Successfully applied " << operation << "!" << endl;
    }
    else 
    {
        cout << "Error: Failed to write image to '" << output << "'." << endl;
    }
}

const vector<string> operations =
{"change image", "vignette", "clarendon", "grayscale", "rotate 90 degrees", "rotate multiple 90 degrees", "enlarge", "high contrast", "lighten", "darken", "black, white, red, green, blue filters"};

int main()
{
    // Surfacing initial menu; Prompting user for input_filename;
    // Ensuring .bmp extension is present; Validating file exists by looping until
    // read_image returns non-empty vector; Declaring menu input variable for
    // operation menu - display_menu helper function
    string input_filename;
    vector<vector<Pixel>> image;

    cout << "\nCSPB 1300 Image Processing Application" << endl;

    do {
        cout << "Enter lowercase input filename: " << endl;
        cin >> input_filename;
        input_filename = ensure_bmp(input_filename);

        image = read_image(input_filename);

        if (image.empty()) 
        {
            cout << "\nError: File '" << input_filename << "' not found or could not be read. Please try again.\n" << endl;
        }

        } while (image.empty());

string input;
bool quit = false;
    
    do
    {
        // Helper function to surface operation menu and check for user input to quit
        display_menu(input_filename);
        cin >> input;
        
        if (input == "Q" || input == "q")
        {
            cout << "\nThank you for using my program!\n" << endl;
            cout << "***THANK YOU, SUPRIYA AND JUSTIN -- 1300 HAS BEEN A BLAST!!!***\n\n";
            cout << "Quiting...\n\n" << endl;
            quit = true;
            continue;
        }
        
        // Takes user input and converts to integer option
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
            cout << "Invalid selection. Please enter a number between 0 and " << operations.size() - 1 << ", or Q to quit." << endl;
            continue;  
        }
        
        // Helper function
        intro_msg(operations.at(option));
        
        if (option == 0)
        {
            do 
            {
                cout << "Enter new input BMP filename: " << endl;
                cin >> input_filename;
                input_filename = ensure_bmp(input_filename);
                image = read_image(input_filename);

                if (image.empty()) 
                {
                    cout << "\nError: File '" << input_filename << "' not found or could not be read. Please try again.\n" << endl;
        }
            } while (image.empty());
}
        
        else if (option == 1)
        {
            // Helper function for user to input output file name
            output_prompt(); 
            // Helper function to check output not overwriting input file name
            string output = valid_output(input_filename);  
            // Calling process_1 function
            auto processed = process_1(image);
            // Write the processed image to file and display success or error message using helper function
            process_and_write(output, processed, operations.at(option)); 
        }
        
        else if (option == 2)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Declaring double scaling_factor
            // Checking for valid scaling_factor input
            double scaling_factor = valid_sfactor();
            // Calling process_2 function
            auto processed = process_2(image, scaling_factor);
            process_and_write(output, processed, operations.at(option));
        }
        
        else if (option == 3)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Calling process_3 function
            auto processed = process_3(image);
            process_and_write(output, processed, operations.at(option));
        }
        
        else if (option == 4)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Calling process_4 function
            auto processed = process_4(image);
            process_and_write(output, processed, operations.at(option));
        }
        
        else if (option == 5)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Declaring int rotations
            // Resurfaces prompt until valid integer is provided
            int rotations = valid_integer("Enter number of 90 degree rotations: ");
            // Calling process_5 function
            auto processed = process_5(image, rotations);
            process_and_write(output, processed, operations.at(option));
        }
        
        else if (option == 6)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Declaring integers x_scale, y_scale
            // Using valid_integer helper function to check value is greater than zero
            int x_scale = valid_integer("Enter X scale: ");
            int y_scale = valid_integer("Enter Y scale: ");
            // Calling process_6 function
            auto processed = process_6(image, x_scale, y_scale);
            process_and_write(output, processed, operations.at(option));
        }
        
        else if (option == 7)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Calling process_7 function
            auto processed = process_7(image);
            process_and_write(output, processed, operations.at(option));
        }
        
        else if (option == 8)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Declaring double scaling_factor
            // Checking for valid scaling_factor input
            double scaling_factor = valid_sfactor();
            // Calling process_8 function
            auto processed = process_8(image, scaling_factor);
            process_and_write(output, processed, operations.at(option));
        }
        
        else if (option == 9)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Declaring double scaling_factor
            // Checking for valid scaling_factor input
            double scaling_factor = valid_sfactor();
            // Calling process_9 function
            auto processed = process_9(image, scaling_factor);
            process_and_write(output, processed, operations.at(option));
        }
        
        else if (option == 10)
        {
            output_prompt();
            string output = valid_output(input_filename);
            // Calling process_10 function
            auto processed = process_10(image);
            process_and_write(output, processed, operations.at(option));
        }
        
    } while (!quit);
    
    return 0;
}