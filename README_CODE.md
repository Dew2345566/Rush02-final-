# rush-02 — Deep Code Explanation

> อธิบายทุกฟังก์ชันในทุกไฟล์อย่างละเอียด พร้อมเหตุผลว่าทำไมถึงเขียนแบบนี้  
> Every function in every file explained in deep detail with reasoning.

---

# 🇬🇧 English

---

## Project Structure

```
ex00/
├── rush02.h          ← Shared types, structs, all function declarations
├── main.c            ← Entry point: argument parsing, program flow
├── utils.c           ← Core string tools: strlen, strdup, strjoin, putstr
├── str_utils.c       ← More string tools: trim, isdigit, strcmp, strdup_n
├── bignum.c          ← Big number arithmetic: cmp, div, strip zeros
├── convert.c         ← Conversion engine: recursive number-to-word logic
├── convert_help.c    ← Helper for convert: large_remainder
├── dict.c            ← Dictionary CRUD: init, add, get, floor, free
├── dict_parse.c      ← Dictionary I/O: read file, parse lines, store entries
├── dict_load.c       ← Dictionary entry point: ties parse together
├── bonus.c           ← Stdin mode: read numbers line by line
├── bonus_syntax.c    ← Word classifiers: scale, tens, ones, separator
└── bonus_build.c     ← Proper English syntax: hyphen, comma, "and"
```

---

## `rush02.h` — The Shared Header

```c
# define DICT_FILE    "numbers.dict"
# define MAX_ENTRIES  1024
# define BUF_SIZE     65536
# define MAX_DIGITS   64
```

**`DICT_FILE`** — The default filename used when only 1 argument is given.  
**`MAX_ENTRIES`** — Upper limit of dictionary entries. Fixed array means no dynamic resizing needed.  
**`BUF_SIZE`** — 64 KB cap on dictionary file size. Enough for any reasonable word list.  
**`MAX_DIGITS`** — Max digits a number can have (64 > 39, which is the largest needed for undecillion range).

### Why string keys instead of `t_ull`?

```c
typedef struct s_entry
{
    char    *key;    // "1000000000000000000000000000000000000" (undecillion)
    char    *value;  // "undecillion"
} t_entry;
```

`unsigned long long` holds at most ~18 quintillion (10¹⁹). The dictionary has undecillion (10³⁶). Storing keys as strings removes all overflow limitations — a 37-character string holds undecillion perfectly.

---

## `utils.c` — Core String Tools

### `ft_strlen(char *str)` → `int`

```c
int len = 0;
while (str && str[len])
    len++;
return (len);
```

Counts characters until `'\0'`. The `str &&` guard prevents a crash if a `NULL` pointer is passed.

**Why reimplement?** `strlen` is not in the allowed functions list for 42 projects.

---

### `ft_strdup(char *s)` → `char *`

```c
res = malloc(sizeof(char) * (ft_strlen(s) + 1));
// copy every character, add '\0'
```

Creates an independent copy of string `s` in freshly allocated memory.

**Why +1?** The null terminator `'\0'` needs its own byte.

**When to use:** Whenever you need a copy that will outlive the original, or that you will `free` independently.

---

### `ft_strjoin(char *a, char *b)` → `char *`

```c
res = malloc(ft_strlen(a) + ft_strlen(b) + 1);
// copy a, then b, then '\0'
```

Concatenates two strings into a new allocation.

**Why not modify `a`?** The caller still owns `a`. Modifying it would be a side effect. We allocate new memory to keep both originals intact.

---

### `ft_strjoin_sp(char *a, char *b)` → `char *`

Joins two strings with a single space between them.

```
"one" + "thousand" → "one thousand"
```

**Pattern:**
```c
temp = ft_strjoin(a, " ");   // "one "
res  = ft_strjoin(temp, b);  // "one thousand"
free(temp);                  // intermediate string no longer needed
```

**Why free `temp`?** Every `malloc` must have one matching `free`. `temp` is a stepping stone — once `res` is built, `temp` is dead weight.

**NULL guards:** If `a` is NULL, returns a copy of `b`. If `b` is NULL or empty, returns a copy of `a`. Prevents crashes from uninitialized pointers.

---

### `ft_putstr(char *s)` → `void`

```c
if (s)
    write(1, s, ft_strlen(s));
```

Prints a string to stdout using the `write` syscall. `printf` is not in the allowed functions list.

---

## `str_utils.c` — More String Tools

### `ft_trim(char *str)` → `char *`

Removes leading and trailing spaces/tabs. Returns a fresh allocation.

```
"  1000  " → "1000"
"  hello " → "hello"
```

**Algorithm:**
1. `start` pointer walks right past spaces
2. `end` pointer walks left past spaces
3. Copy only what's between them

**Edge case — all spaces:** `end < start` → return `ft_strdup("")` instead of crashing.

