# rush-02 — Number to Words Converter

> แปลงตัวเลขเป็นคำภาษาอังกฤษ รองรับตัวเลขขนาดใหญ่มากถึง undecillion (10³⁶)  
> Converts any number into English words, supporting numbers up to undecillion (10³⁶).

---

# 🇬🇧 English

## How to Compile & Run

```bash
make fclean && make

# Basic usage
./rush-02 numbers.dict 42
# → forty-two

# One argument (uses default dict: numbers.dict)
./rush-02 42
# → forty-two

# Stdin mode (bonus)
./rush-02 numbers.dict -
42
forty-two
^D

# Different language dict (bonus)
./rush-02 numbers_th.dict 42
# → สี่สิบ สอง
```

---

## File Structure

```
ex00/
├── rush02.h          ← Shared types, structs, and function declarations
├── main.c            ← Entry point
├── utils.c           ← Core string helpers (strlen, strdup, strjoin...)
├── str_utils.c       ← More string helpers (trim, isdigit, strcmp, strdup_n)
├── bignum.c          ← Big number arithmetic (cmp, div, strip zeros)
├── convert.c         ← Core conversion logic (recursive)
├── dict.c            ← Dictionary: store, look up, find floor key
├── dict_parse.c      ← Dictionary: read file, parse lines
├── dict_load.c       ← Dictionary: entry point for loading
├── bonus.c           ← Stdin reading mode
├── bonus_syntax.c    ← Word classifiers (scale, tens, ones, separator)
├── bonus_build.c     ← Proper English syntax builder
└── numbers.dict      ← The number-to-word dictionary
```

---

## `rush02.h` — The Shared Menu

Every `.c` file includes this header. Think of it as a table of contents — it tells each file what types and functions exist in the rest of the project.

```c
# define DICT_FILE "numbers.dict"   // default dictionary filename
# define MAX_ENTRIES 1024           // max entries the dictionary can hold
# define BUF_SIZE 65536             // max file size to read (64 KB)
# define MAX_DIGITS 64              // max digits in a number
```

**Why `typedef unsigned long long t_ull`?**  
`unsigned long long` is the largest integer type in standard C. We alias it to `t_ull` for brevity. Even though we later moved to string-based bignum, `t_ull` is still kept in the header for compatibility.

**Why string keys in `t_entry` instead of `t_ull`?**  
The dictionary contains numbers like `1000000000000000000000000000000000000` (undecillion = 10³⁶), which is far larger than `t_ull` can hold (~18 quintillion = 1.8×10¹⁹). By storing keys as `char *` strings, we support arbitrarily large numbers.

```c
typedef struct s_entry
{
    char    *key;    // the number as a string, e.g. "1000000"
    char    *value;  // the word, e.g. "million"
} t_entry;

typedef struct s_dict
{
    t_entry entries[MAX_ENTRIES];  // fixed array of up to 1024 entries
    int     size;                  // how many entries are currently stored
} t_dict;
```

---

## `utils.c` — Core String Helpers

### `ft_strlen` — Count characters

```c
int ft_strlen(char *str)
```

Walks character by character until hitting `'\0'`. Returns the count.

**Why not use the standard `strlen`?** The 42 norm only allows permitted functions. `strlen` is not in the allowed list, so we reimplement it.

---

### `ft_strdup` — Copy a string into new memory

```c
char *ft_strdup(char *s)
```

Allocates fresh memory with `malloc`, copies every character, adds `'\0'` at the end. Returns the new copy.

**Why?** In C, strings are just pointers to memory. If you want your own independent copy that won't be affected when the original changes or is freed, you must allocate new memory and copy it yourself.

---

### `ft_strjoin` — Glue two strings together

```c
char *ft_strjoin(char *a, char *b)
```

Allocates `len(a) + len(b) + 1` bytes, copies `a` then `b`, adds `'\0'`. Returns the combined string.

**Why +1?** The `'\0'` terminator needs its own byte.

---

### `ft_strjoin_sp` — Glue two strings with a space between

```c
char *ft_strjoin_sp(char *a, char *b)
```

