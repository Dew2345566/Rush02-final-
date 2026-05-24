# rush-02 — Flowchart & Visual Explanation

> แผนภาพอธิบาย flow การทำงานของทุกส่วนในโปรแกรม  
> Visual flowcharts explaining how every part of the program works.

---

# 🇬🇧 English

---

## 1. Program Entry Flow (`main.c`)

```
User runs: ./rush-02 [dict_file] number
                │
                ▼
        ┌───────────────┐
        │  setup_args() │
        │  parse argc   │
        └───────┬───────┘
                │
        ┌───────┴───────────────────────┐
        │  argc == 2 ?                  │
        │  argc == 3 ?                  │
        │  other?                       │
        └───────┬───────────────────────┘
                │
        ┌───────┴──────────┬────────────────────┐
     argc==2            argc==3              other
        │                  │                    │
  dict = default     dict = argv[1]        print "Error"
  num  = argv[1]     num  = argv[2]        exit(1)
        │                  │
        └──────────┬───────┘
                   │
                   ▼
        ┌──────────────────┐
        │ num_str == "-" ? │
        └────────┬─────────┘
                 │
        ┌────────┴────────┐
       YES               NO
        │                 │
        ▼                 ▼
   run_stdin()    ft_str_isnum(num)?
   (bonus)              │
                ┌────────┴────────┐
               NO               YES
                │                 │
          print "Error"        run()
          exit(1)
```

---

## 2. `run()` — Load Dict and Convert

```
run(dict_file, num_str)
        │
        ▼
┌───────────────────┐
│   dict_init(&d)   │  ← set d.size = 0
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│ dict_load(file,d) │
└────────┬──────────┘
         │
    ┌────┴────┐
   FAIL      OK
    │         │
    ▼         ▼
dict_free  convert(num_str, &d)
"Dict Error"      │
              ┌───┴───┐
             NULL    result
              │         │
              ▼         ▼
          dict_free  ft_putstr(result)
          "Dict Error"  write("\n")
                        free(result)
                        dict_free(&d)
```

---

## 3. Dictionary Loading Flow (`dict_parse.c` + `dict_load.c`)

```
dict_load(filename, d)
        │
        ▼
read_file_buf(filename)
        │
        ├── open_and_alloc()
        │       │
        │   ┌───┴───┐
        │  FAIL    OK
        │   │       │
        │   └─NULL  └── malloc(BUF_SIZE+1)
        │               read() loop
        │               close(fd)
        │               buf[sum] = '\0'
        │               return buf
        │
        ▼
process_lines(buf, d)
        │
        ▼
   ┌────────────────────────────────────┐
   │  LOOP: walk through buf            │
   │                                    │
   │  find '\n' → split line            │
   │      │                             │
   │      ├── empty line? → skip        │
   │      │                             │
   │      └── parse_line(line, d)       │
   │              │                     │
   │          find ':'                  │
   │              │                     │
   │          ┌───┴───┐                 │
   │        NONE    FOUND               │
   │          │       │                 │
   │        -1    split at ':'          │
   │              store_entry()         │
   │                  │                 │
   │              ft_trim(key)          │
   │              ft_str_isnum(key)?    │
   │                  │                 │
   │              ft_trim(val)          │
   │              dict_add(d, key, val) │
   │                                    │
   │  last line (no '\n')? → parse, break│
   └────────────────────────────────────┘
        │
        ▼
   free(buf)
   return 0 or -1
```

---

## 4. Dictionary Structure in Memory

```
t_dict d
┌──────────────────────────────────────────────┐
│  size: 35                                    │
│                                              │
│  entries[0]  key─→"0"        value─→"zero"  │
│  entries[1]  key─→"1"        value─→"one"   │
│  entries[2]  key─→"2"        value─→"two"   │
│  ...                                         │
│  entries[28] key─→"100"      value─→"hundred"│
│  entries[29] key─→"1000"     value─→"thousand"│
│  entries[30] key─→"1000000"  value─→"million"│
│  ...                                         │
│  entries[34] key─→"1000...0" value─→"undecillion"│
└──────────────────────────────────────────────┘
   (keys and values are heap-allocated strings)
```

---

## 5. `convert()` — The Recursive Core

