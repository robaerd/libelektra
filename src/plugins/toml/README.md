- infos = Information about the toml plugin is in keys below
- infos/author = Jakob Fischer <jakobfischer93@gmail.com>
- infos/licence = BSD
- infos/provides = storage/toml
- infos/needs = null base64
- infos/recommends = type
- infos/placements = getstorage setstorage
- infos/status = experimental unfinished
- infos/metadata = order comment/# comment/#/start comment/#/space type tomltype origvalue
- infos/description = This storage plugin reads and writes TOML files using Flex and Bison.

# Introduction

This plugin is a storage plugin for reading and writing TOML files. The plugin retains most of the file structure of a read TOML file such as comments, empty lines and TOML tables.
It supports all kinds of TOML specific types and tables, including nested inline tables and multiline strings.

For parsing TOML files, the plugins uses Flex and Bison.

# Requirements

The plugin needs Flex (>=2.6.2) and Bison (>=3).

# NULL/empty keys

The plugin supports null and empty keys with the help of the [null](../null/README.md) plugin.

# Strings

The plugin can read any kind of TOML string: bare, basic, literal, basic multiline and literal multiline.
However, it will write back all non-bare strings as basic strings or it's multiline version.
Therefore, any string set with `kdb set` must be treated as a basic string and possible escape sequences and special meanings of quotation characters must be taken care of.

```
# Mount TOML file
sudo kdb mount test_strings.toml user/tests/storage toml type

# setting a string containing a newline escape sequence
kdb set 'user/tests/storage/string' 'I am a basic string\not a literal one.'

kdb get 'user/tests/storage/string'
# > I am a basic string
# > ot a literal one

# setting the string again, but escape the backslash with another backslash
kdb set 'user/tests/storage/string' 'I am a basic string\\not a literal one.'

kdb get 'user/tests/storage/string'
# > I am a basic string\not a literal one

# Cleanup
kdb rm -r user/tests/storage
sudo kdb umount user/tests/storage
```

The plugin supports all kinds of escape sequences used by TOML in basic and basic multiline strings, like `\n`, `\r`, `\t` and
even `\u`/`\U` for Unicode escape sequences. `\t` is interpreted to be 4 spaces.

# TOML specific structures

TOML specific structures are represented by the metakey `tomltype` on a certain key.
It will be set when the TOML plugin reads a TOML structure from a file. Additionally, this metakey can be set by the user, if they want a certain TOML structure to be written.
No automatic inference of this metakey is done on writing.

## Simple Tables

TOML's simple tables are represented by setting the `tomltype` metakey to `simpletable`.

```
# Mount TOML file
sudo kdb mount test_table.toml user/tests/storage toml type

# Create three keys, which are all a subkey of 'common',
# but we have no 'common' simple table key yet
kdb set 'user/tests/storage/common/a' '0'
kdb set 'user/tests/storage/common/b' '1'
kdb set 'user/tests/storage/common/c' '2'

# Print the content of the resulting TOML file
cat `kdb file user/tests/storage`
#> common.a = 0
#> common.b = 1
#> common.c = 2

# Create a simple table key
kdb meta-set 'user/tests/storage/common' 'tomltype' 'simpletable'

# Print the content of the resulting TOML file
cat `kdb file user/tests/storage`
#> [common]
#> a = 0
#> b = 1
#> c = 2

# Cleanup
kdb rm -r user/tests/storage
sudo kdb umount user/tests/storage
```

## Table Arrays

Table arrays are represented by setting the `tomltype` metakey to `tablearray`. It is not required to also set the array metakey, since the plugin will set the metakey, if it is missing.

```
# Mount TOML file
sudo kdb mount test_table_array.toml user/tests/storage toml type

# Create a table array containing two entries, each with a key 'a' and 'b'
kdb meta-set 'user/tests/storage/tablearray' 'tomltype' 'tablearray'
kdb set 'user/tests/storage/tablearray/#0/a' '1'
kdb set 'user/tests/storage/tablearray/#0/b' '2'

kdb set 'user/tests/storage/tablearray/#1/a' '3'
kdb set 'user/tests/storage/tablearray/#1/b' '4'

# Print the highest index of the table array
kdb meta-get 'user/tests/storage/tablearray' 'array'
#> #1

# Print the content of the resulting TOML file
cat `kdb file user/tests/storage`
#> [[tablearray]]
#> a = 1
#> b = 2
#> [[tablearray]]
#> a = 3
#> b = 4

# Cleanup
kdb rm -r user/tests/storage
sudo kdb umount user/tests/storage
```

## Inline Tables

Inline tables are represented by setting the `tomltype` metakey to `inlinetable`. The plugin also supports reading/writing nested inline tables.

