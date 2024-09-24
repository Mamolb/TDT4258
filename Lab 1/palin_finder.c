//Code that checks if a string is a palindrome or not
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
//Function to check if the string is a palindrome or not
bool isPalindrome(char str[])
{
    for(int i = 0; i < strlen(str)/2; i++)
    {
        if(str[i] != str[strlen(str) - i - 1])
        {
            //Need to check if the character is " " or ?
            if(str[i] != ' ' && str[strlen(str) - i - 1] != ' ' && str[i] != '?' && str[strlen(str) - i - 1] != '?')
            {
                return false;
            }
        }
    }
    return true;   
}

//Main function
int main()
{
    //Takes input from the user
    char str[100];
    printf("Enter a string: ");
    gets(str);
    //Converts the string to lowercase
    for (int i = 0; str[i] != '\0'; i++)
    {
        str[i] = tolower(str[i]);
    }
    //Checks if the string is a palindrome or not
    if(isPalindrome(str))
    {
        printf("The string is a palindrome\n");
    }
    else
    {
        printf("The string is not a palindrome\n");
    }

}