**Why not modify in place?** The input string is part of the file buffer that will be freed later. We must not shorten it in place — we need our own allocation.

---

### `ft_isdigit(char c)` → `int`

```c
return (c >= '0' && c <= '9');
```

Checks ASCII range. `'0'` = 48, `'9'` = 57. Any character in this range is a digit.

---

### `ft_str_isnum(char *str)` → `int`

Returns `1` only if every character is a digit AND the string is non-empty.

**Why check empty?** An empty string `""` would pass the `while` loop with zero failures and incorrectly return `1`. The `!str[i]` guard catches this.

---

### `ft_strcmp(char *a, char *b)` → `int`

```c
while (a[i] && b[i] && a[i] == b[i])
    i++;
return ((unsigned char)a[i] - (unsigned char)b[i]);
```

Returns `0` if equal, positive if `a > b`, negative if `a < b`.

**Why `unsigned char` cast?** Prevents sign-extension issues with characters above ASCII 127 (e.g. Thai characters in a custom dictionary).

---

### `ft_strdup_n(char *s, int n)` → `char *`

Copies exactly `n` characters into a new allocation, adds `'\0'`.

**Why needed?** `bn_div` slices a number string at a specific position. The source is in the middle of a longer string and is not null-terminated at the cut point. `ft_strdup_n` lets us copy exactly `n` chars and terminate properly.

---

## `bignum.c` — Big Number Arithmetic

### Why bignum at all?

`unsigned long long` max = 18,446,744,073,709,551,615 (~10¹⁹)  
Undecillion = 1,000,000,000,000,000,000,000,000,000,000,000,000 (10³⁶)

No standard C integer type can hold numbers this large. We store them as digit strings and implement arithmetic directly on the strings.

---

### `strip_zeros(char *s)` → `char *`

Removes leading zeros from a number string. **Takes ownership of `s`** — frees it internally.

```
"0042" → "42"
"0000" → "0"   (always keeps at least one digit)
"100"  → "100" (no leading zeros)
```

**The ownership pattern:** The caller passes a `malloc`'d string and gets back a new `malloc`'d string. The original is freed inside. This lets callers chain operations without manually managing intermediates:

```c
quot = strip_zeros(ft_strdup_n(n, split));
// ft_strdup_n allocates → strip_zeros consumes and replaces → quot is clean
```

**Why `start < len - 1`?** Ensures we never strip the last character. Without this, `"0"` would become `""`, breaking all downstream logic.

---

### `bn_is_zero(char *n)` → `int`

```c
while (n[i] == '0')
    i++;
return (n[i] == '\0');
```

Returns `1` if the number is zero (all characters are `'0'`).

**Why not `ft_strcmp(n, "0")`?** Intermediate results from arithmetic can be `"00"` or `"000"` before `strip_zeros` runs. This function handles those correctly.

---

### `bn_cmp(char *a, char *b)` → `int`

Compares two number strings. Returns positive if `a > b`, negative if `a < b`, zero if equal.

**Step 1 — Length comparison:**
```c
if (la != lb)
    return (la - lb);
```
A longer digit string always represents a larger number. `"100"` (3 digits) > `"99"` (2 digits). Without this, direct character comparison would fail: `'9' > '1'` but 9 < 100.

**Step 2 — Character-by-character:**
```c
while (a[i] && a[i] == b[i])
    i++;
return ((int)(a[i] - b[i]));
```
Only reached when both strings have equal length. The first differing digit decides the result.

---

### `bn_div(char *n, char *divisor, char **remainder)` → `char *`

Divides `n` by `divisor`, returns the quotient and writes the remainder to `*remainder`.

**The key insight:** Every key in `numbers.dict` is a power of 10:

```
1, 10, 100, 1000, 1000000, 1000000000...
```

Dividing by a power of 10 is just slicing the string at the right position:

```
"1234567" ÷ "1000" (4 digits)
split = 7 - (4-1) = 4
quotient  = first 4 chars = "1234"
remainder = last 3 chars  = "567"
```

**Why `ld - 1` not `ld`?**  
`"1000"` has 4 digits but represents 10³. The split position uses `ld - 1` because the number of trailing zeros is `ld - 1`, not `ld`.

**Why this is O(1):**  
Standard long division requires a loop. String slicing is a single `malloc` + `memcpy`. This works exclusively because all our divisors are powers of 10.

**When `split <= 0`:** The dividend is smaller than the divisor. Quotient = `"0"`, remainder = the whole number.

---

## `convert.c` — The Conversion Engine

### Overview

`convert` is recursive. It keeps finding the largest matching dictionary key and recursing on the remainder until the number is fully expressed.

```
convert("1042"):
  dict_floor("1042") = "1000" → "thousand"
  1042 ÷ 1000 → quotient="1", remainder="42"
  convert("1") = "one"
  "one" + "thousand" = "one thousand"
  convert("42"):
    dict_floor("42") = "40" → "forty"
    42 - 40 = 2
    convert("2") = "two"
    "forty" + "two" = "forty two"
  "one thousand" + "forty two" = "one thousand forty two"
```

