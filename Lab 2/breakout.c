/***************************************************************************************************
 * DON'T REMOVE THE VARIABLES BELOW THIS COMMENT                                                   *
 **************************************************************************************************/
unsigned long long __attribute__((used)) VGAaddress = 0xc8000000; // Memory storing pixels
unsigned int __attribute__((used)) red = 0x0000F0F0;
unsigned int __attribute__((used)) green = 0x00000F0F;
unsigned int __attribute__((used)) blue = 0x000000FF;
unsigned int __attribute__((used)) white = 0x0000FFFF;
unsigned int __attribute__((used)) black = 0x0;
unsigned short light_blue = 0x07FF;  // Light blue in RGB565 (5 bits for red, 6 for green, 5 for blue)

unsigned short orange = 0xFD20;  // A common 16-bit value for orange
//TODO: Check if n_cols is correct
unsigned char n_cols = 10; // <- This variable might change depending on the size of the game. Supported value range: [1,18]

char *won = "You Won";       // DON'T TOUCH THIS - keep the string as is
char *lost = "You Lost";     // DON'T TOUCH THIS - keep the string as is
unsigned short height = 240; // DON'T TOUCH THIS - keep the value as is
unsigned short width = 320;  // DON'T TOUCH THIS - keep the value as is
char font8x8[128][8];        // DON'T TOUCH THIS - this is a forward declaration
/**************************************************************************************************/
/***
 * TODO: Define your variables below this comment
 */
const int block_height = 15;
const int block_width = 15;
const int ball_height = 7;
const int ball_width = 7;
const int ball_speed = 10;

const unsigned int checkRange = 30; //TODO: Change to size / speed or smth 
#define BAR_HIT_CHECK_RANGE checkRange
#define BLOCK_HIT_CHECK_RANGE  (width - n_cols * block_width - checkRange)
#define WALL_HIT_CHECK_RANGE_MIN checkRange
#define WALL_HIT_CHECK_RANGE_MAX (height - checkRange)
#define number_of_x_blocks (int)(320 / 15)
#define number_of_y_blocks (int)(240 / 15)
#define STARR_POSITION_BAR (int)(height / 2 - 45 / 2)
#define WallHitMin 10
#define WallHitMax height - 10
//MACRO to get PI
#define M_PI acos(-1.0)
/***
 * You might use and modify the struct/enum definitions below this comment
 */


typedef struct _block
{
    unsigned char destroyed;
    unsigned char deleted;
    unsigned int pos_x;
    unsigned int pos_y;
    unsigned int color;
} Block;

typedef enum _gameState
{
    Stopped = 0,
    Running = 1,
    Won = 2,
    Lost = 3,
    Exit = 4,
} GameState;
GameState currentState = Running;


typedef struct _ball
{
    unsigned int pos_x;
    unsigned int pos_y;
    double degree;
    unsigned int r_vector;
    unsigned int color;
} BallBlock;

typedef struct _hitBox
{
    unsigned int x_min;
    unsigned int x_max;
    unsigned int y_min;
    unsigned int y_max;
} HitBox;

typedef enum _gameregion
{
    CloseToWall,
    CloseToBlocks,
    CloseToBar,
    CloseToWallAndBar,
    CloseToWallAndBlock,
    NotClose,
}GameRegion;
//Globals that represent the gamepicture
//2D array of blocks //Think we should have this in main instead
Block blocksList[10][number_of_y_blocks]; //This takes some mem but it is the easiest way to keep track of the blocks
int BarPosition = 0;
BallBlock ball;
GameRegion currentRegion = NotClose;
/***
 * Here follow the C declarations for our assembly functions
 */

void SetPixel(unsigned int x_coord, unsigned int y_coord, unsigned int color);
void DrawBlock(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int color);
void DrawBar(unsigned int y);
int ReadUart();
void WriteUart(char c);
void ClearScreen();
/***
 * Now follow the assembly implementations
 */

