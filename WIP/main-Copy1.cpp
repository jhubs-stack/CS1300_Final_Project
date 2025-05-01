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
    <ANSWER>
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


void displayMenu (const string& filename)
{
}


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


vector<vector<Pixel>> process_5(vector<vector<Pixel>>& image, int number)
{
    // Get the number of rows/columns from the input 2D vector; Allows user input for desired 90 degree rotations; Define value, angle variable; Define a new 2D vector the same size as the input 2D vector.  Calls helper function rotate_by_90;
    int height = image.size();
    int width = image[0].size();
    int angle = (number * 90) % 360;
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

vector<vector<Pixel>> process_6(vector<vector<Pixel>>& image, int x_scale, int y_scale)
{
    // Get the number of rows/columns from the input 2D vector; Implemented variables for x_scale, y_scale; User input for added height/width; Define a new 2D vector the same size as the input 2D vector.
    int height = image.size();
    int width = image[0].size();
    vector<vector<Pixel>> new_image(y_scale * height, vector<Pixel> (x_scale * width));
    
    // For each of the rows in the input 2D vector
    for (int row = 0; row < (y_scale * height); row++)
    {
        // For each of the columns in the input 2D vector
        for (int col = 0; col < (x_scale * width); col++)
        {
            new_image.at(row).at(col) = image.at(row / y_scale).at(col / x_scale);
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

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
            int gray_value = (red + green + blue) / 3;
            
            int newRed, newGreen, newBlue;
            if (gray_value >= 255 / 2)
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
            char max_color;
            
            // Finding the max color
            if (red > blue && red > green)
            {
                max_color = red;
            }
            else if (green > red && green > blue)
            {
                max_color = green;
            }
            else
            {
                max_color = blue;
            }
            
            // Set new RGB values
            int newRed, newGreen, newBlue;
            if (red + green + blue >= 550)
            {
                newRed = 255;
                newGreen = 255;
                newBlue = 255;
            }
            else if (red + green + blue <= 150)
            {
                newRed = 0;
                newGreen = 0;
                newBlue = 0;
            }
            else if (max_color == red)
            {
                newRed = 255;
                newGreen = 0;
                newBlue = 0;
            }
            else if (max_color == green)
            {
                newRed = 0;
                newGreen = 255;
                newBlue = 0;
            }
            else
            {
                newRed = 0;
                newGreen = 0;
                newBlue = 255;
            }
            
            // Store new scaled RGB values in vector
            new_image[row][col] = {newRed, newGreen, newBlue};
        }
    }
    // Return the new 2D vector after the nested for loop is complete
    return new_image;
}

                 
int main()
{
    vector<vector<Pixel>> tiny =
{
        {{  0,  5, 10},{ 15, 20, 25},{ 30, 35, 40},{ 45, 50, 55}},{{ 60, 65, 70},{ 75, 80, 85},{ 90, 95,100},{105,110,115}},{{120,125,130},{135,140,145},{150,155,160},{165,170,175}}
        
};
    vector<vector<Pixel>> result = process_6(tiny, 2, 3);
   
    for (int row = 0; row < result.size(); row++)
    {
        for (int col = 0; col < result[0].size(); col++)
        {
            cout << fixed << setw(3) << result[row][col].red << " ";
            cout << fixed << setw(3) << result[row][col].green << " ";
            cout << fixed << setw(3) << result[row][col].blue << " ";
        }
        cout << endl;
    }
    return 0;
}