Used by `convert` to build phrases like `"one"` + `"thousand"` → `"one thousand"`.

Steps:
1. Join `a` + `" "` → `temp`
2. Join `temp` + `b` → `result`
3. `free(temp)` — intermediate string no longer needed

**Why free `temp`?** Every `malloc` must have exactly one matching `free`. Failing to free `temp` would be a memory leak.

---

### `ft_putstr` — Print a string

```c
void ft_putstr(char *s)
```

Uses the low-level `write(1, ...)` syscall to print directly to stdout. `printf` is not in the allowed functions list.

---

## `str_utils.c` — More String Helpers

### `ft_trim` — Remove leading/trailing spaces and tabs

```c
char *ft_trim(char *str)
```

The dictionary file might have lines like `"  1000  :  thousand  "`. This function strips the extra whitespace from both ends.

**Why two passes?**  
- `start` pointer moves right until hitting a non-space character
- `end` pointer moves left until hitting a non-space character
- Only the content between them is copied

**Why `end < start` check?** Handles strings that are all spaces — returns `""` instead of crashing.

---

### `ft_isdigit` — Is this character a digit?

```c
int ft_isdigit(char c)
```

Returns `1` if `c` is between `'0'` and `'9'`, `0` otherwise.

**Why not use `isdigit()` from `<ctype.h>`?** Not in the allowed functions. We reimplement it using ASCII arithmetic — `'0'` is ASCII 48, `'9'` is ASCII 57.

---

### `ft_str_isnum` — Is the entire string digits only?

```c
int ft_str_isnum(char *str)
```

Returns `1` only if every character passes `ft_isdigit`, and the string is not empty.

**Why check empty string?** An empty string `""` would pass the loop with no failures, incorrectly returning `1`. The early `!str[i]` guard prevents this.

---

### `ft_strcmp` — Compare two strings

```c
int ft_strcmp(char *a, char *b)
```

Returns `0` if equal, positive if `a > b`, negative if `a < b` — same convention as the standard `strcmp`.

**Why do we need this?** To look up dictionary keys by exact match. Dictionary keys are stored as strings (e.g. `"1000"`), so we need string comparison, not integer comparison.

---

### `ft_strdup_n` — Copy exactly N characters

```c
char *ft_strdup_n(char *s, int n)
```

Like `ft_strdup` but copies at most `n` characters. Used by `bn_div` to split a number string at a specific position.

**Why not just use `ft_strdup` with a null-terminated slice?** The source string `n` is not null-terminated at the split point — it's the middle of a longer string. We need to copy exactly `n` chars and then add `'\0'` ourselves.

---

## `bignum.c` — Big Number Arithmetic

This is the most important innovation of the project. Instead of storing numbers as `t_ull` (max ~18 quintillion), we store them as `char *` strings and do arithmetic on the strings directly.

**Why strings?** The dictionary contains `undecillion = 10³⁶`, which needs 37 digits. `t_ull` holds at most 20 digits. There is no standard C integer type large enough, so we implement our own string-based arithmetic.

---

### `strip_zeros` — Remove leading zeros

```c
char *strip_zeros(char *s)
```

Takes ownership of `s`, removes leading zeros, returns a new clean string, and frees the original.

```
"0042" → "42"
"0"    → "0"   ← never removes the last digit
"100"  → "100" ← no leading zeros, copies as-is
```

**Why does it `free(s)`?** This function takes ownership of the input. The caller passes a `malloc`'d string and gets back a new `malloc`'d string. The original is freed inside to avoid leaks. This is the "consume and replace" pattern.

**Why `start < len - 1`?** To always keep at least one character. Without this guard, `"0"` would strip to `""`, which breaks everything downstream.

---

### `bn_is_zero` — Check if a number string equals zero

```c
int bn_is_zero(char *n)
```

Skips all `'0'` characters. If nothing remains (hits `'\0'`), the number is zero.

```
"0"    → 1 (true)
"000"  → 1 (true)
"042"  → 0 (false)
```

**Why not just `ft_strcmp(n, "0")`?** Because remainders from `bn_div` might be `"00"` or `"000"` before `strip_zeros` runs. `bn_is_zero` handles those cases correctly.