//Assumes that every block should be black at start
//Assumes R2 = black, R4 = x, R5 = y
asm("ClearScreen: \n\t"
    "    PUSH {LR} \n\t" //Saves the link register(The LR (Link Register) holds the return address when a function call (BL, BLX) is made.)
    "    PUSH {R4, R5} \n\t" //I dont get why we do this seems useless to me
    "    LDR R2, =0x0000FFFF \n\t"  // Move the hexadecimal value 0x0000FFFF into register R2
    "    MOV R4, #0 \n\t" //intit x at 0
    "    MOV R5, #0 \n\t" //init y at 0
    "    LDR R3, =VGAaddress \n\t"
    "    LDR R3, [R3] \n\t"
    "InnerLoop: \n\t"
    "    LSL R0, R4, #1 \n\t" //R0 is the X position of the pixel placed correctly
    "    LSL R1, R5, #10 \n\t" //R1 is the Y position of the pixel placed correctly
    "    ADD R1, R0 \n\t" //R1 is the position of the pixel
    "    STRH R2, [R3, R1] \n\t" //Set the pixel to black
    //Check if we are at the end of the screen
    "    Add R4, R4, #1 \n\t" //Increment x with one
    "    CMP R4, #320 \n\t" //Compare x with 320
    "    BEQ OuterLoop \n\t" //If x == 320 go to outer loop (increment Y)
    "    B InnerLoop \n\t" //Else go to inner loop
    "OuterLoop: \n\t"
    "    MOV R4, #0 \n\t" //Reset x to 0
    "    Add R5, R5, #1 \n\t" //Increment y with one
    "    CMP R5, #240 \n\t" //Compare y with 240
    "    BEQ End \n\t" //If y == 240 go to end
    "    B InnerLoop \n\t" //Else go to inner loop
    "End: \n\t"
    "    POP {R4,R5}\n\t"
    "    POP {LR} \n\t"
    "    BX LR");

// assumes R0 = x-coord, R1 = y-coord, R2 = colorvalue
asm("SetPixel: \n\t"
    "LDR R10, =VGAaddress \n\t"
    "LDR R10, [R10] \n\t"
    "LSL R7, R1, #10 \n\t" //Changed to R7 Y position
    "LSL R8, R0, #1 \n\t"  //Changed to R8 X position
    "ADD R7, R8 \n\t"
    "STRH R2, [R10,R7] \n\t"
    "MOV R15, R14");

asm("DrawBlock: \n\t"
 // Save LR and relevant registers
    "   PUSH {LR, R2, R3, R4, R5, R6, R7, R8, R9} \n\t"
    // Load color (5th argument) from the stack
    "   LDR R4, [SP, #36] \n\t" // Load color from the correct stack offset
        // Debug: Print out R0, R1, R2, R3, R4 values
    "   MOV R0, R0 \n\t"                // Print or check R0 (pos_x)
    "   MOV R1, R1 \n\t"                // Print or check R1 (pos_y)
    "   MOV R2, R2 \n\t"                // Print or check R2 (width)
    "   MOV R3, R3 \n\t"                // Print or check R3 (height)
    "   MOV R4, R4 \n\t"                // Print or check R4 (color)
    //LOL
    "   MOV R9, R4 \n\t" //Move coler to temp register
    "   MOV R5, R2 \n\t" //Move width to R5
    "   MOV R2, R9 \n\t" //Move color to R2
    "   ADD R5, R0, R5 \n\t" //Add width to x
    "   ADD R6, R1, R3 \n\t" //Add height to y
    "   MOV R9, R0 \n\t" //Move start X to R9
    "BlockInnerLoop: \n\t"
    //Save values on stack for later use
    "   Bl SetPixel \n\t"
    //get old values from stack
    "   ADD R0, R0, #1 \n\t" //Increment x with one
    "   CMP R0, R5 \n\t" //Compare x with width
    "   BEQ BlockOuterLoop \n\t" //If x == width go to outer loop (increment Y)
    "   B BlockInnerLoop \n\t" //Else go to inner loop
    "BlockOuterLoop: \n\t"
    "   MOV R0, R9 \n\t" //Reset x to startValue
    "   ADD R1, R1, #1 \n\t" //Increment y with one
    "   CMP R1, R6 \n\t" //Compare y with height
    "   BEQ EndBlock \n\t" //If y == height go to end
    "   B BlockInnerLoop \n\t" //Else go to inner loop
    "EndBlock: \n\t"
    "   POP {LR, R2, R3, R4, R5, R6, R7, R8, R9} \n\t"
    "   BX LR");

