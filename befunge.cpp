#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stack>

#define MAX_X 80
#define MAX_Y 25

/* 
    a little befunge93 interpreter for fun
     -- Alice

    From https://catseye.tc/view/Befunge-93/doc/Befunge-93.markdown 

    COMMAND         INITIAL STACK (bot->top)RESULT (STACK)
    -------         -------------           -----------------
    + (add)         <value1> <value2>       <value1 + value2>
    - (subtract)    <value1> <value2>       <value1 - value2>
    * (multiply)    <value1> <value2>       <value1 * value2>
    / (divide)      <value1> <value2>       <value1 / value2> (nb. integer)
    % (modulo)      <value1> <value2>       <value1 mod value2>
    ! (not)         <value>                 <0 if value non-zero, 1 otherwise>
    ` (greater)     <value1> <value2>       <1 if value1 > value2, 0 otherwise>
    > (right)                               PC -> right
    < (left)                                PC -> left
    ^ (up)                                  PC -> up
    v (down)                                PC -> down
    ? (random)                              PC -> right? left? up? down? ???
    _ (horizontal if) <boolean value>       PC->left if <value>, else PC->right
    | (vertical if)   <boolean value>       PC->up if <value>, else PC->down
    " (stringmode)                          Toggles 'stringmode'
    : (dup)         <value>                 <value> <value>
    \ (swap)        <value1> <value2>       <value2> <value1>
    $ (pop)         <value>                 pops <value> but does nothing
    . (output int)  <value>                 outputs <value> as integer
    , (output char) <value>                 outputs <value> as ASCII
    # (bridge)                              'jumps' PC one farther; skips
                                            over next command
    g (get)         <x> <y>                 <value at (x,y)>
    p (put)         <value> <x> <y>         puts <value> at (x,y)
    & (input int)                           <value user entered>
    ~ (input character)                     <character user entered>
    @ (end)                                 ends program
*/

struct ProgramCounter {
    short x = 0;
    short y = 0;
    short dx = 1;
    short dy = 0;
    

    void move() {
        x += dx;
        y += dy;

        // wrap around
        if (x<0) {
            x = MAX_X;
        } else if (x>=MAX_X) {
            x = 0;
        } else if (y<0) {
            y = MAX_Y;
        } else if (y>=MAX_Y) {
            y = 0;
        }
    }

    void set_direction_left() {
        dx = -1;
        dy = 0;
    }

    void set_direction_right() {
        dx = 1;
        dy = 0;
    }

    void set_direction_up() {
        dx = 0;
        dy = -1;
    }

    void set_direction_down() {
        dx = 0;
        dy = 1;
    }
};

struct BefungeProgram {
    ProgramCounter programCounter;
    std::stack<long> stack;
    char grid[MAX_Y][MAX_X];
    bool stringMode = false;

    BefungeProgram() {
        programCounter = ProgramCounter();
        stack = std::stack<long>();
        for (size_t y=0; y<MAX_Y; y++) {
            for (size_t x=0; x<MAX_X; x++) {
                grid[y][x] = 0;
            }
        }
    }

    long get_cursor() const {
        return grid[programCounter.y][programCounter.x];
    }

    long stack_value() {
        long val = 0;
        if (stack.size()>0) {
            val = stack.top();
            stack.pop();
        }
        return val;
    }

    void stack_duplicate_top() {
        long val = 0;
        if (stack.size()>0)
            val = stack.top();
        stack.push(val);
    }