---

### `bn_cmp` — Compare two number strings

```c
int bn_cmp(char *a, char *b)
```

Returns positive if `a > b`, negative if `a < b`, zero if equal.

**Step 1 — Compare lengths:**
```c
if (la != lb)
    return (la - lb);
```
A longer string represents a larger number. `"100"` (len 3) > `"99"` (len 2), so `3 - 2 = +1`. This handles the case where direct character comparison would fail — `'9' > '1'` but 99 < 100.

**Step 2 — Same length, compare character by character:**
```c
while (a[i] && a[i] == b[i])
    i++;
return ((int)(a[i] - b[i]));
```
Since both strings have the same number of digits, the first differing digit determines which is larger.

**Why this matters:** `dict_floor` uses `bn_cmp` to find the largest dictionary key that fits inside the input number. Getting this wrong produces gibberish output.

---

### `bn_div` — Divide a big number by a power of 10

```c
char *bn_div(char *n, char *divisor, char **remainder)
```

This is the cleverest function in the project. It exploits the fact that **every key in the dictionary is a power of 10** (1, 10, 100, 1000, 1000000...).

Dividing by a power of 10 is just slicing the string:

```
"1234" ÷ "1000"  →  quotient = "1",   remainder = "234"
"100000" ÷ "100" →  quotient = "1000", remainder = "0"
```

**How the split works:**
```c
ln = ft_strlen(n);          // length of dividend
ld = ft_strlen(divisor);    // length of divisor (= number of digits)
split = ln - (ld - 1);      // where to cut
```

For `"1234"` (len=4) ÷ `"1000"` (len=4):
- `split = 4 - (4-1) = 1`
- Quotient = first 1 character = `"1"`
- Remainder = remaining characters = `"234"`

**Why `ld - 1` not `ld`?** Because `1000` has 4 digits but represents 10³. The quotient takes `ln - (ld-1)` digits, not `ln - ld`.

**Why not implement full long division?** Because we don't need it. The dictionary keys are always exact powers of 10, so string slicing gives the exact quotient and remainder without any looping. This makes the function O(1) instead of O(n²).

---

## `dict.c` — Dictionary Storage

### `dict_init` — Initialize an empty dictionary

```c
void dict_init(t_dict *d)
```

Sets `d->size = 0`. The dictionary starts empty; no memory allocation needed because the array is part of the struct.

---

### `dict_add` — Add or update an entry

```c
int dict_add(t_dict *d, char *key, char *val)
```

First scans existing entries to check if the key already exists. If it does, replaces the value (freeing the old one). If the dictionary is full, returns `-1`. Otherwise appends the new entry.

**Why scan first?** The dictionary file could theoretically have duplicate keys. We handle this gracefully by updating instead of duplicating.

---

### `dict_value_get` — Exact key lookup

```c
char *dict_value_get(t_dict *d, char *key)
```

Linear scan using `ft_strcmp`. Returns the value string if found, `NULL` otherwise.

**Why linear scan?** With at most 1024 entries, binary search would be premature optimization. Linear scan is simple, correct, and fast enough.

---

### `dict_floor` — Find the largest key ≤ n

```c
char *dict_floor(t_dict *d, char *n)
```

This is the core lookup used by `convert`. Given a number `n`, it finds the largest dictionary key that is less than or equal to `n`.

```
n = "1042", keys include "1000", "100", "40", "2"...
dict_floor returns "1000"
```

**Why "floor"?** Like the mathematical floor function — the largest value that doesn't exceed `n`. This determines which unit to use next in the conversion.

**Why scan all entries?** The dictionary is unsorted (entries can be in any order per the subject). We must check all of them to find the maximum valid key.

---

### `dict_free` — Release all dictionary memory

```c
void dict_free(t_dict *d)
```

Frees both `key` and `value` strings for every entry. Keys are `malloc`'d strings (not original pointers) because `store_entry` in `dict_parse.c` calls `ft_trim` which allocates new memory.

---

## `dict_load.c` — Dictionary Load Entry Point