```
convert("1042", d)
        │
        ▼
dict_value_get(d, "1042") → NULL  (not an exact key)
        │
        ▼
dict_floor(d, "1042")
  scan all keys, find largest ≤ "1042"
  → returns "1000"
        │
        ▼
bn_cmp("1000", "100") >= 0 ?  YES
        │
        ▼
convert_large("1042", "1000", "thousand", d)
        │
        ├── bn_div("1042","1000") → quot="1", rem="42"
        │
        ├── convert("1", d)
        │       │
        │       ▼
        │   dict_value_get(d,"1") → "one"
        │   bn_cmp("1","100") < 0  → YES (shortcut)
        │   return ft_strdup("one")
        │
        ├── "one" + "thousand" = "one thousand"
        │
        └── large_remainder("one thousand", "42", d)
                │
                ├── bn_is_zero("42")? NO
                │
                ├── convert("42", d)
                │       │
                │       ▼
                │   dict_value_get(d,"42") → NULL
                │   dict_floor(d,"42") → "40"
                │   bn_cmp("40","100") < 0 → convert_small
                │       │
                │       ├── result = ft_strdup("forty")
                │       ├── bn_sub_str("42","40") = "2"
                │       ├── convert("2") = "two"
                │       └── "forty" + "two" = "forty two"
                │
                └── "one thousand" + "forty two"
                    = "one thousand forty two" ✅
```

---

## 6. Recursion Tree for `convert("1234567")`

```
convert("1234567")
├── dict_floor → "1000000" (million)
├── bn_div("1234567","1000000") → quot="1", rem="234567"
│
├── convert("1") → "one"
├── "one million"
│
└── large_remainder("one million", "234567", d)
    └── convert("234567")
        ├── dict_floor → "100000"? NO → "1000" (thousand)
        │   wait — dict_floor finds "1000" not "100000"
        │   actually: scan all keys ≤ "234567"
        │   largest is "100000"? NO, dict has no "100000"
        │   largest is "1000" ✅
        ├── bn_div("234567","1000") → quot="234", rem="567"
        │
        ├── convert("234")
        │   ├── dict_floor → "100" (hundred)
        │   ├── bn_div("234","100") → quot="2", rem="34"
        │   ├── convert("2") → "two"
        │   ├── "two hundred"
        │   └── convert("34")
        │       ├── dict_floor → "30" (thirty)
        │       ├── bn_sub("34","30") → "4"
        │       ├── "thirty"
        │       └── convert("4") → "four"
        │           = "thirty four"
        │       = "two hundred thirty four"
        │
        ├── "two hundred thirty four" + "thousand"
        │   = "two hundred thirty four thousand"
        │
        └── convert("567")
            ├── dict_floor → "100"
            ├── bn_div("567","100") → quot="5", rem="67"
            ├── convert("5") → "five"
            ├── "five hundred"
            └── convert("67")
                ├── dict_floor → "60"
                ├── bn_sub("67","60") → "7"
                ├── "sixty"
                └── convert("7") → "seven"
                    = "sixty seven"
                = "five hundred sixty seven"

Final: "one million two hundred thirty four thousand five hundred sixty seven" ✅
```

---

## 7. `bn_div()` — String Division (Power of 10 Trick)

```
bn_div("1234567", "1000", &rem)

ln = 7  (length of "1234567")
ld = 4  (length of "1000")
split = 7 - (4-1) = 4

┌─────────────────────────────────┐
│  "1234567"                      │
│   ^^^^│^^^                      │
│   0123│456  ← positions         │
│       │                         │
│  split=4                        │
│       │                         │
│  [0..3] = "1234" → quotient     │
│  [4..6] = "567"  → remainder    │
└─────────────────────────────────┘

Why split = ln - (ld - 1)?
  "1000" has 4 digits, represents 10³
  trailing zeros = ld - 1 = 3
  quotient takes ln - 3 = 4 digits
  remainder takes the last 3 digits
```

**Edge case — number smaller than divisor:**
```
bn_div("42", "1000", &rem)
split = 2 - (4-1) = -1  ← negative!
→ quotient = "0", remainder = "42" (the whole number)
```

---

## 8. `bn_cmp()` — String Number Comparison