---

### `bn_sub_loop(char *buf, char *n, char *key)` → `char *`

Implements grade-school subtraction from right to left, handling borrow.

```
  42        n[1]='2', key[1]='0' → diff=2-0=2 → buf[1]='2'
- 40        n[0]='4', key[0]='4' → diff=4-4=0 → buf[0]='0'
----        result: "02"
   2
```

**Why separate from `bn_sub_str`?**  
The loop needs `char buf[]` on the stack (no `malloc`, no `free`). Separating it keeps both functions under the 25-line norminette limit while avoiding heap allocation for the temporary buffer.

---

### `bn_sub_str(char *n, char *key)` → `char *`

```c
char buf[MAX_DIGITS + 1];
buf[ft_strlen(n)] = '\0';
return (strip_zeros(ft_strdup(bn_sub_loop(buf, n, key))));
```

1. Puts the result of subtraction in stack buffer `buf`
2. `ft_strdup` copies it to heap (needed because stack memory disappears when function returns)
3. `strip_zeros` removes leading zeros and returns the clean result

**Why the stack buffer?** Avoids a `malloc`/`free` for a temporary that only lives inside this function.

---

### `convert_large(char *n, char *key, char *keyword, t_dict *d)` → `char *`

Handles numbers where `key >= 100` (thousands, millions, billions...).

```
n="1234567", key="1000000", keyword="million"

quot = "1"     (1234567 ÷ 1000000)
rem  = "234567"

convert("1") = "one"
"one" + "million" = "one million"
→ large_remainder("one million", "234567", d)
  → convert("234567") = "two hundred thirty four thousand five hundred sixty seven"
  → "one million" + "two hundred..." = full result
```

**Why store `quot` in a variable before passing to `convert`?**

The old code was:
```c
left = convert(strip_zeros(bn_div(n, key, &rem)), d);  // LEAK!
```
`strip_zeros(bn_div(...))` creates a string, passes it to `convert`, but there is no variable holding the pointer — so it can never be freed. This was the memory leak found by valgrind.

The fix:
```c
quot = strip_zeros(bn_div(n, key, &rem));  // save pointer
left = convert(quot, d);
free(quot);                                 // now we can free it
```

---

### `large_remainder(char *result, char *rem, t_dict *d)` → `char *`

Handles the remainder part after the main large-unit conversion.

```c
if (bn_is_zero(rem))
    return (free(rem), result);   // no remainder → done
right = convert(rem, d);
free(rem);
left = ft_strjoin_sp(result, right);
free(result);
free(right);
return (left);
```

**Why separated from `convert_large`?** Adding this logic inside `convert_large` would push it over 25 lines (norminette limit). Extracting it also improves readability.

---

### `convert_small(char *n, char *key, char *keyword, t_dict *d)` → `char *`

Handles numbers where `key < 100` (tens + units: 20–99).

```
n="42", key="40", keyword="forty"

result = "forty"
rem = bn_sub_str("42", "40") = "2"
right = convert("2") = "two"
left = "forty" + "two" = "forty two"
```

**Why subtraction instead of division?**  
For small numbers, the decomposition is additive: `forty + two = forty-two`. Division would give `42 ÷ 40 = 1 remainder 2`, but we don't want `"one forty two"` — we want `"forty two"`. Subtraction gives the right remainder directly.

---

### `convert(char *n, t_dict *d)` → `char *`

Main dispatcher.

```c
// Step 1: exact match for n < 100
keyword = dict_value_get(d, n);
if (keyword && bn_cmp(n, "100") < 0)
    return (ft_strdup(keyword));

// Step 2: find largest key <= n
key = dict_floor(d, n);

// Step 3: dispatch based on key size
if (bn_cmp(key, "100") >= 0)
    return (convert_large(...));
return (convert_small(...));
```

**Why `< 100` not `<= 100`?**  
If we shortcut at exactly `100`, we'd return just `"hundred"` without `"one"` in front. By letting `100` fall through to `convert_large`, it produces `convert(1)` + `"hundred"` = `"one hundred"` correctly.

**Why check NULL for `n`?**  
`convert` is called recursively, including with results of `strip_zeros(bn_div(...))`. If any of those return `NULL` due to a failed `malloc`, we must not dereference `NULL`.

---

## `dict.c` — Dictionary Storage

### `dict_init(t_dict *d)` → `void`

```c
d->size = 0;
```

Sets the counter to zero. The `entries` array is part of the struct (stack-allocated) so no `malloc` needed.

---

### `dict_add(t_dict *d, char *key, char *val)` → `int`

1. Scans existing entries for duplicate key → if found, replace value (free old one)
2. If full (`size >= MAX_ENTRIES`) → return `-1`
3. Otherwise append and increment `size`