### `dict_load` — Load the whole dictionary file

```c
int dict_load(char *filename, t_dict *d)
```

The public entry point for loading a dictionary. Calls `read_file_buf` to get the raw file content, passes it to `process_lines` to parse every entry, then frees the buffer.

**Why a separate file?** `dict_parse.c` already had 5 functions (the norminette maximum per file). `dict_load` was moved to its own file to stay within the limit.

**Return value:** `0` on success, `-1` if the file could not be read or a line could not be parsed.

---
## `dict_parse.c` — File Reading and Parsing

### `open_and_alloc` — Open file and allocate buffer

```c
static char *open_and_alloc(char *filename, int *fd)
```

Opens the file in read-only mode and allocates a 64 KB buffer. If allocation fails, closes the file before returning `NULL` to avoid a file descriptor leak.

**Why `static`?** This is a private helper — it should not be visible outside `dict_parse.c`. `static` enforces this at the compiler level.

---

### `read_file_buf` — Read entire file into memory

```c
char *read_file_buf(char *filename)
```

Reads the file in chunks (the `read` syscall may not return all bytes at once) until the buffer is full or end-of-file. Adds `'\0'` at the end to make it a proper C string.

**Why read in a loop?** The `read` syscall on most systems returns as many bytes as are available, which might be less than requested — especially on pipes or slow filesystems. The loop ensures we read everything.

**Why 64 KB limit?** A reasonable upper bound for dictionary files. The reference dictionary is less than 1 KB.

---

### `store_entry` — Validate and store one key-value pair

```c
static int store_entry(char *key_str, char *val_str, t_dict *d)
```

Trims both sides, validates the key is numeric, then calls `dict_add`. If the key overflows (too many digits for our system to care about), `ft_str_isnum` still passes but we store it as a string — no overflow possible.

**Key design decision:** We no longer convert the key to `t_ull`. We store it directly as a string. This is why undecillion works — the string `"1000000000000000000000000000000000000"` is stored as-is.

---

### `parse_line` — Split `"number : word"` into two parts

```c
static int parse_line(char *line, t_dict *d)
```

Walks forward to find `':'`, replaces it with `'\0'` to split the line in-place into key and value halves, then passes both to `store_entry`.

**Why in-place split?** Avoids a `malloc`. We're modifying the buffer that was already allocated by `read_file_buf`. After parsing, the buffer is freed anyway.

---

### `process_lines` — Walk through all lines

```c
int process_lines(char *buf, t_dict *d)
```

Scans the buffer for `'\n'` characters, replacing each with `'\0'` to isolate lines, then calls `parse_line` on each non-empty line.

**Why handle last line separately?** If the file doesn't end with a newline (common), the last line is never terminated by `'\n'`. The `else` branch handles this edge case explicitly.

---

## `convert.c` — The Conversion Engine

This is the recursive heart of the program. It breaks a number string into words by repeatedly finding the largest matching dictionary key and recursing on the remainder.

---

### `bn_sub_loop` — Subtract two number strings (loop)

```c
static char *bn_sub_loop(char *buf, char *n, char *key)
```

Implements grade-school subtraction from right to left, handling borrowing.

```
  42
- 40
----
   2
```

**Why separate from `bn_sub_str`?** The loop needs a `char buf[]` array on the stack (not heap). Separating the loop into its own function keeps `bn_sub_str` under the 25-line norm limit.

---

### `bn_sub_str` — Subtract and return a new string

```c
static char *bn_sub_str(char *n, char *key)
```

Calls `bn_sub_loop` on a stack buffer, then copies the result to heap with `ft_strdup`, and strips leading zeros.

**Why a stack buffer?** `char buf[MAX_DIGITS + 1]` is allocated on the stack — no `malloc`, no `free` needed. It's temporary and only used within this function.

**Why call `strip_zeros` after?** Subtraction can produce leading zeros: `42 - 40 = 02`. We need `"2"` not `"02"` for correct comparison and lookup.

---

### `convert_large` — Convert numbers ≥ 100

```c
static char *convert_large(char *n, char *key, char *keyword, t_dict *d)
```