    bool tick() {
        char cursor = get_cursor();
        
        if (cursor=='"') {
            stringMode = !stringMode;
        } else if (stringMode) {
            stack.push(cursor);
        } else {

            switch (cursor)
            {
            case '+':
                {
                    long a = stack_value();
                    long b = stack_value();
                    stack.push(a+b);
                }
                break;
            case '-':
                {
                    long a = stack_value();
                    long b = stack_value();
                    stack.push(b-a);
                }
                break;
            case '*':
                {
                    long a = stack_value();
                    long b = stack_value();
                    stack.push(a*b);
                }
                break;
            case '/':
                {
                    long a = stack_value();
                    long b = stack_value();
                    stack.push(b/a);
                }
                break;
            case '%':
                {
                    long a = stack_value();
                    long b = stack_value();
                    stack.push(b%a);
                }
                break;
            case '!':
                stack.push(stack_value() == 0 ? 1 : 0);
                break;
            case '`':
                {
                    long a = stack_value();
                    long b = stack_value();
                    stack.push(a>b ? 1: 0);
                }
                break;
            case '>':
                programCounter.set_direction_right();
                break;
            case '<':
                programCounter.set_direction_left();
                break;
            case '^':
                programCounter.set_direction_up();
                break;
            case 'v':
                programCounter.set_direction_down();
                break;
            case '?':
                {
                    int r = rand() % 4;
                    if (r==0) {
                        programCounter.set_direction_down();
                    } else if (r==1) {
                        programCounter.set_direction_up();
                    } else if (r==2) {
                        programCounter.set_direction_left();
                    } else {
                        programCounter.set_direction_right();
                    }
                }
                break;
            case '_':
                if(stack_value()==0) {
                    programCounter.set_direction_right();
                } else
                {
                    programCounter.set_direction_left();
                }
                break;
            case '|':
                if(stack_value()==0) {
                    programCounter.set_direction_down();
                } else
                {
                    programCounter.set_direction_up();
                }
                break;
            case ':':
                stack_duplicate_top();
                break;
            case '\\':
                {
                    long a = stack_value();
                    long b = stack_value();
                    stack.push(a);
                    stack.push(b);
                }
                break;
            case '$':
                stack_value();
                break;
            case '.':
                printf("%ld", stack_value());
                break;
            case ',':
                printf("%c", (char)stack_value());
                break;
            case '#':
                programCounter.move();
                break;
            case 'g':
                {
                    long x = stack_value();
                    long y = stack_value();
                    long v = grid[y][x];
                    stack.push(v);
                }
                break;
            case 'p':
                {
                    long v = stack_value();
                    long x = stack_value();
                    long y = stack_value();
                    grid[y][x] = v;
                }
                break;
            case '&':
                {
                    char buf[16];
                    fgets(buf, 16, stdin);
                    stack.push(atol(buf));
                }
                break;
            case '~':
                {
                    char buf[2];
                    fgets(buf, 2, stdin);
                    stack.push(*buf);
                }
                break;
            case '@':
                // terminate
                return false;
                break;
            
            default:
                if (cursor>='0'&&cursor<='9'){
                    stack.push(cursor-48);
                }
                break;
            }
        }
        
        programCounter.move();
        return true;
    }
};


/*const char* test_program = "64+\"!dlroW ,olleH\">:#,_@";

BefungeProgram loadTestProgram() {
    BefungeProgram befungeProgram = BefungeProgram();
    int x=0,y=0;

    for(const char* it = test_program; *it; ++it) {
        if (*it == '\n') {
            x = 0;
            y++;
            if (y>=MAX_Y) {
                break;
            }
            continue;
        }
        if (x<MAX_X) {
            befungeProgram.grid[y][x] = *it;
        }
        x++;
    }
    return befungeProgram;
}*/

bool loadProgram(FILE& filePtr, BefungeProgram& program) {
    int x = 0;
    int y = 0;
    char buf[1024];

    do {
        size_t bytesRead = fread(buf, sizeof(char), 1024, &filePtr);
        if (ferror(&filePtr) != 0) {
            return false;
        }
        if (bytesRead == 0) {
            break;
        }
        const char* it = buf;
        for (size_t i = 0;i<bytesRead;++i,++it) {
            if (*it == '\n') {
                x = 0;
                y++;
                if (y>=MAX_Y) {
                    break;
                }
                continue;
            }
            if (x<MAX_X) {
                program.grid[y][x] = *it;
            }
            x++;
        }
    } while(true);

    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Incorrect number of arguments. Usage: befunge [sourcePath]\n");
        return 1;
    }

    srand(time(NULL));

    BefungeProgram program = BefungeProgram();

    char* sourcePath = argv[1];
    FILE* sourceFile = fopen(sourcePath, "r");
    if (sourceFile == NULL) {
        printf("Unable to open source file: %s\n", sourcePath);
        return 1;
    };

    bool success = loadProgram(*sourceFile, program);
    if (!success) {
        printf("Error processing source file contents.\n");
        return 1;
    }

    int steps = 0;
    while(program.tick()) {
        steps++;
    }

    //printf("steps: %d\n", steps);
    printf("\n");
    return 0;
}