```
Compare "100" vs "99":

Step 1: Compare lengths
  len("100") = 3
  len("99")  = 2
  3 ≠ 2 → return 3-2 = +1 (100 > 99) ✅

Why not just compare characters?
  '1' vs '9'
  '1' < '9' → would return -1 (wrong! 100 > 99) ❌

Compare "234" vs "235":

Step 1: lengths equal (both 3)
Step 2: character by character
  '2' == '2' → continue
  '3' == '3' → continue
  '4' vs '5' → return '4'-'5' = -1 (234 < 235) ✅
```

---

## 9. `strip_zeros()` — Clean Leading Zeros

```
Input: "0042" (heap-allocated)

len = 4
start = 0

Loop: start < len-1 (3) AND s[start] == '0'
  s[0]='0' → start=1
  s[1]='0' → start=2
  s[2]='4' → STOP

malloc(4 - 2 + 1) = malloc(3)
Copy s[2..3] = "42"
Add '\0'

free("0042")  ← frees the INPUT
return "42"   ← returns new allocation

Special case: "0000"
  start would reach len-1=3 (stops at last char)
  copies s[3]='0'
  returns "0" (never empty!)
```

---

## 10. `bn_sub_str()` — Number Subtraction

```
bn_sub_str("42", "40")

Stack buffer: buf[MAX_DIGITS+1]
buf[2] = '\0'  (length of "42")

bn_sub_loop(buf, "42", "40"):

  ln = 1 (last index of "42")
  lk = 1 (last index of "40")
  borrow = 0

  i=1: diff = ('2'-'0') - 0 = 2
       key[1]='0' → diff -= 0 → diff=2
       borrow=0, buf[1]='0'+2='2'
       ln=0, lk=0

  i=0: diff = ('4'-'0') - 0 = 4
       key[0]='4' → diff -= 4 → diff=0
       borrow=0, buf[0]='0'+0='0'
       ln=-1, lk=-1

  buf = "02"

ft_strdup("02") → heap copy "02"
strip_zeros("02") → free "02", return "2"

Result: "2" ✅
```

---

## 11. Memory Flow for `convert("1042")`

```
ALLOCATIONS DURING convert("1042"):
─────────────────────────────────────────────────────

[1] bn_div("1042","1000") allocates:
    quot = "1"    ← ft_strdup_n then strip_zeros
    rem  = "42"   ← ft_strdup_n then strip_zeros

[2] convert("1") returns:
    left = "one"  ← ft_strdup("one")

[3] ft_strjoin_sp("one", "thousand"):
    temp = "one " ← ft_strjoin
    result = "one thousand" ← ft_strjoin
    free(temp)     ← freed immediately

[4] free(quot)    ← "1" is freed here
[5] free(left)    ← "one" is freed here

[6] convert("42") returns:
    right = "forty two" ← built inside recursion

[7] ft_strjoin_sp("one thousand", "forty two"):
    temp = "one thousand " ← ft_strjoin
    final = "one thousand forty two" ← ft_strjoin
    free(temp)     ← freed immediately

[8] free(result)  ← "one thousand" freed
[9] free(right)   ← "forty two" freed

RETURNED: "one thousand forty two"
  └─ caller (print_result) frees this after printing

NET LEAKS: 0 ✅
```

---

## 12. Stdin Mode Flow (`bonus.c`)

```
./rush-02 numbers.dict -
        │
        ▼
run_stdin(dict_file)
        │
        ├── dict_load() once
        │
        └── LOOP:
                │
                ▼
          read_line_stdin()
                │
                ▼
          read 1 byte at a time
          until '\n' or EOF
                │
          ┌─────┴─────┐
         NULL       line
          │           │
         break   ft_str_isnum?
                  │
              ┌───┴───┐
             NO      YES
              │       │
         "Error"  convert(line, d)
                      │
                  ft_putstr(result)
                  free(result)
                      │
                  free(line)
                  (loop again)
                │
          dict_free()
```

---

## 13. `proper_syntax()` — Bonus Syntax Flow