asm("DrawBar: \n\t"
    " PUSH {LR,R2,R3,R4,R5,R6,R7,R8} \n\t" 
    //The bar is 7 x 45 pixels 
    " MOV R3, R0 \n\t" //Move y start to R3
    " ADD R4, R3, #45 \n\t" //When to stop y
    " ADD R5, R3, #15 \n\t" //When to change color
    " ADD R6, R3, #30 \n\t" //When to change color again
    " MOV R0, #0 \n\t"
    " MOV R1, R3 \n\t"
    " LDR R2, =0x07FF \n\t"
    "DrawBarInnerLoop:"
    "   MOV R7, R1 \n\t"
    "   MOV R8, R0 \n\t"
    "   Bl SetPixel \n\t"
    "   ADD R0, R0, #1\n\t"
    "   CMP R0, #7 \n\t"
    "   BEQ DrawBarOuterLoop \n\t"
    "   B DrawBarInnerLoop \n\t"
    "DrawBarOuterLoop: \n\t"
    "   MOV R0, #0 \n\t"
    "   ADD R1, R1, #1 \n\t"
    //Change color when y = 15 more than start value
    "   CMP R1, R5 \n\t" //Check if we should change color
    "   BNE CheckFor30 \n\t"
     "  LDR R2, =0x000000FF \n\t"  // Change to a different color (e.g., purple)
    "   B ContinueDrawing \n\t"
    
    "CheckFor30: \n\t"
    // Change color when R1 is 30
    "   CMP R1, R6 \n\t"
    "   BNE ContinueDrawing \n\t"
    "   LDR R2, =0x07FF \n\t"  // Change to another color (e.g., yellow)

    "ContinueDrawing: \n\t"
    "   CMP R1, R4 \n\t"  // Limit the height to 45 pixels
    "   BNE DrawBarInnerLoop \n\t"
    "POP {LR,R2,R3,R4,R5,R6,R7,R8} \n\t"
    "BX LR \n\t");

asm("ReadUart:\n\t"
    "LDR R1, =0xFF201000 \n\t"
    "LDR R0, [R1]\n\t"
    "BX LR");

// TODO: Add the WriteUart assembly procedure here that respects the WriteUart C declaration on line 46
asm("WriteUart: \n\t"
    "LDR R1, =0xFF201000 \n\t"
    "CMP R0, #0 \n\t" //Check if we have reaced the end of the string
    "BEQ EndWriteUart \n\t"
    "STR R0, [R1] \n\t"
    "EndWriteUart: \n\t"
    "BX LR \n\t"
    );
// TODO: Implement the C functions below
void draw_ball()
{
    DrawBlock(ball.pos_x, ball.pos_y, ball_width, ball_height, ball.color );
}
//Draw blocks I guess 
void draw_playing_field()
{
    for (int x = number_of_x_blocks - n_cols; x < number_of_x_blocks; x++)
    {
        for (int y = 0; y < number_of_y_blocks; y++)
        {
            if (blocksList[x][y].destroyed == 0)
            {
                DrawBlock(blocksList[x][y].pos_x, blocksList[x][y].pos_y, block_width, block_height, blocksList[x][y].color);
            }
            
        }
    }
    return;
}

