// Friedjof Noweck
// 2021-10-29 Fri

int stringLength(char* given_string);

int main()
{
    char str[8] = "12.50656";
    int length = 0;

    length = stringLength(str);

    return 0;
}

// Source => https://www.geeksforgeeks.org/length-string-using-pointers/
int stringLength(char* given_string)
{
    // variable to store the
    // length of the string
    int length = 0;

    while (*given_string != '\0') {
        length++;
        given_string++;
    }
  
    return length;
}