**Why scan for duplicates?** The subject says values can be modified and entries can be added. A dictionary file might have duplicate keys if a user edits it. We handle this gracefully.

**Why return int?** `-1` signals failure. The caller (`store_entry`) can decide whether to treat it as fatal.

---

### `dict_value_get(t_dict *d, char *key)` → `char *`

Linear scan using `ft_strcmp`. Returns the value string if found, `NULL` otherwise.

**Why linear scan?** With max 1024 entries, a binary search would be premature optimization. O(n) with n≤1024 is negligible.

---

### `dict_floor(t_dict *d, char *n)` → `char *`

Returns a pointer to the largest key string that is `<= n`.

```
n = "1042", keys = ["1", "10", "100", "1000", "1000000"...]
returns "1000"  ← largest key that fits inside 1042
```

**Why scan all entries?** The dictionary is unsorted (subject says entries can be in any order). We must check every entry to find the maximum valid key.

**Why return `char *` (pointer into dict) instead of a copy?**  
The returned pointer is used immediately in `convert` to look up the keyword. It is never stored beyond the function call. Returning a pointer (not a copy) avoids an unnecessary `malloc`/`free`.

---

### `dict_free(t_dict *d)` → `void`

Frees both `key` and `value` for every entry, resets `size` to 0.

**Why free `key`?** Keys are stored as trimmed copies (`ft_trim` allocates new memory). They are not pointing directly into the file buffer — they are independent allocations.

---

## `dict_parse.c` — File Reading and Parsing

### `open_and_alloc(char *filename, int *fd)` → `char *`

Opens the file and allocates the read buffer.

```c
*fd = open(filename, O_RDONLY);
buf = malloc(BUF_SIZE + 1);
if (!buf)
    return (close(*fd), NULL);  // close fd before returning NULL!
```

**Why close before returning NULL?** File descriptors are a limited OS resource. If `malloc` fails and we return without closing, the fd leaks — on a long-running process, this accumulates.

**Why `static`?** This is a private helper only used inside `dict_parse.c`. `static` enforces this at the compiler level and avoids namespace pollution.

---

### `read_file_buf(char *filename)` → `char *`

Reads the entire file into a heap-allocated buffer.

```c
n = read(fd, buf, BUF_SIZE);
while (n > 0)
{
    sum += n;
    if (sum >= BUF_SIZE)
        break;
    n = read(fd, buf + sum, BUF_SIZE - sum);
}
```

**Why loop?** The `read` syscall does not guarantee it returns all bytes in one call. On slow filesystems or pipes, it may return partial data. The loop accumulates until EOF or buffer full.

**Why `buf[sum] = '\0'`?** Makes the buffer a proper C string so all string functions work on it.

---

### `store_entry(char *key_str, char *val_str, t_dict *d)` → `int`

Validates and stores one key-value pair.

```c
trimmed_key = ft_trim(key_str);
if (!trimmed_key || !ft_str_isnum(trimmed_key))
    return (free(trimmed_key), -1);
trimmed_val = ft_trim(val_str);
dict_add(d, trimmed_key, trimmed_val);
```

**Why trim?** The subject format is `[number][spaces]:[spaces][value]`. Spaces around both sides must be removed.

**Why validate key is numeric?** A non-numeric key means the dictionary file is malformed. We return `-1` to signal a parse error.

**Key design decision:** We store the trimmed key string directly — not as a number. This eliminates all overflow concerns for large keys like undecillion.

---

### `parse_line(char *line, t_dict *d)` → `int`

Splits `"1000 : thousand"` into `"1000 "` and `" thousand"`.

```c
while (*colon && *colon != ':')
    colon++;
if (!*colon)
    return (-1);   // no colon found → malformed line
*colon = '\0';     // split in-place
return (store_entry(line, colon + 1, d));
```

**Why in-place split?** The buffer is already allocated. Replacing `:` with `'\0'` effectively gives us two strings pointing into the same buffer — no extra allocation needed.

---

### `process_lines(char *buf, t_dict *d)` → `int`

Walks the file buffer line by line.

```c
while (*line)
{
    // find next newline
    while (*end && *end != '\n')
        end++;
    if (*end == '\n')
    {
        *end = '\0';              // terminate line
        parse_line(line, d);
        line = end + 1;           // advance past newline
    }
    else                          // last line, no trailing newline
    {
        parse_line(line, d);
        break;
    }
}
```

**Why handle last line separately?** If the file doesn't end with `'\n'` (common), the outer loop condition `*line` stays true but no newline is ever found. The `else` branch handles this edge case and breaks out of the loop.

---

## `dict_load.c` — Dictionary Entry Point

### `dict_load(char *filename, t_dict *d)` → `int`

```c
buf = read_file_buf(filename);
if (!buf)
    return (-1);
ret = process_lines(buf, d);
free(buf);
return (ret);
```