Handles numbers like `1234567` where the best key is `1000000` (million).

**Algorithm:**
```
n = "1234567", key = "1000000", keyword = "million"

bn_div("1234567", "1000000") → quotient="1", remainder="234567"

convert("1") = "one"
"one" + "million" = "one million"

convert("234567") = "two hundred thirty four thousand five hundred sixty seven"
"one million" + "two hundred..." = "one million two hundred..."
```

**Memory management — why free `left` before using `result`?**
```c
left = convert(...);          // malloc inside
result = ft_strjoin_sp(left, keyword);  // new malloc
free(left);                   // done with left
```
`result` is a new, independent string. `left` is no longer needed. If we don't free `left` here, it becomes a memory leak.

---

### `convert_small` — Convert numbers < 100

```c
static char *convert_small(char *n, char *key, char *keyword, t_dict *d)
```

Handles tens + ones combinations like `42` with key `40` (forty).

```
n = "42", key = "40", keyword = "forty"
bn_sub_str("42", "40") = "2"
convert("2") = "two"
"forty" + "two" = "forty two"
```

**Why subtraction instead of division?** For numbers below 100, the dictionary uses additive decomposition (forty + two = forty-two), not multiplicative (not "two forties"). Subtraction gives the correct remainder.

---

### `convert` — Main dispatcher

```c
char *convert(char *n, t_dict *d)
```

**Step 1 — Exact match shortcut:**
```c
keyword = dict_value_get(d, n);
if (keyword && bn_cmp(n, "100") < 0)
    return (ft_strdup(keyword));
```
If the number is in the dictionary directly AND is less than 100, return immediately. This handles `0`–`19` and tens like `20`, `30`...`90`.

**Why `< 100` not `<= 100`?** If we shortcut at `100`, we'd return just `"hundred"` without `"one"` in front. By letting `100` fall through, it goes through `convert_large` which produces `convert(1)` + `"hundred"` = `"one hundred"`.

**Step 2 — Find best key:**
```c
key = dict_floor(d, n);
```
Get the largest key that fits inside `n`.

**Step 3 — Dispatch:**
```c
if (bn_cmp(key, "100") >= 0)
    return (convert_large(n, key, keyword, d));
return (convert_small(n, key, keyword, d));
```
Keys ≥ 100 use multiplicative decomposition (large). Keys < 100 use additive decomposition (small).

---

## `main.c` — Entry Point

### `error_exit` — Print error and return 1

```c
int error_exit(char *msg)
```

Prints the message followed by `'\n'` and returns `1` (error exit code). Kept as a function to avoid repeating the write calls everywhere.

---

### `setup_args` — Parse command line arguments

```c
static int setup_args(int argc, char **argv, char **dict_file, char **num_str)
```

Handles two valid forms:
- `./rush-02 42` → dict = default, num = `"42"`
- `./rush-02 numbers.dict 42` → dict = `"numbers.dict"`, num = `"42"`

**Why out-parameters (`char **dict_file`)?** C functions can only return one value. By passing pointers to the variables, we let this function write two values back to `main`.

---

### `print_result` — Apply syntax and print

```c
static int print_result(char *result, t_dict *d)
```

Takes the raw converted string (e.g. `"forty two"`), applies `proper_syntax` to get `"forty-two"`, then prints it.

**Why apply `proper_syntax` here** instead of inside `convert`? Separation of concerns. `convert` only knows about dictionary-based conversion. The proper English syntax (hyphens, commas, "and") is a presentation concern handled at the output layer.

---

### `run` — Load dictionary and convert

```c
static int run(char *dict_file, char *num_str)
```

Initialises an empty dictionary, loads the file into it, then calls `convert` and passes the result to `print_result`.

**Why a separate function from `main`?** Keeps `main` short and readable. `main` handles argument parsing and routing (stdin vs normal mode). `run` handles the actual conversion pipeline.

**Why `dict_free` is called on error but not on success?** `print_result` calls `dict_free` itself before printing. This avoids passing `d` to two places.

---

### `main` — Program entry

```c
int main(int argc, char **argv)
```