```
Input: "one hundred forty two"

split_words() → ["one", "hundred", "forty", "two"]

Build result word by word:

i=0: res = "one"

i=1: prev="one",     curr="hundred"
     get_sep → " " (default)
     res = "one hundred"

i=2: prev="hundred", curr="forty"
     get_sep → " and " (hundred + non-scale)
     res = "one hundred and forty"

i=3: prev="forty",   curr="two"
     get_sep → "-" (tens + ones)
     res = "one hundred and forty-two"

Output: "one hundred and forty-two" ✅
```

**Separator decision tree:**

```
get_sep(prev, curr)
        │
        ├── is_scale(prev)?  YES → return ", "
        │   (million, thousand, billion...)
        │
        ├── prev == "hundred" AND NOT is_scale(curr)?
        │                     YES → return " and "
        │
        ├── is_tens_word(prev) AND is_ones_word(curr)?
        │                      YES → return "-"
        │
        └── default            → return " "
```

---

## 14. Complete Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                        USER INPUT                           │
│              ./rush-02 numbers.dict 1042                    │
└──────────────────────────┬──────────────────────────────────┘
                           │
                    ┌──────▼──────┐
                    │    main()   │
                    └──────┬──────┘
                           │
              ┌────────────▼────────────┐
              │      setup_args()       │
              │  dict="numbers.dict"    │
              │  num="1042"             │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │    ft_str_isnum("1042") │
              │    → valid ✅           │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │         run()           │
              └────────────┬────────────┘
                           │
         ┌─────────────────▼─────────────────┐
         │           dict_load()              │
         │  read_file_buf → process_lines     │
         │  → parse_line → store_entry        │
         │  → dict_add                        │
         │                                    │
         │  t_dict d:                         │
         │  "0"→"zero", "1"→"one",...         │
         │  "1000"→"thousand", etc.           │
         └─────────────────┬─────────────────┘
                           │
         ┌─────────────────▼─────────────────┐
         │          convert("1042", d)        │
         │                                    │
         │  dict_floor("1042") → "1000"       │
         │  convert_large(...)                │
         │    bn_div → quot="1", rem="42"     │
         │    convert("1") → "one"            │
         │    "one thousand"                  │
         │    large_remainder(...)            │
         │      convert("42")                 │
         │        convert_small(...)          │
         │          "forty" + "two"           │
         │          = "forty two"             │
         │    "one thousand forty two"        │
         └─────────────────┬─────────────────┘
                           │
         ┌─────────────────▼─────────────────┐
         │         print_result()             │
         │  ft_putstr("one thousand forty two")│
         │  write("\n")                       │
         │  free(result)                      │
         │  dict_free(d)                      │
         └─────────────────┬─────────────────┘
                           │
                    ┌──────▼──────┐
                    │   OUTPUT    │
                    │one thousand │
                    │  forty two  │
                    └─────────────┘
```

---

## 15. File Dependency Map

```
rush02.h  ←──────────────────────────── included by ALL files
    │
    ├── utils.c          ft_strlen, ft_strdup, ft_strjoin,
    │                    ft_strjoin_sp, ft_putstr
    │
    ├── str_utils.c      ft_trim, ft_isdigit, ft_str_isnum,
    │                    ft_strcmp, ft_strdup_n
    │
    ├── bignum.c         strip_zeros, bn_is_zero,
    │                    bn_cmp, bn_div
    │
    ├── dict.c           dict_init, dict_add, dict_value_get,
    │                    dict_floor, dict_free
    │       ▲
    │       │ uses dict_add, dict_value_get, dict_floor
    │
    ├── dict_parse.c     open_and_alloc, read_file_buf,
    │                    store_entry, parse_line, process_lines
    │       ▲
    │       │ uses read_file_buf, process_lines
    │
    ├── dict_load.c      dict_load
    │       ▲
    │       │ uses dict_load
    │
    ├── convert.c        bn_sub_loop, bn_sub_str,
    │   convert_help.c   convert_large, large_remainder,
    │                    convert_small, convert
    │       ▲
    │       │ uses convert
    │
    ├── bonus.c          read_line_stdin, process_line,
    │                    run_stdin
    │
    ├── bonus_syntax.c   word_in_list, is_scale,
    │                    is_tens_word, is_ones_word, get_sep
    │       ▲
    │       │ uses get_sep
    │
    ├── bonus_build.c    count_spaces, split_words,
    │                    append_word, proper_syntax
    │
    └── main.c           error_exit, setup_args,
                         print_result, run, main
