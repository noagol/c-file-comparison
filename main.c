#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define IDENTICAL 1
#define SIMILAR 3
#define DIFFERENT 2
#define SYSTEM_CALL_ERROR "Error in system call"


/**
 * Check if files opened successfully
 * @param file1 first file
 * @param file2 second file
 * @return 1 if succeed else 0
 */
int validateFilesOpened(int file1, int file2) {
    // if first file did not opened
    if (file1 < 0 && file2 >= 0) {
        // close second
        close(file2);
        return 0;
        // if second file did not opened
    } else if (file1 >= 0 && file2 < 0) {
        // close first
        close(file1);
        return 0;
        // if both file did not opened
    } else if (file1 < 0 && file2 < 0) {
        return 0;
    } else {
        // both opened
        return 1;
    }
}

/**
 * Check if current char is big letter
 * @param buff current char
 * @return 1 if the char is big letter else 0
 */
int isBigLetter(char *buff) {
    return (*buff >= 'A' && *buff <= 'Z');
}

/**
 * Check if current char is small letter
 * @param buff current char
 * @return 1 if the char is small letter else 0
 */
int isSmallLetter(char *buff) {
    return (*buff >= 'a' && *buff <= 'z');
}

/**
 * Read file and check if all the chars are spaces
 * @param file file to read
 * @return 1 if we read only spaces else return 0
 */
int readOnlySpaces(int file) {
    char c;
    // read 1 char
    ssize_t n = read(file, &c, 1);
    // if we read
    while (n > 0) {
        // if the char isn't space return 0
        if (!isspace(c)) {
            return 0;
        }
        // read another char
        n = read(file, &c, 1);
    }
    // read only spaces and return 1
    return 1;
}

/**
 * Read the two files and compare them
 * @param file1 the first file
 * @param file2 the second file
 * @return 1 - if the files identical
 *         2 - if the files different
 *         3 - if the files similar
 */
int readFiles(int file1, int file2) {
    char char1, char2;
    int isSimilar = 0;

    // read one char from each file
    ssize_t n1 = read(file1, &char1, 1);
    ssize_t n2 = read(file2, &char2, 1);

    // if we read from both files
    while (n1 > 0 && n2 > 0) {
        // if chars identical
        if (char1 == char2) {
            // read next chars
            n1 = read(file1, &char1, 1);
            n2 = read(file2, &char2, 1);
            // if the first char is space and the second isn't a space
        } else if (isspace(char1) && !isspace(char2)) {
            // read next char from first file
            n1 = read(file1, &char1, 1);
            // change bool to similar
            isSimilar = 1;
            // if the second char is space and the first isn't a space
        } else if (isspace(char2) && !isspace(char1)) {
            // read next char from second file
            n2 = read(file2, &char2, 1);
            // change bool to similar
            isSimilar = 1;
            // both chars is space
        } else if (isspace(char1) && isspace(char2)) {
            // read next chars
            n1 = read(file1, &char1, 1);
            n2 = read(file2, &char2, 1);
            // change bool to similar
            isSimilar = 1;
            // Check if first char is big and second is little
        } else if (isBigLetter(&char1) > 0 && isSmallLetter(&char2) > 0) {
            // compare them
            if (char1 == char2 - 32) {
                // change bool to similar
                isSimilar = 1;
                // read next chars
                n1 = read(file1, &char1, 1);
                n2 = read(file2, &char2, 1);
                // else return different
            } else {
                return DIFFERENT;
            }
            // Check if second char is big and first is little
        } else if (isSmallLetter(&char1) > 0 && isBigLetter(&char2) > 0) {
            // compare them
            if (char1 == char2 + 32) {
                // change bool to similar
                isSimilar = 1;
                // read next chars
                n1 = read(file1, &char1, 1);
                n2 = read(file2, &char2, 1);
                // else return different
            } else {
                return DIFFERENT;
            }
            // else return different
        } else {
            return DIFFERENT;
        }
    }

    // Error
    if (n1 < 0 || n2 < 0) {
        write(2, SYSTEM_CALL_ERROR, 21);
        return -1;
    }

    // If we not finish to read first file
    if (n1 > 0 && n2 == 0) {
        // check if the rest of the file is spaces - return similar
        if (isspace(char1) && readOnlySpaces(file1)) {
            return SIMILAR;
        }
        // return different
        return DIFFERENT;
        // If we not finish to read second file
    } else if (n1 == 0 && n2 > 0) {
        // check if the rest of the file is spaces - return similar
        if (isspace(char2) && readOnlySpaces(file2)) {
            return SIMILAR;
        }
        // return different
        return DIFFERENT;
        // if we finish to read both files and bool is similar
    } else if (n1 == 0 && n2 == 0 && isSimilar) {
        return SIMILAR;
    } else {
        return IDENTICAL;
    }

}

/**
 * Main func to check if two file
 * are identical
 * @param argc num of args
 * @param argv the files
 * @return 1 - if the files identical
 *         2 - if the files different
 *         3 - if the files similar
 */
int main(int argc, char **argv) {

    // check num of args is 3
    if (argc != 3) {
        fprintf(stderr, "error input\n");
        return -1;
    }

    // get two files
    char *firstFile = argv[1];
    char *secondFile = argv[2];

    // open the files
    int file1, file2;
    file1 = open(firstFile, O_RDONLY);
    file2 = open(secondFile, O_RDONLY);

    int result;

    // check if the files did not opened successfully
    if (validateFilesOpened(file1, file2) == 0) {
        write(2, SYSTEM_CALL_ERROR, 21);
        return -1;
    }

    // compare files and return result
    result = readFiles(file1, file2);
    // close the files
    close(file1);
    close(file2);

    // return the result of the compare
    return result;
}