```
# Mount TOML file
sudo kdb mount test_inline_table.toml user/tests/storage toml type

# Create a table array containing two entries, each with a key 'a' and 'b'
kdb meta-set 'user/tests/storage/inlinetable' 'tomltype' 'inlinetable'
kdb set 'user/tests/storage/inlinetable/a' '1'
kdb set 'user/tests/storage/inlinetable/b' '2'
kdb meta-set 'user/tests/storage/inlinetable/nested' 'tomltype' 'inlinetable'
kdb set 'user/tests/storage/inlinetable/nested/x' '3'
kdb set 'user/tests/storage/inlinetable/nested/y' '4'

# Print the content of the resulting TOML file
cat `kdb file user/tests/storage`
#> inlinetable = { a = 1, b = 2, nested = { x = 3, y = 4 } }

# Cleanup
kdb rm -r user/tests/storage
sudo kdb umount user/tests/storage
```

## Arrays

Arrays are recognized by the `array` metakey. On writing, the plugin will detect arrays automatically and set the appropriate metakey if it is missing.

```
# Mount TOML file
sudo kdb mount test_array.toml user/tests/storage toml type

# Create array elements
kdb set 'user/tests/storage/array/#0' '1'
kdb set 'user/tests/storage/array/#1' '2'
kdb set 'user/tests/storage/array/#2' '3'

# Print the highest index of the array
kdb meta-get 'user/tests/storage/array' 'array'
#> #2

# Print the content of the resulting TOML file
cat `kdb file user/tests/storage`
#> array = [1, 2, 3]


# Cleanup
kdb rm -r user/tests/storage
sudo kdb umount user/tests/storage
```

# Comments and Empty Lines

The plugin preserves all comments with only one limitation for arrays. The amount of whitespace in front of a comment is also saved.
For this purpose, each tab will get translated to 4 spaces.

Comments can also be created by assigning meta keys to a key.
The meta keys must be of the form `comment/#n`, where `n` is a positive number, indicating the position of the comment relative to the key.
An index of 0 is always the inline comment of the key.
Indices greater than zero are for comments preceding the given key, where 1 is the top-most comment and the highest index comment is right above the key.

Spaces can be added to a comment by creating a `comment/#n/space` metakey with the amount of spaces to the key.

File ending comments must be assigned to the file root key.

Empty lines in front of a key can be created by adding an empty `comment/#n/start` entry to it. In this case, no `comment/#n` key is needed.

```
# Mount TOML file
sudo kdb mount test_comments.toml user/tests/storage toml type

# create a key-value pair, ready for comment decoration

kdb set 'user/tests/storage/key' '1'

# add an inline comment with 4 leading spaces
kdb meta-set 'user/tests/storage/key' 'comment/#0' ' This value is very interesting'
kdb meta-set 'user/tests/storage/key' 'comment/#0/space' '4'

# add some comments preceding the key
kdb meta-set 'user/tests/storage/key' 'comment/#1' ' I am the top-most comment relative to my key.'
kdb meta-set 'user/tests/storage/key' 'comment/#2' ' I am in the middle. Just boring.'
kdb meta-set 'user/tests/storage/key' 'comment/#3' ' I am in the line right above my key.'

# add file ending comments and empty lines
kdb meta-set 'user/tests/storage' 'comment/#1' ' First file-ending comment'
kdb meta-set 'user/tests/storage' 'comment/#2/start' ''
kdb meta-set 'user/tests/storage' 'comment/#3' ' Second file-ending comment. I am the last line of the file.'

# Print the content of the resulting TOML file
cat `kdb file user/tests/storage`
#> # I am the top-most comment relative to my key.'
#> # I am in the middle. Just boring.'
#> # I am in the line right above my key.'
#> key = 1    # This value is very interesting
#> # First file-ending comment
#>
#> # Second file-ending comment. I am the list line of the file.

# Cleanup
kdb rm -r user/tests/storage
sudo kdb umount user/tests/storage
```

## Comments in Arrays

Any amount of comments can be placed between array elements or between the first element and the opening brackets.

However, only one comment - an inline commment - can be placed after the last element and the closing brackets.
On reading, the plugin discards any non-inline comments between the last element and the closing brackets.

