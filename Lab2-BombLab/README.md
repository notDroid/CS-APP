# Solutions With Notes
## Phase 1
- Call <strings_not_equal>
- Compare string length (<string_length>), correct length: 52
- The argument for this called is initially in %rbp, examining %rbp gives:

**Answer**: "Border relations with Canada have never been better."

## Phase 2
- Call <read_six_numbers>, then call \<__isoc99_sscanf@plt>
- If %eax <= 5 explode. scanf returns the # of inputs in the correct format.
- Therefore we need 6 spaced numbers.
- Then we enter a loop comparing the read numbers to some values.
- Inspecting we see that it checks if the first element is 1 then compares the current element to double the last one yeilding:

**Answer**: "1 2 4 8 16 32"

C reconstruction of phase 2:

```
void phase_2(char *input_line) {
    int input_numbers[6], i, cv;

    // Read input numbers, verify there are 6.
    n_read = read_six_numbers(input_line, input_numbers);
    if (n_read < 6)
        explode_bomb();
    
    // Compare the input numbers to the powers of 2: 1, 2, 4, 8, 16, 32.
    for (i = 0, cv = 1; i < 6; i++, cv*=2) {
        if (input_numbers[i] != cv)
            explode_bomb();
    }
}
```
## Phase 3
- Call scanf, make sure number of inputs > 1.
  - %rdi = input line
  - %rsi = some string pointer, format string: "%d %d"
  - %rdx = %rsp + 8, first output loc, int *xp
  - %rcx = %rsp + 12, second output loc, int *yp
- If x > 7 blow up. 
- Enter a switch statement with x as index, each case sets %eax = some value.
- If %eax = y bomb defused.
- I chose index x = 0, which sets %eax = 0xcf, so we need y = 0xcf, which is 207 in decimal.

**Answer**: "0 207"

## Phase 4
- Call scanf and confirm 2 inputs.
  - Format string: "%d %d"
  - First output loc: %rsp + 8, int *xp
  - Second output loc: %rsp + 12, int *yp
- x > 14 explode
- Call func4 with args: x, 0, 14. If return not 0 blow up. We can see from the function reconstruction that x must be 7.
- If y != 0 blow up, so y = 0.

**Answer**: "7 0"

C Reconstruction of func4:
```
// Returns 0 when x = (a + b) / 2
int func4(int x, int a, int b) {
    int middle, res;

    res = 0;
    middle = a + (b - a) / 2;

    if (x < middle)
        res = 2 * func4(x, a, middle - 1);
    else if (x > middle)
        res = 2 * func4(x, middle + 1, b) + 1;
    return res;
}
```

## Phase 5
- Check that input string length = 6.
- Go through a loop to construct some string from the input string.
- Defused if this created string is "flyers".
- Need to find input characters that map to the letters in flyers.
  - Examining the table of characters at 0x4024b0 from an offset of 0 to 15 we get:
![Table Values](https://github.com/user-attachments/assets/cc97b865-3d64-4060-8cc6-f5def6902d77)

| Offset   | Output Char | Ascii Value | Input Char |
| -------- | ------- | ------- | ------- |
| 0x9    | f    | 0x69    | i    |
| 0xf    | l    | 0x6f    | o    |
| 0xe    | y    | 0x6e    | n    |
| 0x5    | e    | 0x65    | e    |
| 0x6    | r    | 0x66    | f    |
| 0x7    | s    | 0x67    | g    |

**Answer**: "ionefg"

String creation loop:
```
char *table;

void decode_string(char *input, char *output){
    int i;

    for (i = 0; i < 6; i++)
        output[i] = table[input & 0xf];
}
```

## Phase 6
- Call read_six_numbers, save in int nums[6].
- Make sure there are no duplicates by running a nested for loop, also check that no numbers are less than or equal to 0 or greater than 6.
- Flip each value in nums: 7 - nums[i].
- Reorder a linked list according to nums.
- Check if the list is reordered such that its from greatest to least value.
- This means our flipped answer must be the indexes of the greatest to least values.

| Index   | Value |
| -------- | ------- | 
| 1    | 332    |
| 2    | 168    |
| 3    | 924    |
| 4    | 691    |
| 5    | 477    |
| 6    | 443    |

| Ordered Index   | Flipped |
| -------- | ------- | 
| 3    | 4    |
| 4    | 3    |
| 5    | 2    |
| 6    | 1    |
| 1    | 6    |
| 2    | 5    |

**Answer**: "4 3 2 1 6 5"

Phase 6 C reconstruction:
```
struct linked_list {
    int val;
    struct linked_list *next;
}

struct linked_list *linked_list_ptr;

void phase_6(char *input) {
    int *nums, i, j;
    long values[6];

    read_six_numbers[nums];

    // Make sure there are no duplicate numbers and every number between 1-6.
    for (i = 0; i < 6; i++) {
        if (nums[i] <= 0; || nums[i] > 6)
            explode_bomb();
        for (j = i + 1; j < 6; j++)
            if (nums[i] == nums[j])
                explode_bomb();
    }

    // Flip each value
    for (i = 0; i < 6; i++)
        nums[i] = 7 - nums[i];

    // Values maps i to a pointer to the nums[i]th linked list element.
    for (i = 0; i < 6; i++) {
        next = linked_list_ptr;
        for (j = 1; j < nums[i]; j++)
            next = next->next;
        values[i] = next;
    }


    // Reorder linked list to follow values entries.
    for (i = 1; i < 6; i++) {
        values[i - 1]->next = values[i]
    }
    values[5].next = NULL;

    // Check if linked list is ordered greatest val to least.
    for (i = 0; i < 5; i++)
        if (values[i].val < values[i + 1].val)
            explode_bomb();
}
```

## Secret Phase
- After 6 input lines have been read (after phase 6 complete) the phase_defused function skips a conditional jump.
- Then it reads a line from some location, printing the string at this location gives "7 0", the the phase 4 solution. It checks if this line has 3 entries first 2 numbers and then a string and if it does have this string it compares is to the string "DrEvil" and if they're the same it calls the secret_phase.
- First reads a number from stdin x, if x is 0 or greater than 0x3e9 then the bomb blows up.
- Call fun7 with a tree_node struct pointer in %rdi and x as arg 2, if return value 2 bomb defused.
- To get a return value of 2 We need to go left then right then set the value at this node is x, 36->8->22

**Answer**: 22

Reconstruction on fun7 in C:
```
struct tree_node {
    int val;
    struct tree_node *left;
    struct tree_node *right;
}

// return path taken to get to tree node with value = x (in binary)
// (read right to left) ex: 10 go left, then right
int fun7(struct tree *tp, int x) {
    if (tp == NULL)
        return -1;

    if (x < tp->val)
        return 2 * fun7(tp->left, x);
    if (x > tp->val)
        return 2 * fun7(tp->right, x) + 1;
    return 0;
}
```