Flow:
1. Parse arguments with `setup_args`
2. If number is `"-"` → stdin mode
3. Validate the number string is all digits
4. Load dict and convert

**Why check `"-"` before `ft_str_isnum`?** `"-"` would fail `ft_str_isnum` (dash is not a digit), so the stdin check must come first.

---

## `bonus.c` — Stdin Mode

### `read_line_stdin` — Read one line from stdin

```c
static char *read_line_stdin(void)
```

Reads one character at a time until hitting `'\n'` or end-of-file. Returns the line as a `malloc`'d string, or `NULL` on EOF.

**Why read one character at a time?** We can't use `fgets` (not in allowed functions). Reading one byte at a time with `read(0, &c, 1)` is the simplest alternative that doesn't require buffering.

**Why return `NULL` only when `n <= 0 && i == 0`?** If `read` returns 0 (EOF) but we already have characters in the buffer, we return those characters first. On the next call, `i == 0` and `n <= 0`, so we return `NULL`.

---

### `process_line` — Convert and print one stdin line

```c
static void process_line(char *line, t_dict *d)
```

Validates, converts, applies proper syntax, and prints. Errors print `"Error"` or `"Dict Error"` but don't stop the loop — the program continues reading the next line.

---

### `run_stdin` — Main stdin loop

```c
int run_stdin(char *dict_file)
```

Loads the dictionary once, then loops reading lines from stdin until EOF (`Ctrl+D`). Frees each line after processing.

**Why load the dict only once?** Loading and parsing the file on every line would be hugely wasteful. The dictionary doesn't change between lines.

---

## `bonus_syntax.c` — Word Classifiers

### `word_in_list` — Check if a word is in a list

```c
static int word_in_list(char *w, char **list, int n)
```

Linear search through an array of strings. Used by `is_scale`, `is_tens_word`, `is_ones_word`.

**Why a shared helper?** Avoids writing three identical loops. Each classifier just passes a different list.

---

### `is_scale` — Is this a large unit word?

```c
int is_scale(char *w)
```

Returns `1` for words like `"thousand"`, `"million"`, `"billion"`... through `"undecillion"`.

**Why needed?** After a scale word, the next group gets a comma: `"one million, two hundred"`.

---

### `is_tens_word` and `is_ones_word`

Classify words like `"forty"` (tens) and `"two"` (ones). Used to decide when to insert a hyphen.

**Why hardcoded lists?** These are fixed English words that don't change. A lookup table is simpler and faster than any alternative.

---

### `get_sep` — Choose the right separator

```c
char *get_sep(char *prev, char *curr)
```

Returns one of `", "`, `" and "`, `"-"`, or `" "` based on the relationship between the previous and current word.

| Previous word | Current word | Separator |
|---------------|-------------|-----------|
| `"million"` (scale) | anything | `", "` |
| `"hundred"` | not a scale | `" and "` |
| `"forty"` (tens) | `"two"` (ones) | `"-"` |
| anything else | anything | `" "` |

**Why post-process instead of building proper syntax during conversion?** `convert` works recursively on number strings. It has no concept of "is this the last group?" or "is the previous word a scale?". Post-processing the flat word list is much simpler.

---

## `bonus_build.c` — Proper Syntax Builder

### `count_spaces` — Count words in a string

```c
static int count_spaces(char *s)
```

Counts spaces to determine word count. A string with `n` spaces has `n+1` words.

**Why a separate function?** Extracting the count loop from `split_words` keeps both functions under the 25-line norm limit.

---

### `split_words` — Split string into word array

```c
static char **split_words(char *s, int *count)
```

Replaces spaces with `'\0'` in-place, then builds an array of pointers to each word start.

**Why in-place modification?** Avoids allocating copies of each word. The words array just points into the already-allocated `copy` string. Only one `malloc` for the pointer array.

**Important:** The caller must `free(words[0])` (which is `copy`) to free the underlying string, then `free(words)` for the pointer array.

---

### `append_word` — Join with separator

```c
static char *append_word(char *res, char *sep, char *word)
```

Joins `res + sep + word` into a new string, freeing the old `res` in the process.