```

---

---

# 🇹🇭 ภาษาไทย

---

## 1. Flow การทำงานของโปรแกรม (main.c)

```
ผู้ใช้รัน: ./rush-02 [dict_file] number
                │
                ▼
        ┌───────────────┐
        │  setup_args() │
        │  เช็ค argc    │
        └───────┬───────┘
                │
        ┌───────┴───────────────────────┐
        │  argc == 2?  (ตัวเลขอย่างเดียว) │
        │  argc == 3?  (dict + ตัวเลข)   │
        │  อื่นๆ?                         │
        └───────┬───────────────────────┘
                │
        ┌───────┴──────────┬────────────────────┐
     argc==2            argc==3              อื่นๆ
        │                  │                    │
  dict = numbers.dict  dict = argv[1]       พิมพ์ "Error"
  num  = argv[1]       num  = argv[2]       exit(1)
        │                  │
        └──────────┬───────┘
                   │
                   ▼
        ┌──────────────────┐
        │ num_str == "-" ? │
        └────────┬─────────┘
                 │
        ┌────────┴────────┐
       ใช่               ไม่ใช่
        │                 │
        ▼                 ▼
   run_stdin()    ft_str_isnum(num)?
   (bonus)              │
                ┌────────┴────────┐
               ไม่ใช่            ใช่
                │                 │
          "Error"             run()
```

---

## 2. Flow การโหลดพจนานุกรม

```
dict_load(filename, d)
        │
        ▼
read_file_buf(filename)
        │
        ├── เปิดไฟล์ด้วย open()
        ├── malloc(65537 bytes)
        ├── read() เป็น loop จนครบ
        └── ปิดไฟล์ คืน buffer
        │
        ▼
process_lines(buf, d)
        │
   ┌────────────────────────────────────┐
   │  LOOP: เดินทีละบรรทัด              │
   │                                    │
   │  หา '\n' → แทนด้วย '\0'           │
   │      │                             │
   │      ├── บรรทัดว่าง? → ข้าม        │
   │      │                             │
   │      └── parse_line()             │
   │              │                     │
   │          หา ':' → แยกเป็นสองส่วน  │
   │              │                     │
   │          store_entry()             │
   │              │                     │
   │          ft_trim(key) + ft_trim(val)│
   │          ตรวจ key เป็นตัวเลข       │
   │          dict_add(d, key, val)     │
   └────────────────────────────────────┘
        │
        ▼
   free(buf)  ← คืน memory ทันทีหลัง parse เสร็จ
```

---

## 3. โครงสร้าง Dictionary ใน Memory

```
t_dict d
┌──────────────────────────────────────────────┐
│  size: 35                                    │
│                                              │
│  entries[0]  key──▶"0"      value──▶"zero"  │
│  entries[1]  key──▶"1"      value──▶"one"   │
│  ...                                         │
│  entries[28] key──▶"100"    value──▶"hundred"│
│  entries[29] key──▶"1000"   value──▶"thousand"│
│  entries[34] key──▶"1000...0" value──▶"undecillion"│
└──────────────────────────────────────────────┘
   (key และ value ทุกตัวเป็น heap string ที่ malloc แยกกัน)
```

---

## 4. Recursion Tree ของ convert("1042")

```
convert("1042")
    │
    ├── dict_floor → "1000" (thousand)
    ├── bn_div("1042","1000") → quot="1", rem="42"
    │
    ├── convert("1")
    │       └── exact match → return "one"
    │
    ├── "one" + "thousand" = "one thousand"
    │
    └── convert("42")
            │
            ├── dict_floor → "40" (forty)
            ├── bn_sub("42","40") = "2"
            │
            ├── result = "forty"
            ├── convert("2")
            │       └── exact match → return "two"
            │
            └── "forty" + "two" = "forty two"

ผลลัพธ์สุดท้าย: "one thousand forty two" ✅
```

---

## 5. bn_div() — การตัด String (Power of 10 Trick)

```
bn_div("1234567", "1000", &rem)

ความยาว n        = 7
ความยาว divisor  = 4
split = 7 - (4-1) = 4

 "1234567"
  ^^^^|^^^
  0123|456   ← ตำแหน่ง
      |
      split=4