Ties everything together: read file → parse lines → free buffer → return result.

**Why free `buf` here?** `buf` is only needed during parsing. Once `process_lines` finishes, all data has been extracted and stored in the dictionary. The buffer is freed immediately to minimise peak memory usage.

---

## `main.c` — Entry Point

### `error_exit(char *msg)` → `int`

```c
write(1, msg, ft_strlen(msg));
write(1, "\n", 1);
return (1);
```

Prints a message followed by newline, returns `1` (error exit code). Used as: `return (error_exit("Error"))`.

**Why return `int` instead of `void`?** Allows `return (error_exit("Error"))` as a one-liner anywhere a function returns `int`. Saves lines and reduces code duplication.

---

### `setup_args(...)` → `int`

Handles two valid argument patterns:

| argc | dict_file | num_str |
|------|-----------|---------|
| 2 | `DICT_FILE` (default) | `argv[1]` |
| 3 | `argv[1]` | `argv[2]` |
| other | — | returns -1 |

**Why out-parameters (`char **dict_file`)?** C functions can only return one value. Passing pointers lets this function write two results back to the caller without a struct.

---

### `print_result(char *result, t_dict *d)` → `int`

```c
if (!result)
{
    dict_free(d);
    return (error_exit("Dict Error"));
}
ft_putstr(result);
write(1, "\n", 1);
free(result);
dict_free(d);
return (0);
```

**Why `dict_free` in both branches?** The dictionary must be freed regardless of success or failure. Putting cleanup in both branches ensures no leak under either outcome.

---

### `run(char *dict_file, char *num_str)` → `int`

```c
dict_init(&d);
dict_load(dict_file, &d);
return (print_result(convert(num_str, &d), &d));
```

Note: `convert(num_str, &d)` is passed directly into `print_result`. `print_result` takes ownership of both the result string and the dictionary — it frees both.

---

### `main(int argc, char **argv)` → `int`

```c
setup_args(...)           // parse arguments
ft_strcmp(num_str, "-")   // check for stdin mode
ft_str_isnum(num_str)     // validate number
run(dict_file, num_str)   // load dict and convert
```

**Why check `"-"` before `ft_str_isnum`?** `"-"` fails `ft_str_isnum` (dash is not a digit), so stdin mode must be detected before validation.

---

## `bonus.c` — Stdin Mode

### `read_line_stdin(void)` → `char *`

Reads one character at a time until `'\n'` or EOF.

```c
n = read(0, &c, 1);
while (n > 0 && c != '\n')
{
    buf[i++] = c;
    n = read(0, &c, 1);
}
if (n <= 0 && i == 0)
    return (NULL);   // true EOF with no data
```

**Why `read` one byte at a time?** `fgets` is not in the allowed functions. Reading single bytes is the simplest buffer-free approach.

**Why return `NULL` only when `i == 0`?** If `read` hits EOF but we already have characters buffered, we return those first. Only on the next call (when `i == 0` and `n <= 0`) do we signal EOF.

---

### `process_line(char *line, t_dict *d)` → `void`

Validates one line from stdin, converts it, and prints the result. Errors (`Error`, `Dict Error`) are printed but do **not** stop the loop — the program continues reading subsequent lines.

---

### `run_stdin(char *dict_file)` → `int`

Loads the dictionary once, then loops reading and processing lines until EOF.

**Why load once?** Loading and re-parsing the file for every input line would be a huge waste. The dictionary is constant throughout the session.

---

## `bonus_syntax.c` — Word Classifiers

### `word_in_list(char *w, char **list, int n)` → `int`

Linear search through a string array. Returns `1` if found.

**Why a shared helper?** `is_scale`, `is_tens_word`, and `is_ones_word` all do the same linear search with different lists. Extracting the loop eliminates three identical implementations.

---

### `is_scale(char *w)` → `int`

Returns `1` for: `thousand`, `million`, `billion`, `trillion`, `quadrillion`, `quintillion`, `sextillion`, `septillion`, `octillion`, `nonillion`, `decillion`, `undecillion`.

**Why needed?** After a scale word, the next group gets a comma: `"one million, two hundred"`.

---

### `is_tens_word(char *w)` → `int`

Returns `1` for: `twenty`, `thirty`, `forty`, `fifty`, `sixty`, `seventy`, `eighty`, `ninety`.

---

### `is_ones_word(char *w)` → `int`

Returns `1` for: `one`, `two`, `three`, `four`, `five`, `six`, `seven`, `eight`, `nine`.

---

### `get_sep(char *prev, char *curr)` → `char *`

Returns the appropriate separator between two consecutive words:

| `prev` | `curr` | separator |
|--------|--------|-----------|
| scale word (million...) | anything | `", "` |
| `"hundred"` | not a scale | `" and "` |
| tens word (forty...) | ones word (two...) | `"-"` |
| anything else | anything | `" "` |