```
# Mount TOML file
sudo kdb mount test_array_comments.toml user/tests/storage toml type

# Create array elements
kdb set 'user/tests/storage/array/#0' '1'
kdb set 'user/tests/storage/array/#1' '2'
kdb set 'user/tests/storage/array/#2' '3'

# Add inline comment after the array
kdb meta-set 'user/tests/storage/array' 'comment/#0' ' Inline comment after the array'
kdb meta-set 'user/tests/storage/array' 'comment/#0/start' '#'
kdb meta-set 'user/tests/storage/array' 'comment/#0/space' '5'

# Add comments for array elements
kdb meta-set 'user/tests/storage/array/#0' 'comment/#0' ' Inline comment of first element'
kdb meta-set 'user/tests/storage/array/#0' 'comment/#0/start' '#'
kdb meta-set 'user/tests/storage/array/#0' 'comment/#0/space' '4'

kdb meta-set 'user/tests/storage/array/#0' 'comment/#1' ' Comment preceding the first element'
kdb meta-set 'user/tests/storage/array/#0' 'comment/#1/start' '#'
kdb meta-set 'user/tests/storage/array/#0' 'comment/#1/space' '4'

kdb meta-set 'user/tests/storage/array/#0' 'comment/#2' ' Another comment preceding the first element'
kdb meta-set 'user/tests/storage/array/#0' 'comment/#2/start' '#'
kdb meta-set 'user/tests/storage/array/#0' 'comment/#2/space' '6'

kdb meta-set 'user/tests/storage/array/#1' 'comment/#0' ' Inline comment of second element'
kdb meta-set 'user/tests/storage/array/#1' 'comment/#0/start' '#'
kdb meta-set 'user/tests/storage/array/#1' 'comment/#0/space' '4'

kdb meta-set 'user/tests/storage/array/#1' 'comment/#1' ' Comment preceding the second element'
kdb meta-set 'user/tests/storage/array/#1' 'comment/#1/start' '#'
kdb meta-set 'user/tests/storage/array/#1' 'comment/#1/space' '6'

kdb meta-set 'user/tests/storage/array/#2' 'comment/#0' ' Inline comment of the last element'
kdb meta-set 'user/tests/storage/array/#2' 'comment/#0/start' '#'
kdb meta-set 'user/tests/storage/array/#2' 'comment/#0/space' '5'

# Print the content of the resulting TOML file
cat `kdb file user/tests/storage`
#> array = [    # Comment preceding the first element
#>       # Another comment preceding the first element
#> 1,    # Inline comment of first element
#>       # Comment preceding the second element
#> 2,    # Inline comment of second element
#> 3     # Inline comment of third element
#> ]     # Inline comment after the array

# Cleanup
kdb rm -r user/tests/storage
sudo kdb umount user/tests/storage
```

# Order

The plugin preserves the file order by the usage of the metakey `order`. When reading a file, the order metakey will be set according to the order as read in the file.
If new keys are added, eg. via `kdb set`, the order of the set key will be set to the next-to-highest order value present in the existing key set.

However, the order is only relevant between elements with the same TOML-parent. For example keys of a simple table are only sorted with respecet to each other, not with any keys outside that table. If that table has it's order changed and moves to another position in the file, so will it's subkeys.

When sorting elements under the same TOML-parent, tables (simple and array) will always be sorted after non-table elements, regardless of their order.
With this limitation, we prevent that a newly set key, that is not part of a certain table array/simple table, would be placed after the table declaration, making it a member of that table on a subsequent read.

```
# Mount TOML file
sudo kdb mount test_order.toml user/tests/storage toml type

# Create three keys in reverse alphabetical order under the subkey common
# Additionally, create one key not in the common subkey space

kdb set 'user/tests/storage/common/c' '0'
kdb set 'user/tests/storage/common/b' '1'
kdb set 'user/tests/storage/common/a' '2'
kdb set 'user/tests/storage/d' '3'

# Print the content of the resulting TOML file
# The keys are ordered as they were set

cat `kdb file user/tests/storage`
#> common.c = 2
#> common.b = 1
#> common.a = 0
#> d = 3

# Create a simple table for the three keys under `common`
kdb meta-set 'user/tests/storage/common' 'tomltype' 'simpletable'

# Print the content of the resulting TOML file
cat `kdb file user/tests/storage`
#> d = 3
#> [common]
#> c = 0
#> b = 1
#> a = 2

# Cleanup
kdb rm -r user/tests/storage
sudo kdb umount user/tests/storage
```

In this example, `d` and `common` have the same parent, the file root. This means, they need to be sorted with each other. `d` would be placed before `common` by it's order, since it was set before, and thus, has lesser order.
However, their order never gets compared, since `common` is a simple table and `d` is not, so `d` will get sorted before the table regardless of order.

# Limitations

While the plugin has good capabilities in handling the TOML file format, it currently lacks some features possible with Elektra:

- Sparse arrays are not preserved on writing, they get a continuous array without index holes.
- Values on non-leaf keys are currently not supported, they get discarded.
- Custom metakeys cannot be written by the plugin, so they get discared.

Additionally, there are some minor limitations related to the TOML file format, mostly related to the preservation of the original file structure:

- On writing, each string is written as a TOML basic string (single or multiline). See [Strings](#Strings)
- Comments and newlines between the last array element and closing brackets are discarded.
- Trailing commas in arrays and inline tables are discarded
- Only spaces in front of comments are preserved.