**Why consume `res`?** The caller no longer needs the old `res` after appending. Freeing it here prevents leaks and keeps the calling loop clean.

---

### `proper_syntax` — Main entry point for syntax

```c
char *proper_syntax(char *raw)
```

Converts `"forty two"` → `"forty-two"`, `"one hundred forty two"` → `"one hundred and forty-two"`, etc.

Full example:
```
Input:  "one million two hundred thirty four thousand five hundred sixty seven"
Output: "one million, two hundred and thirty-four thousand, five hundred and sixty-seven"
```

**Algorithm:**
1. `ft_strdup(raw)` — make a copy (split_words modifies it in place)
2. `split_words` — build array of word pointers
3. Walk words left to right, calling `get_sep` for each pair
4. `append_word` — glue them together with the right separator

**Why copy before splitting?** `split_words` replaces spaces with `'\0'`, destroying the original string. We work on a copy so `raw` is never modified.

---

## Memory Management Summary

Every function that creates a string via `malloc` is responsible for documenting who owns it:

| Function | Frees input? | Caller must free output? |
|----------|-------------|--------------------------|
| `ft_strdup` | No | Yes |
| `ft_strjoin` | No | Yes |
| `ft_strjoin_sp` | No | Yes |
| `strip_zeros` | **Yes** (takes ownership) | Yes |
| `bn_div` | No | Yes (both quot and *remainder) |
| `convert` | No | Yes |
| `proper_syntax` | No | Yes |

---

## Concept Glossary

| Term | Plain English |
|------|--------------|
| `*` (pointer) | Stores a memory address. Like a "page number" pointing to where data lives. |
| `&` (address-of) | "Give me the address of this variable" |
| `->` | Access a field inside a struct through a pointer |
| `malloc` | Reserve memory. Returns the address of the first byte. |
| `free` | Release memory back. Must be called exactly once per `malloc`. |
| `NULL` | Pointer to address zero — means "nothing" or "failed" |
| `static` | Makes a function private to its own file |
| recursion | A function that calls itself with a simpler input until hitting a base case |
| `'\0'` | The invisible character marking the end of every C string |
| bignum | Storing a very large number as a string instead of an integer |
| `dict_floor` | Largest dictionary key that is ≤ the input number |
| ownership | Which piece of code is responsible for `free`-ing a pointer |

---

---

# 🇹🇭 ภาษาไทย

## วิธีคอมไพล์และรัน

```bash
make fclean && make

./rush-02 numbers.dict 42         # → forty-two
./rush-02 42                      # → forty-two (ใช้ไฟล์ default)
./rush-02 numbers_th.dict 42      # → สี่สิบ สอง
printf "42\n0\n" | ./rush-02 numbers.dict -  # stdin mode
```

---

## ภาพรวมไฟล์

| ไฟล์ | หน้าที่ |
|------|---------|
| `rush02.h` | สารบัญของโปรเจกต์ — type, struct, function declarations |
| `main.c` | จุดเริ่มต้น |
| `utils.c` | ฟังก์ชัน string พื้นฐาน |
| `str_utils.c` | ฟังก์ชัน string เพิ่มเติม |
| `bignum.c` | คณิตศาสตร์สำหรับตัวเลขขนาดใหญ่ |
| `convert.c` | ตรรกะการแปลง (recursive) |
| `dict.c` | เก็บและค้นหา dictionary |
| `dict_parse.c` | อ่านและ parse ไฟล์ |
| `dict_load.c` | Entry point สำหรับโหลด dictionary |
| `bonus.c` | Stdin mode |
| `bonus_syntax.c` | ตรวจสอบประเภทคำ |
| `bonus_build.c` | สร้าง proper English syntax |

---

## `rush02.h` — สารบัญโปรเจกต์

**ทำไมเปลี่ยน key จาก `t_ull` เป็น `char *`?**

`t_ull` เก็บได้สูงสุด ~18 quintillion (10¹⁹) แต่ dictionary มี undecillion (10³⁶) ซึ่งใหญ่กว่ามาก การเก็บ key เป็น string ทำให้รองรับตัวเลขขนาดใดก็ได้