---

## `bonus_build.c` — Proper Syntax Builder

### `count_spaces(char *s)` → `int`

Counts spaces to compute word count. A string with `n` spaces has `n+1` words.

**Why separate from `split_words`?** Extracting the count loop keeps both functions under the 25-line norminette limit.

---

### `split_words(char *s, int *count)` → `char **`

Replaces spaces with `'\0'` in-place, then builds an array of pointers to each word start.

**Memory layout after split:**

```
Before: "forty two hundred\0"
After:  "forty\0two\0hundred\0"
words:   ^       ^    ^
```

**One allocation for the pointer array.** The words themselves point into the already-allocated `copy` string. Only `words[0]` (= `copy`) and `words` itself need to be freed.

---

### `append_word(char *res, char *sep, char *word)` → `char *`

```c
tmp = ft_strjoin(res, sep);
free(res);                    // consume old res
res = ft_strjoin(tmp, word);
free(tmp);                    // consume temp
return (res);
```

Extends `res` by appending `sep + word`. Frees the old `res` and the intermediate `tmp` at each step — the "consume and replace" pattern. The caller gets back a new, longer string and never needs to manage the old one.

---

### `proper_syntax(char *raw)` → `char *`

The main entry point for proper English syntax.

```
"forty two"                    → "forty-two"
"one hundred forty two"        → "one hundred and forty-two"
"one thousand one hundred..."  → "one thousand, one hundred and..."
"one million two hundred..."   → "one million, two hundred and... thousand, ..."
```

**Algorithm:**
1. `ft_strdup(raw)` — copy (split_words modifies in-place, must not destroy original)
2. `split_words` — array of word pointers
3. Walk left to right, call `get_sep(words[i-1], words[i])` for each pair
4. `append_word` — build the result string progressively
5. `free(words)` and `free(copy)` — cleanup

**Why copy before splitting?** `split_words` destroys spaces by replacing them with `'\0'`. Working on a copy preserves `raw` for any caller that might still need it.

---

## Memory Ownership Summary

| Function | Consumes input? | Caller must free output? |
|----------|----------------|--------------------------|
| `ft_strdup` | No | Yes |
| `ft_strjoin` | No | Yes |
| `ft_strjoin_sp` | No | Yes |
| `strip_zeros` | **Yes** — frees input | Yes |
| `bn_div` | No | Yes — both `quot` AND `*remainder` |
| `convert` | No | Yes |
| `proper_syntax` | No | Yes |
| `append_word` | **Yes** — frees `res` | Yes — returns new `res` |

---

---

# 🇹🇭 ภาษาไทย

## โครงสร้างโปรเจกต์

```
ex00/
├── rush02.h          ← นิยามร่วม: type, struct, function declarations
├── main.c            ← จุดเริ่มต้น: รับ argument, ควบคุมโปรแกรม
├── utils.c           ← เครื่องมือ string หลัก
├── str_utils.c       ← เครื่องมือ string เพิ่มเติม
├── bignum.c          ← คณิตศาสตร์ตัวเลขขนาดใหญ่
├── convert.c         ← ตรรกะการแปลง (recursive)
├── convert_help.c    ← ช่วย convert_large
├── dict.c            ← จัดการพจนานุกรม
├── dict_parse.c      ← อ่านและ parse ไฟล์พจนานุกรม
├── dict_load.c       ← Entry point โหลดพจนานุกรม
├── bonus.c           ← Stdin mode
├── bonus_syntax.c    ← ตรวจสอบประเภทคำ
└── bonus_build.c     ← สร้าง proper English syntax
```

---

## `rush02.h` — สารบัญโปรเจกต์

**ทำไม key เก็บเป็น `char *` ไม่ใช่ `t_ull`?**

```
t_ull สูงสุด  = 18,446,744,073,709,551,615  (~10¹⁹)
undecillion   = 1,000,000,000,000,000,000,000,000,000,000,000,000  (10³⁶)
```

ถ้าเก็บเป็น `t_ull` จะ overflow ทันที การเก็บเป็น string ทำให้รองรับตัวเลขขนาดใดก็ได้

---

## `utils.c` — เครื่องมือ String หลัก

### `ft_strlen` — นับตัวอักษร

เดินทีละตัวจนเจอ `'\0'` guard `str &&` ป้องกัน crash ถ้า NULL ถูกส่งมา

### `ft_strdup` — copy string

จอง memory ใหม่ด้วย `malloc`, copy ทุกตัวอักษร, ปิดท้ายด้วย `'\0'`

**ทำไม +1?** ตัวอักษร `'\0'` ต้องมี byte ของตัวเอง

### `ft_strjoin` — ต่อ string สองตัว

จอง memory ขนาด `len(a) + len(b) + 1` แล้ว copy `a` ก่อน ตามด้วย `b`