ส่วน [0..3] = "1234" → quotient (ผลหาร)
ส่วน [4..6] = "567"  → remainder (เศษ)
```

**ทำไมถึงใช้วิธีนี้ได้?**  
เพราะ key ทุกตัวในพจนานุกรมเป็น power of 10 (`1`, `10`, `100`, `1000`...) การหารด้วย power of 10 คือการตัด string ที่ตำแหน่งที่ถูกต้องเสมอ → ไม่ต้องทำ long division จริงๆ → **O(1)**

---

## 6. Memory Lifecycle ของ convert("1042")

```
MALLOC ที่เกิดขึ้น:
────────────────────────────────────────────

① bn_div("1042","1000"):
   quot = "1"   ← malloc ใหม่
   rem  = "42"  ← malloc ใหม่

② convert("1") คืน:
   left = "one" ← malloc ใหม่ (ft_strdup)

③ ft_strjoin_sp("one","thousand"):
   temp   = "one "          ← malloc → free ทันที
   result = "one thousand"  ← malloc

④ free(quot)  "1" → freed ✅
⑤ free(left)  "one" → freed ✅

⑥ convert("42") คืน:
   right = "forty two" ← malloc ใหม่

⑦ ft_strjoin_sp("one thousand","forty two"):
   temp  = "one thousand "           ← malloc → free ทันที
   final = "one thousand forty two"  ← malloc

⑧ free(result) "one thousand" → freed ✅
⑨ free(right)  "forty two"   → freed ✅

คืน: "one thousand forty two"
  └── print_result() free หลัง print

NET LEAKS: 0 ✅
```

---

## 7. bn_cmp() — เปรียบเทียบตัวเลข

```
เปรียบ "100" กับ "99":

ขั้นที่ 1: เปรียบ length ก่อน
  len("100") = 3
  len("99")  = 2
  3 ≠ 2 → return 3-2 = +1  (100 > 99) ✅

ถ้าเปรียบตัวอักษรตรงๆ:
  '1' กับ '9' → '1' < '9' → return -1  (ผิด! 100 > 99) ❌

เปรียบ "234" กับ "235":
  length เท่ากัน (ทั้งคู่ 3)
  '2' == '2' → ต่อไป
  '3' == '3' → ต่อไป
  '4' กับ '5' → return '4'-'5' = -1  (234 < 235) ✅
```

---

## 8. Flow Stdin Mode (bonus.c)

```
./rush-02 numbers.dict -
        │
        ▼
run_stdin(dict_file)
        │
        ├── โหลด dict ครั้งเดียว
        │
        └── LOOP อ่านทีละบรรทัด:
                │
                ▼
          read_line_stdin()
          อ่านทีละ 1 byte จนเจอ '\n' หรือ EOF
                │
          ┌─────┴─────┐
         NULL       บรรทัด
          │           │
        หยุด    ft_str_isnum?
                  │
              ┌───┴───┐
             ไม่ใช่   ใช่
              │        │
          "Error"  convert(line, d)
                       │
                   ft_putstr
                   free(result)
                   free(line)
                   (วนลูปใหม่)
```

---

## 9. แผนผัง Flow ทั้งโปรแกรม

```
┌─────────────────────────────────────────────────────────────┐
│                      INPUT ของผู้ใช้                        │
│              ./rush-02 numbers.dict 1042                    │
└──────────────────────────┬──────────────────────────────────┘
                           │
                    ┌──────▼──────┐
                    │    main()   │
                    └──────┬──────┘
                           │
              ┌────────────▼────────────┐
              │      setup_args()       │
              │  dict="numbers.dict"    │
              │  num="1042"             │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │    ตรวจ num เป็นตัวเลข  │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │    โหลดพจนานุกรม        │
              │  dict_load()            │
              │  t_dict d (35 entries)  │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │    แปลงตัวเลข           │
              │  convert("1042", d)     │
              │  → "one thousand        │
              │     forty two"          │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │    พิมพ์ผลลัพธ์         │
              │  ft_putstr(result)      │
              │  free ทุก memory        │
              └────────────┬────────────┘
                           │
                    ┌──────▼──────┐
                    │   OUTPUT    │
                    │one thousand │
                    │ forty two   │
                    └─────────────┘
```