void update_game_region()
{
    //Check ball position and update region
     if (ball.pos_y > WALL_HIT_CHECK_RANGE_MAX || ball.pos_y < WALL_HIT_CHECK_RANGE_MIN)
    {
        //Check are we also close to wall or bar
        if (ball.pos_x > BAR_HIT_CHECK_RANGE)
        {
            currentRegion = CloseToWallAndBar;
            return;
        }
        else if (ball.pos_x < BLOCK_HIT_CHECK_RANGE)
        {
            currentRegion = CloseToWallAndBlock;
            return;
        }
        else
        {
            currentRegion = CloseToWall;
            return;
        }
    }
    if (ball.pos_x < BAR_HIT_CHECK_RANGE)
    {
        //We are close to bar
        currentRegion = CloseToBar;
        return;   
    }
    else if (ball.pos_x > BLOCK_HIT_CHECK_RANGE)
    {
        //Close to block
        currentRegion = CloseToBlocks;
        return;
    }
}

void update_game_state()
{
    if (currentState != Running)
    {
        return;
    }

    // TODO: Check: game won? game lost?
    if (ball.pos_x == width)
    {
        currentState = Won; 
        return;
    }
    if(ball.pos_x <= 5)
    {
        currentState = Lost;
        return;
    }

    //Update balls position and direction
    double radians = ball.degree * (M_PI) / 180.0;
    ball.pos_x += ball_speed * sin(radians);
    ball.pos_y -= ball_speed * cos(radians);//TODO:Think this is correct
    // Hit Check with Blocks
    // HINT: try to only do this check when we potentially have a hit, as it is relatively expensive and can slow down game play a lot
    //Only check when we are in a region where a hit can happen
    switch (currentRegion)
    {
    case CloseToBar:
        //Check If we have hit bar
        check_if_barHit();
        break;
    case CloseToBlocks:
        //Check if we have hit Block
        check_if_blockHit();
        break;
    case CloseToWall:
        //Check if we have hit wall
        check_if_wallHit();
        break;
    case CloseToWallAndBar:
        //Check if we have hit Wall or Bar
        check_if_barHit();
        check_if_wallHit();
        break;
    case CloseToWallAndBlock:
        //Check if we have hit Wall or block.
        check_if_blockHit();
        check_if_wallHit();
    default:
        break;
    }
    return;
}

void check_if_wallHit()
{
    //HitBoxBall
    HitBox ballHitBox;
    ballHitBox.x_max = ball.pos_x + 7;
    ballHitBox.x_min = ball.pos_x;
    ballHitBox.y_min = ball.pos_y;
    ballHitBox.y_max = ball.pos_y + 7;
    if(ballHitBox.y_min <= WallHitMin || ballHitBox.y_max >= WallHitMax)
    {
        //We have hit a wall
        //TODO:Change degree to be more dynamic
        if(ball.degree <= 180) ball.degree = 180 - ball.degree;
        else if(ball.degree > 180)ball.degree = 540 - ball.degree;
    }
}