**ทำไมไม่แก้ `a` โดยตรง?** Caller ยังเป็นเจ้าของ `a` อยู่ การแก้จะมีผลข้างเคียง

### `ft_strjoin_sp` — ต่อ string สองตัวโดยมีช่องว่าง

สร้าง `"one thousand"` จาก `"one"` + `"thousand"`

**ทำไม free `temp`?** ทุก `malloc` ต้องมี `free` ที่ตรงกัน `temp` เป็นแค่ stepping stone

### `ft_putstr` — พิมพ์ string

ใช้ `write` syscall โดยตรง เพราะ `printf` ไม่อยู่ใน allowed functions

---

## `str_utils.c` — เครื่องมือ String เพิ่มเติม

### `ft_trim` — ตัด space/tab หัวท้าย

```
"  1000  " → "1000"
```

เดิน `start` จากซ้ายข้าม space, เดิน `end` จากขวาข้าม space, copy เฉพาะส่วนกลาง

**ทำไมต้อง copy ใหม่?** input string เป็นส่วนหนึ่งของ file buffer ที่จะถูก free ทีหลัง ห้ามแก้ใน place

### `ft_isdigit` — เช็คว่าเป็นตัวเลขไหม

เช็ค ASCII range `'0'` (48) ถึง `'9'` (57)

### `ft_str_isnum` — ทุกตัวเป็นตัวเลขไหม

คืน `1` เฉพาะเมื่อทุกตัวผ่าน `ft_isdigit` และ string ไม่ว่าง  
**ทำไมเช็คว่าง?** string ว่าง `""` จะผ่าน loop โดยไม่มี error ผิดพลาด

### `ft_strcmp` — เปรียบเทียบ string สองตัว

คืน `0` ถ้าเท่ากัน, บวกถ้า `a > b`, ลบถ้า `a < b`

**ทำไม cast เป็น `unsigned char`?** ป้องกัน sign-extension กับตัวอักษรเกิน ASCII 127 (เช่น ภาษาไทย)

### `ft_strdup_n` — copy `n` ตัวอักษร

`bn_div` ตัด number string ที่ตำแหน่งกลาง source ไม่ได้ null-terminated ตรงนั้น ต้องใช้ฟังก์ชันนี้

---

## `bignum.c` — คณิตศาสตร์ตัวเลขขนาดใหญ่

### `strip_zeros` — ตัด 0 นำหน้า

**รับ ownership ของ `s`** — free input, คืน string ใหม่

```
"0042" → "42"   "0000" → "0"   "100" → "100"
```

**ทำไม `start < len - 1`?** ป้องกันตัด 0 ทิ้งหมด `"0"` จะกลายเป็น `""` ซึ่งพังทุกอย่าง

### `bn_is_zero` — เช็คว่าเป็น 0 ไหม

ข้าม `'0'` ทั้งหมด ถ้าหมด string = เป็น 0

**ทำไมไม่ใช้ `ft_strcmp(n, "0")`?** ผลลัพธ์ arithmetic อาจเป็น `"00"` หรือ `"000"` ฟังก์ชันนี้จัดการได้ถูกต้อง

### `bn_cmp` — เปรียบเทียบตัวเลขสอง string

**เช็ค length ก่อน!** เพราะ `"9"` มี length น้อยกว่า `"42"` แต่ถ้าเปรียบตัวอักษรตรงๆ `'9' > '4'` จะผิด

### `bn_div` — หารด้วย power of 10

**Trick สำคัญ:** key ทุกตัวในพจนานุกรมเป็น power of 10 → การหารคือการตัด string

```
"1234567" ÷ "1000" (4 digits)
split = 7 - (4-1) = 4
quotient  = "1234"
remainder = "567"
```

**ทำไม O(1)?** แค่ copy substring ไม่ต้องวนคำนวณ ถ้า divisor ไม่ใช่ power of 10 ต้องทำ long division จริงๆ

---

## `convert.c` — ตรรกะการแปลง

### `bn_sub_loop` — บวกลบตัวเลขใน buffer

Subtraction จากขวาไปซ้าย จัดการ borrow (การยืม)

**ทำไมแยกจาก `bn_sub_str`?** loop ต้องการ `char buf[]` บน stack (ไม่ต้อง malloc) การแยกทำให้ทั้งสองฟังก์ชันอยู่ใต้ 25 บรรทัด (norm limit)

### `bn_sub_str` — ลบและคืน string

```c
buf[ft_strlen(n)] = '\0';
return (strip_zeros(ft_strdup(bn_sub_loop(buf, n, key))));
```

1. บวกลบใน stack buffer
2. `ft_strdup` copy ไปยัง heap (stack memory หายไปเมื่อ function return)
3. `strip_zeros` ตัด leading zeros

### `convert_large` — แปลงตัวเลขขนาดใหญ่ (key ≥ 100)

