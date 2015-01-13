# Coding Convention

Parts adapted from [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.html)

### Indentation

- 4 spaces per level
- Spaces only
- Kerninghan & Ritchi with curly on same line

### Whitespace

- No whitespace between identifier and paren
- Whitespace on both sides of identifier and operators

### Comments

- Multiline comments - Single star aligned


    /**
     * ... text ...
     */

- Single line comments - Before the line of code


    // If value is set
    if(val()){


- Tags - One of the following
  - `// FIXME: Problem to be fixed`
  - `// NOTE: Keep this in mind`
  - `// TODO: Finish this code here`

### Header Files

- Named with `.h` extension
- Include a `#define` guard


    #ifndef FILENAME_H
    #define FILENAME_H
    ...
    #endif


- Include order


    // Corresponding file
    #include "foo/server/fooserver.h"

    // C libraries
    #include <sys/types.h>
    #include <unistd.h>
    // C++ Libraries
    #include <hash_map>
    #include <vector>

    // Other headers
    #include "base/basictypes.h"
    #include "base/commandlineflags.h"
    #include "foo/server/bar.h"


### Accessor Functions

- Have getters and setters


    class CClassName {
      protected:
        int DValue ;
      public:
        int Value() const {
          return DValue;
        };
        int Value(int value){
          return DValue = value;
        };
    };

### Constants in comparisons

- Do what is easier to understand
- Take special care with equality

### Blocks

- Bracket single line `if`s
- Switch requires default unless value is an enumeration and case on new indent


    switch(value){
        case 0:
            foo = "bar";
            break;
        case 1:
            foo = "baz";
            break;
        default:
            break;
    }


### Write short functions

- Prefer small and focused functions
- Functions longer than 40 lines that can be broken up should be broken up.

### Naming

- General
  - Should be as descriptive as possible within reason. It's better to be
    longer than to be ambiguous.


    int price_count_reader;    // No abbreviation.
    int num_errors;            // "num" is a widespread convention.
    int num_dns_connections;   // Most people know what "DNS" stands for.


- File names
  - Pascal Case
- Code
  - Case Rules
    - Pascal Case
      - Classes
      - Member Variables
      - Member Functions
      - Enum Types
      - Local Variables
    - Screaming Snake Case
      - Constants
    - Lowercase
      - Parameters
  - Metadata Conventions
    - Classes begin with C
    - Data member variables begin with D
    - Enum types begin with E
    - Enum values begin with first letter abbreviation of type.
      `EConstructionTileType -> cttNone`
    - Type names begin with T
    - Struct names begin with S
    - Globals begin with G


### Example

    TODO: Comment code with conventions

    class CRandomNumberGenerator{
      protected:
      unsigned int DRandomSeedHigh;
      unsigned int DRandomSeedLow;
      public:
      CRandomNumberGenerator(){
        DRandomSeedHigh = 0x01234567;
        DRandomSeedLow = 0x89ABCDEF;
      };
      void Seed(unsigned int high, unsigned int low){
        if((high != low) && low && high){
          DRandomSeedHigh = high;
          DRandomSeedLow = low;
        }
      };
      unsigned int Random(){
        DRandomSeedHigh = 36969 * (DRandomSeedHigh & 65535) + (DRandomSeedHigh >> 16);
        DRandomSeedLow = 18000 * (DRandomSeedLow & 65535) + (DRandomSeedLow >> 16);
        return (DRandomSeedHigh << 16) + DRandomSeedLow;
      };
    };