void check_if_barHit()
{
    // We have 3 sections for the bar and each should have a different bounce angle
    HitBox barLowerHitBox;  // This is actually at the top of the bar in this coordinate system
    barLowerHitBox.x_max = 15;
    barLowerHitBox.x_min = 0;
    barLowerHitBox.y_max = BarPosition + 15;
    barLowerHitBox.y_min = BarPosition; // Lower section is at the top in screen coordinates

    HitBox barCentralHitBox;
    barCentralHitBox.x_max = 15;
    barCentralHitBox.x_min = 0;
    barCentralHitBox.y_max = BarPosition + 30;
    barCentralHitBox.y_min = BarPosition + 15; // Central section is below the lower section

    HitBox barUpperHitBox;  // This is at the bottom of the bar
    barUpperHitBox.x_max = 15;
    barUpperHitBox.x_min = 0;
    barUpperHitBox.y_max = BarPosition + 45;
    barUpperHitBox.y_min = BarPosition + 30; // Upper section is at the bottom

    // HitBox for the ball
    HitBox ballHitBox;
    ballHitBox.x_max = ball.pos_x + 7;
    ballHitBox.x_min = ball.pos_x;
    ballHitBox.y_min = ball.pos_y;
    ballHitBox.y_max = ball.pos_y + 7;

    // Start simple: if you hit the bar, bounce it back at a dynamic angle
    if (ballHitBox.x_min <= barLowerHitBox.x_max && barLowerHitBox.y_min <= ballHitBox.y_max && ballHitBox.y_min <= barLowerHitBox.y_max)
    {
        ball.degree = 45; // Lower section hit (which is at the top in screen coordinates)
    }
    else if (ballHitBox.x_min <= barCentralHitBox.x_max && barCentralHitBox.y_min <= ballHitBox.y_max && ballHitBox.y_min <= barCentralHitBox.y_max)
    {
        ball.degree = 90; // Central section hit
    }
    else if (ballHitBox.x_min <= barUpperHitBox.x_max && barUpperHitBox.y_min <= ballHitBox.y_max && ballHitBox.y_min <= barUpperHitBox.y_max)
    {
        ball.degree = 135; // Upper section hit (which is at the bottom in screen coordinates)
    }
    return;
}
void check_if_blockHit()
{
    //HitBoxBall
    HitBox ballHitBox;
    ballHitBox.x_max = ball.pos_x + 7;
    ballHitBox.x_min = ball.pos_x;
    ballHitBox.y_min = ball.pos_y;
    ballHitBox.y_max = ball.pos_y + 7;  
    for (int x = number_of_x_blocks - n_cols; x < number_of_x_blocks; x++)
    {
        for (int y = 0; y < number_of_y_blocks; y++)
        {
            if (blocksList[x][y].destroyed == 0)
            {
                HitBox blockHitBox;
                blockHitBox.x_max = blocksList[x][y].pos_x + block_width;
                blockHitBox.x_min = blocksList[x][y].pos_x;
                blockHitBox.y_min = blocksList[x][y].pos_y;
                blockHitBox.y_max = blocksList[x][y].pos_y + block_height;

                //Check if hit above
                if(ballHitBox.y_max >= blockHitBox.y_min &&ballHitBox.y_min < blockHitBox.y_min && ballHitBox.x_max >= blockHitBox.x_min && ballHitBox.x_min <= blockHitBox.x_max)
                {
                    //We have hit a block from below
                    blocksList[x][y].destroyed = 1;
                    //Change direction of ball
                    if (ball.degree == 135) ball.degree = 45;
                    else if(ball.degree == 225) ball.degree = 135;
                    else
                    {
                        //ERROR SHOULD NEVER GET THIS
                        ball.degree =  270;
                    }
                    return;
                }
                //Check if hit from front
                if(ballHitBox.x_max >= blockHitBox.x_min && blockHitBox.y_min <= ballHitBox.y_max && ballHitBox.y_min <= blockHitBox.y_max)
                {
                    //We have hit a block
                    blocksList[x][y].destroyed = 1;
                    //Change direction of ball 
                    if (ball.degree == 45) ball.degree = 315;
                    else if(ball.degree == 135) ball.degree = 225;
                    else if(ball.degree == 90) ball.degree = 270;
                    else
                    {
                        //ERROR SHOULD NEVER GET THIS
                        ball.degree =  270;
                    }
                    return;
                }
                //Check if hit from below
                if(ballHitBox.y_min <= blockHitBox.y_max && ballHitBox.y_max > blockHitBox.y_max && ballHitBox.x_max >= blockHitBox.x_min && ballHitBox.x_min <= blockHitBox.x_max)
                {
                    //We have hit a block from above
                    blocksList[x][y].destroyed = 1;
                    //Change direction of ball
                    if (ball.degree == 45) ball.degree = 135;
                    else if(ball.degree == 315) ball.degree = 225;
                    else
                    {
                        //ERROR SHOULD NEVER GET THIS
                        ball.degree =  270;
                    }
                    return;
                }
            }
        }
    } 
}
void update_bar_state()
{
    int remaining = 0;
    int reedValue = 0;
    // Hint: This is draining the UART buffer
    do
    {
        unsigned long long out = ReadUart();
        if (!(out & 0x8000))
        {
            // not valid - abort reading
            return;
        }
        remaining = (out & 0xFF0000) >> 4;
        reedValue = (out & 0x000000FF);
        if (reedValue == 0x00000077)
        {
            //GoUp 
            if(BarPosition > 45) BarPosition -= 15;
            else BarPosition = 0;
        }
        if (reedValue == 0x00000073)
        {
            //Go down
            if(BarPosition < height - 45 - 15) BarPosition += 15;
            else BarPosition = height - 45;
        }
    } while (remaining > 0);

    // HINT: w == 77, s == 73
    // HINT Format: 0x00 'Remaining Chars':2 'Ready 0x80':2 'Char 0xXX':2, sample: 0x00018077 (1 remaining character, buffer is ready, current character is 'w')

}