```
n="1042", key="1000", keyword="thousand"
bn_div("1042","1000") → quot="1", rem="42"
convert("1") = "one"
"one" + "thousand" = "one thousand"
large_remainder("one thousand", "42", d) → "one thousand forty two"
```

**Bug ที่แก้แล้ว:** เดิม `convert(strip_zeros(bn_div(...)))` ไม่มีตัวแปรเก็บ pointer → leak!  
แก้โดยเก็บใน `quot` ก่อน แล้ว `free(quot)` หลัง convert เสร็จ

### `convert_small` — แปลงตัวเลขเล็ก (key < 100)

ใช้ **subtraction** ไม่ใช่ division เพราะ decomposition แบบ additive:  
`42 - 40 = 2` → `"forty" + "two"` = `"forty two"` ✅  
ถ้าใช้ division: `42 ÷ 40 = 1 remainder 2` → `"one forty two"` ❌

### `convert` — main dispatcher

**ทำไม `< 100` ไม่ใช่ `<= 100`?**  
ถ้า shortcut ที่ 100 จะได้แค่ `"hundred"` ต้องให้ 100 ผ่าน `convert_large` เพื่อได้ `"one hundred"`

---

## `dict.c` — จัดการพจนานุกรม

### `dict_add` — เพิ่ม/แก้ไข entry

สแกนหา key ซ้ำก่อน ถ้าเจอ replace value (free อันเก่า) ถ้าไม่เจอ append ใหม่

### `dict_floor` — หา key ใหญ่สุดที่ ≤ n

สแกนทุก entry เพราะ subject บอกว่า entries อาจอยู่ในลำดับใดก็ได้ (unsorted)

**ทำไมคืน pointer ไม่ใช่ copy?** ใช้ทันทีใน `convert` ไม่ต้องเก็บไว้นาน หลีกเลี่ยง malloc/free ที่ไม่จำเป็น

### `dict_free` — คืน memory ทั้งหมด

Free ทั้ง `key` และ `value` เพราะทั้งคู่ถูก malloc ใหม่โดย `ft_trim` ใน `store_entry`

---

## `dict_parse.c` — อ่านและ Parse ไฟล์

### `open_and_alloc` — เปิดไฟล์และ alloc buffer

**ทำไม `close(*fd)` ก่อน return NULL?** File descriptor เป็น OS resource ที่จำกัด ถ้า malloc ล้มเหลวและ return โดยไม่ close จะเกิด fd leak

### `read_file_buf` — อ่านทั้งไฟล์

**ทำไมอ่านเป็น loop?** `read` syscall ไม่รับประกันว่าจะคืน bytes ครบใน call เดียว อาจคืนแค่บางส่วน loop ทำให้อ่านครบแน่นอน

### `store_entry` — validate และเก็บ key-value

Trim ทั้งสองฝั่ง, ตรวจ key เป็นตัวเลข, เก็บเป็น string โดยตรง — ไม่ต้องแปลงเป็น integer เพราะ key อาจใหญ่กว่า `t_ull`

### `parse_line` — แยก `"1000 : thousand"` เป็นสองส่วน

แทน `:` ด้วย `'\0'` ใน place → ได้ string สองตัวโดยไม่ต้อง malloc เพิ่ม

### `process_lines` — เดินทุกบรรทัด

**ทำไม handle บรรทัดสุดท้ายแยก?** ถ้าไฟล์ไม่จบด้วย `'\n'` (เกิดบ่อย) loop ปกติจะไม่ process บรรทัดนั้น `else` branch แก้ edge case นี้

---

## `main.c` — จุดเริ่มต้น

### `error_exit` — พิมพ์ error และ return 1

**ทำไม return `int`?** ให้ใช้เป็น `return (error_exit("Error"))` แบบ one-liner ได้

### `setup_args` — parse arguments

ใช้ out-parameters (`char **dict_file`) เพราะ C return ได้แค่ค่าเดียว pointer ให้เขียนสองค่ากลับไปได้

### `print_result` — พิมพ์ผลลัพธ์

**ทำไม free dict ทั้งสอง branch?** dictionary ต้อง free ไม่ว่า success หรือ fail

### `main` — flow หลัก

**ทำไมเช็ค `"-"` ก่อน `ft_str_isnum`?** `"-"` ไม่ผ่าน `ft_str_isnum` ถ้าเช็คลำดับผิดจะได้ `Error` แทน stdin mode

---

## สรุป Memory Ownership

| ฟังก์ชัน | free input? | caller ต้อง free output? |
|---------|------------|--------------------------|
| `ft_strdup` | ไม่ | ใช่ |
| `ft_strjoin` | ไม่ | ใช่ |
| `strip_zeros` | **ใช่** | ใช่ |
| `bn_div` | ไม่ | ใช่ (ทั้ง quot และ *remainder) |
| `convert` | ไม่ | ใช่ |
| `append_word` | **ใช่** (free res เก่า) | ใช่ |
