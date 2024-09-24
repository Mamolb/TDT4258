// palin_finder.s, provided with Lab1 in TDT4258 autumn 2024
.global _start


// Please keep the _start method and the input strings name ("input") as
// specified below
// For the rest, you are free to add and remove functions as you like,
// just make sure your code is clear, concise and well documented.

_start:
	// Here your execution starts
    ldr r0, =input     //Load the address of the input string into r0
    mov r1, #0         //Initialize a counter r1 to 0(This will be the len of the string after the check_input function)
    mov r3, #0         //Counter to be used in the conver_lower function
    b check_input      //Branch to the function check_input
	b _exit

check_input:
	// You could use this symbol to check for your input length
	// you can assume that your input string is at least 2 characters 
	// long and ends with a null byte
    ldrb r2, [r0, r1]   // Load the next byte (character) into r2
    cmp r2, #0          // Compare it with the null character (0)
    beq conver_lower    // If it's the null character, we've reached the end
    add r1, r1, #1      // Increment the counter
    b check_input       // Repeat until the null character is found

conver_lower:
    ldrb r2, [r0, r3]   // Load the next byte (character) into r2
    cmp r2, #0          // Compare it with the null character (0)
    beq check_palindrom // If it's the null character, we're done

    cmp r2, #'A'        // Compare the character with 'A'
    blt skip_convert    // If it's less than 'A', skip conversion
    cmp r2, #'Z'        // Compare the character with 'Z'
    bgt skip_convert    // If it's greater than 'Z', skip conversion

    add r2, r2, #0x20   // Convert to lowercase by adding 0x20
    strb r2, [r0, r3]   // Store the converted character back to the string
    add r3, r3, #1      // Increment the counter (move to the next character)
    b conver_lower      // Repeat until the null character is found

skip_convert:
    strb r2, [r0, r3]   // Store the converted character back to the string
    add r3, r3, #1       // Increment the counter (move to the next character)
    b conver_lower      // Repeat until the null character is found

check_palindrom:
	// Here you could check whether input is a palindrom or not
    // When we return here the r1 register contains the length of the string and all characters are lowercase
    mov r3, #0         //Reset the counter r3 to 0
    mov r4, r1         //Copy the length of the string to r4
    sub r4, r4, #1     //Decrement r4 by 1 to get the index of the last character
    //Check if the string is a palindrom character by character

check_palindrom_loop:
    ldrb r5, [r0, r3]   // Load the next byte (character) into r5
    ldrb r6, [r0, r4]   // Load the next byte (character) into r6
    //If any of the characters is a space or question mark, skip it
    cmp r5, #' '        // Compare the character with ' '
    beq skip_char       // If it's a space, skip it
    cmp r5, #'?'        // Compare the character with '?'
    beq skip_char       // If it's a question mark, skip it
    cmp r6, #' '        // Compare the character with ' '
    beq skip_char       // If it's a space, skip it
    cmp r6, #'?'        // Compare the character with '?'
    beq skip_char       // If it's a question mark, skip it

    //None of the values are spaces or question marks, compare the characters
    cmp r5, r6          // Compare the characters
    bne is_no_palindrom // If they are not equal, the string is not a palindrom
    add r3, r3, #1      // Increment the counter for the first character
    sub r4, r4, #1      // Decrement the counter for the second character
    cmp r3, r4          // Compare the counters
    blt check_palindrom_loop // Repeat until the counters meet

    b is_palindrom      // If the counters meet, the string is a palindrom

skip_char: 
    add r3, r3, #1      // Increment the counter for the first character
    sub r4, r4, #1      // Decrement the counter for the second character
    cmp r3, r4          // Compare the counters
    blt check_palindrom_loop // Repeat until the counters meet

    b is_palindrom      // If the counters meet, the string is a palindrom


	
is_palindrom:
	// Switch on only the 5 rightmost LEDs
    ldr r1, =0xFF200000  // Load the address of the redLeds port into r1
    mov r2, #0x1F        // Value to turn on the 5 rightmost LEDs
    str r2, [r1]         // Write the value to the redLeds port
	// Write 'Palindrom detected' to UART
    ldr r0, =successString // Load the string 'Palindrome detected' into r0
    ldr r1, =0xFF201000    // Load the address of the UART port into r1    
    b write_success
	
	
is_no_palindrom:
	// Switch on only the 5 leftmost LEDs
    ldr r1, =0xFF200000  // Load the address of the redLeds port into r1
    mov r2, #0x3E0       // Value to turn on the 5 leftmost LEDs
    str r2, [r1]       // Write the value to the redLeds port
	// Write 'Not a palindrom' to UART
    ldr r0, =noSuccessString // Load the string 'Not a palindrom' into r0
    ldr r1, =0xFF201000    // Load the address of the UART port into r1
    b write_nosuccess
	
write_success:
    // Write 'Palindrom detected' to UART
    ldrb r2, [r0], #1     // Load the next byte of the string into r2 and increment r0
    cmp r2, #0            // Check if we have reached the null terminator
    beq _exit             // If so, we're done
    str r2, [r1]          // Write the character to the UART
    b write_success        // Loop to write the next character

write_nosuccess:
    // Write 'Not a palindrom' to UART
    ldrb r2, [r0], #1     // Load the next byte of the string into r2 and increment r0
    cmp r2, #0            // Check if we have reached the null terminator
    beq _exit             // If so, we're done
    str r2, [r1]          // Write the character to the UART
    b write_nosuccess     // Loop to write the next character
_exit:
	// Branch here for exit
	b .
	
.data
.align
	// This is the input you are supposed to check for a palindrom
	// You can modify the string during development, however you
	// are not allowed to change the name 'input'!
	input: .asciz "Grav ned den varg"
    successString: .asciz "Palindrome detected\n"
    noSuccessString: .asciz "Not a palindrom\n"
.end