void write(char *str)
{
    for(*str; *str != '\0'; str++)
    {
        WriteUart(*str);
    }
    // HINT: You can use the WriteUart function to write a single character to the UART

}

void play()
{
    // HINT: This is the main game loop
    // ClearScreen();
    while (1)
    {
        update_game_region();
        update_game_state();
        update_bar_state();
        if (currentState != Running)
        {
            break;
        }
        draw_playing_field();
        draw_ball();
        DrawBar(BarPosition); 
        delay(200000); // Insert delay so that its a bit easier on the eyes
        ClearScreen();
    }
    if (currentState == Won)
    {
        write(won);
    }
    else if (currentState == Lost)
    {
        write(lost);
    }
    else if (currentState == Exit)
    {
        return;
    }
    currentState = Stopped;
}

void reset()
{
    // Hint: This is draining the UART buffer

    //TODO:THIS DOES NOT ALWAYS WORK FOR SOME REASON
    int remaining = 0;
    do
    {
        unsigned long long out = ReadUart();
        if (!(out & 0x8000))
        {
            // not valid - abort reading
            return;
        }
        remaining = (out & 0xFF0000) >> 4;
        if ( (out & 0x000000FF) == 0x00000077 || (out & 0x000000FF) == 0x00000073)
        {
            //We have a new game
            currentState = Running;
            break;
        }
        //If we entered space we should exit THIS DOES NOT WORK IDK WHY
        if((out & 0x000000FF) == 0x0000000a)
        {
            char *str = "Exit Game";
            write(str);
            currentState = Exit;
            break;
        }
    } while (currentState != Running);
    char *str = "New Game LETS GO";
    write(str);
    // TODO: You might want to reset other state in here
    //Reset game for next run
    init_Ball();
    init_blockList();
    //Init Bar Position
    BarPosition = STARR_POSITION_BAR; //Start in middle of all positions
}

void wait_for_start()
{
    unsigned long long out = 0;
    do
    {
        //Wait for input
        out = ReadUart();
        out = out & 0x000000FF;
        //Maybe write somethign IDK
    }while(out != 0x00000077 && out != 0x00000073);
    
}

void init_blockList()
{
    // Define a set of colors to cycle through (excluding white)
    unsigned short colors[] = {blue, red, green, black, orange, light_blue};
    int num_colors = sizeof(colors) / sizeof(colors[0]);
    int color_index = 0;
    for (int x = number_of_x_blocks - n_cols; x < number_of_x_blocks; x++)
    {
        for (int y = 0; y < number_of_y_blocks; y++)
        {
            //Init all blocks with correct values
            blocksList[x][y].pos_x = x * block_width;    // Set the x position
            blocksList[x][y].pos_y = y * block_height;   // Set the y position
            blocksList[x][y].deleted = 0;
            blocksList[x][y].destroyed = 0; 
            // Assign the color, alternating for each row
            blocksList[x][y].color = colors[color_index];
            // Increment and wrap the color index to ensure the next block gets a different color
            color_index = (color_index + 1) % num_colors;
            if (color_index > 5)
            {
                color_index = 1;
            }
            //DrawBlock(block->pos_x, block->pos_y, block_width, block_height, block->color);
        }
    }
    return;
}

void init_Ball()
{
    //TODO: Fix start values if they are not nice
    ball.pos_x = width/2;
    ball.pos_y = 100;
    ball.degree = 270;
    ball.r_vector = cos(ball.degree) * ball.pos_x + sin(ball.degree) * ball.pos_y;
    ball.color = black;

}

