//
//  main.cpp
//  JHImageInfoUtil
//
//  Created by Jeff Hodnett on 10/22/13.
//  Copyright (c) 2013 Jeff Hodnett. All rights reserved.
//

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <unordered_map>

struct AVSize {
    int width;
    int height;
};

#define AVSizeMake(w,h) {w,h}

static AVSize getImageSize(char *imageUrl)
{
    int width = 0, height = 0;
    FILE *image;
    int size, i = 0;
    unsigned char *data;
    
    image = fopen(imageUrl, "rb");
    if(!image) {
        printf("Unable to open image \n");
        return AVSizeMake(0, 0);
    }
    
    fseek(image,  0,  SEEK_END);
    size = (int)ftell(image);
    fseek(image,  0,  SEEK_SET);
    data = (unsigned char *)malloc(size);
    fread(data, 1, size, image);
    /* verify valid JPEG header */
    if(data[i] == 0xFF && data[i + 1] == 0xD8 && data[i + 2] == 0xFF && data[i + 3] == 0xE0) {
        i += 4;
        /* Check for null terminated JFIF */
        if(data[i + 2] == 'J' && data[i + 3] == 'F' && data[i + 4] == 'I' && data[i + 5] == 'F' && data[i + 6] == 0x00) {
            while(i < size) {
                i++;
                if(data[i] == 0xFF){
                    if(data[i+1] == 0xC0) {
                        height = data[i + 5]*256 + data[i + 6];
                        width = data[i + 7]*256 + data[i + 8];
                        break;
                    }
                }
            }
        }
    }
    fclose(image);
    
    return AVSizeMake(width, height);
}

static bool isValidImageName(char *fileName)
{
#warning check for other string types here if you need to
    char *jpg = strstr(fileName, ".jpg");
    return (jpg != NULL);
}

int main(int argc, const char * argv[])
{
    bool debug = true;
    const char *dirUrl;
    if(debug == true) {
#warning Change this path for your project or disable debug!
        dirUrl = "/Users/jeff/Desktop/JHImageInfoUtil/JHImageInfoUtil/JHImageInfoUtil/ImageExample/";
    }
    else {
        if(argc == 0) {
            printf("Please enter a file path!");
            return 0;
        }
        
        dirUrl = argv[0];
    }

    // Read directory
    DIR *pDIR;
    struct dirent *entry;
    int fileCount = 0;
    int dirUrlLen = (int)strlen(dirUrl);
    
    typedef std::unordered_map< std::string, int > hashmap;
    hashmap sizes;
    if( (pDIR = opendir(dirUrl)) ){
        while( (entry = readdir(pDIR)) ){
            if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ) {
                // Ensure file is image
                char *fileName = entry->d_name;
                if(isValidImageName(fileName)) {                    
                    int fileNameLen = (int)strlen(fileName);
                    char fileUrl[dirUrlLen+fileNameLen];
                    strcpy(fileUrl, dirUrl);
                    strcat(fileUrl, fileName);
                    
                    // Get size
                    AVSize size = getImageSize(fileUrl);
                    std::string width = std::to_string(size.width);
                    std::string height = std::to_string(size.height);

                    std::string hashKey = width+"x"+height;
                    int sizeCount = sizes[hashKey];
                    sizes[hashKey] = ++sizeCount;
                    
                    fileCount++;
                }
            }
        }
        closedir(pDIR);
    }
    
    // Print out info
    printf("File count %d \n------------\n", fileCount);
    for( hashmap::const_iterator i = sizes.begin(), e = sizes.end() ; i != e ; ++i ) {
        std::cout << i->first << " -> " << i->second << std::endl;
    }
    
    return 0;
}