---

## `bignum.c` — คณิตศาสตร์ตัวเลขขนาดใหญ่

### `strip_zeros` — ตัด 0 นำหน้า

รับ ownership ของ `s`, ตัด 0 นำหน้า, คืน string ใหม่, free อันเก่า

**ทำไม free input?** เพื่อป้องกัน memory leak — caller ส่ง string มาให้ ฟังก์ชันนี้รับผิดชอบ free แทน

---

### `bn_cmp` — เปรียบเทียบตัวเลขสอง string

เช็ค length ก่อน เพราะ `"9"` มี length น้อยกว่า `"42"` → 9 น้อยกว่า 42 ถ้าเปรียบตัวอักษรตรงๆ `'9' > '4'` จะผิด

---

### `bn_div` — หารด้วย power of 10

**Trick สำคัญ:** key ในพจนานุกรมทุกตัวเป็น power of 10 (1, 10, 100, 1000...) การหารด้วย power of 10 คือการตัด string ที่ตำแหน่งที่กำหนด ไม่ต้องทำ long division จริงๆ

```
"1234" ÷ "1000" → quotient="1", remainder="234"
ตัดที่ตำแหน่ง split = 4 - (4-1) = 1
```

**ทำไม O(1)?** เพราะแค่ copy substring ไม่ต้องวนลูปคำนวณ

---

## `convert.c` — ตรรกะการแปลง

### `convert` — ฟังก์ชันหลัก (recursive)

```
convert("1042"):
  dict_floor("1042") = "1000" (thousand)
  bn_div("1042","1000") → quot="1", rem="42"
  convert("1") = "one"
  "one" + "thousand" = "one thousand"
  convert("42"):
    dict_floor("42") = "40" (forty)
    bn_sub_str("42","40") = "2"
    convert("2") = "two"
    "forty" + "two" = "forty two"
  "one thousand" + "forty two" = "one thousand forty two"
```

**ทำไม `< 100` ไม่ใช่ `<= 100`?** ถ้า shortcut ที่ 100 จะได้แค่ `"hundred"` ไม่มี `"one"` นำหน้า ต้องให้ 100 ผ่าน `convert_large` ซึ่งจะทำ `convert(1)` + `"hundred"` = `"one hundred"`

---

## `bonus_build.c` — สร้าง Proper Syntax

### `proper_syntax` — แปลง raw string เป็น proper English

```
"forty two"                    → "forty-two"
"one hundred forty two"        → "one hundred and forty-two"
"one thousand one hundred..."  → "one thousand, one hundred and..."
```

**ทำไม post-process แทนที่จะแก้ใน convert?** เพราะ `convert` ทำงานแบบ recursive บน number strings มันไม่รู้ว่า "คำก่อนหน้าคืออะไร" การ post-process flat word list ง่ายกว่ามาก

---

## สรุป Memory Management

| ฟังก์ชัน | free input? | caller ต้อง free output? |
|---------|------------|--------------------------|
| `ft_strdup` | ไม่ | ใช่ |
| `strip_zeros` | **ใช่** (รับ ownership) | ใช่ |
| `bn_div` | ไม่ | ใช่ (ทั้ง quot และ *remainder) |
| `convert` | ไม่ | ใช่ |
| `proper_syntax` | ไม่ | ใช่ |

---

## อภิธานศัพท์

| คำ | ความหมาย |
|----|----------|
| `malloc` | ขอหน่วยความจำจาก OS คืน address ของพื้นที่นั้น |
| `free` | คืนหน่วยความจำ ต้องเรียกหนึ่งครั้งต่อหนึ่ง malloc |
| `static` | private ใช้ได้แค่ใน file นั้น |
| recursion | ฟังก์ชันเรียกตัวเองด้วยปัญหาที่เล็กลง |
| bignum | เก็บตัวเลขขนาดใหญ่เป็น string แทน integer |
| `dict_floor` | key ในพจนานุกรมที่ใหญ่ที่สุดแต่ไม่เกิน n |
| ownership | โค้ดไหนรับผิดชอบ free pointer นั้น |