void delay(volatile unsigned int count) {
    while (count--) {
        // Busy-wait loop to consume time
    }
}

int main(int argc, char *argv[])
{
    //ClearScreen();
    // HINT: This loop allows the user to restart the game after loosing/winning the previous game
    init_Ball();
    init_blockList();
    //Init Bar Position
    BarPosition = STARR_POSITION_BAR; //Start in middle of all positions
    while (1)
    {
        //Under follows real code LOL
        wait_for_start();
        play();
        reset();
        if (currentState == Exit)
        {
            break;
        }
    }
    return 0;
}

// THIS IS FOR THE OPTIONAL TASKS ONLY

// HINT: How to access the correct bitmask
// sample: to get character a's bitmask, use
// font8x8['a']
char font8x8[128][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0000 (nul)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0001
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0002
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0003
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0004
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0005
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0006
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0007
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0008
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0009
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000A
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000C
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+000F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0010
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0011
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0012
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0013
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0014
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0015
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0016
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0017
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0018
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0019
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001A
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001C
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001D
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001E
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+001F
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0020 (space)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // U+0021 (!)
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0022 (")
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // U+0023 (#)
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00}, // U+0024 ($)
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00}, // U+0025 (%)
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00}, // U+0026 (&)
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0027 (')
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00}, // U+0028 (()
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00}, // U+0029 ())
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00}, // U+002A (*)
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}, // U+002B (+)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+002C (,)
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, // U+002D (-)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+002E (.)
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, // U+002F (/)
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // U+0030 (0)
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // U+0031 (1)
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // U+0032 (2)
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // U+0033 (3)
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // U+0034 (4)
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // U+0035 (5)
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // U+0036 (6)
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // U+0037 (7)
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // U+0038 (8)
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // U+0039 (9)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+003A (:)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+003B (;)
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00}, // U+003C (<)
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00}, // U+003D (=)
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00}, // U+003E (>)
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00}, // U+003F (?)
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00}, // U+0040 (@)
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, // U+0041 (A)
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, // U+0042 (B)
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, // U+0043 (C)
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, // U+0044 (D)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, // U+0045 (E)
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, // U+0046 (F)
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, // U+0047 (G)
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, // U+0048 (H)
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0049 (I)
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, // U+004A (J)
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, // U+004B (K)
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, // U+004C (L)
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, // U+004D (M)
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, // U+004E (N)
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, // U+004F (O)
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, // U+0050 (P)
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, // U+0051 (Q)
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, // U+0052 (R)
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, // U+0053 (S)
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0054 (T)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, // U+0055 (U)
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // U+0056 (V)
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // U+0057 (W)
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, // U+0058 (X)
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, // U+0059 (Y)
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, // U+005A (Z)
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00}, // U+005B ([)
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00}, // U+005C (\)
    {0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00}, // U+005D (])
    {0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, // U+005E (^)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, // U+005F (_)
    {0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0060 (`)
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00}, // U+0061 (a)
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00}, // U+0062 (b)
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00}, // U+0063 (c)
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00}, // U+0064 (d)
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00}, // U+0065 (e)
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00}, // U+0066 (f)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // U+0067 (g)
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00}, // U+0068 (h)
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+0069 (i)
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E}, // U+006A (j)
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00}, // U+006B (k)
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // U+006C (l)
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00}, // U+006D (m)
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00}, // U+006E (n)
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00}, // U+006F (o)
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F}, // U+0070 (p)
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78}, // U+0071 (q)
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00}, // U+0072 (r)
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00}, // U+0073 (s)
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00}, // U+0074 (t)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00}, // U+0075 (u)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // U+0076 (v)
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00}, // U+0077 (w)
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00}, // U+0078 (x)
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // U+0079 (y)
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00}, // U+007A (z)
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00}, // U+007B ({)
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, // U+007C (|)
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00}, // U+007D (})
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+007E (~)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // U+007F
};