Rune Engine coding style
========================

This is a short document describing the preferred coding style for the Rune
Engine. This coding style is largely inspired by the Linux kernel coding style,
with some tweaks to make it friendly to modern editors.

Indentation
-----------

Tabs are 8 spaces, tab characters should not be used. This makes the code look
the same across editors and systems, and forces developers to simplify their
code wherever possible. Large indentations make each code block easier to
identify at a glance.

.. code-block:: c

   int main(int argc, char* argv[]) {
           if (rune_init(argc, argv) != 0) {
                   log_output(LOG_FATAL, "Error!");
                   return -1;
           }

           int running = 1;
           while (running) {
                   // main loop
           }

           rune_exit();
   }

Branch statements should not be placed on the same line; it makes it look like
you have something to hide:

.. code-block:: c

   if (condition) do_this;
           do_something_always;

Braces should be used on multiple statements:

.. code-block:: c

   if (condition) {
           do_this();
           do_that();
   }

Assignments should be placed on their own lines. The coding style is very simple
and fancy expressions are frowned upon.

Long lines
----------

The preferred limit on the length of a single line is 80 columns, however, some
exceptions are made based on readability. Otherwise, lines longer than 80
columns should ideally be broken down into sensible chunks. A commonly used
technique is to align descendants to a function open parenthesis. However,
user-visible strings should *never* be broken as that breaks the ability to grep
for them.

Braces and spaces
-----------------

The opening brace should *always* be placed last on the same line and the
closing brace first on the following line. This applies to all functions and
statement blocks. If a statement is followed by a continuation, it is placed
right after the brace. This allows easy insertion and deletion of interposing
statements:

.. code-block:: c

   int func(void) {
   }

   switch (value) {
           case 0:
                   break;
           default:
                   return;
   }

   if (x == 1) {
           here();
           then_here();
           over_there_as_well();
   } else {
           do_this();
   }

An exception is when a single statement under a branch: no braces are required
unless one of the branches has multiple lines. In addition, braces should be
used on any loop that has more than a single statement.

Spaces should be placed on each side of most binary and ternary operators, such
as the following:

.. code-block:: c

   = + - < > * / % | & ^ <= >= == != ? :

However, spaces should *not* be placed after unary operators, increment and
decrement operators and structure member access operators.

Unnecessary whitespace at the end of lines or files is frowned upon. Some
editors with smart indentation will leave whitespace at the beginning of new
lines so you can start typing the next line right away. However, many editors
do not erase the whitespace should you choose not to continue typing.

Git will warn about trailing whitespace, and can optionally strip it out for
you; however, if applying a series of patches, this may make later patches in
the series fail by changing context lines.

Naming things
-------------

Encoding the type into the function or variable name can become misleading at
best, or dead wrong at worst. The compiler already knows what type the object
in question is, and relaying this information to the programmer is unnecessary.
Furthermore, overly descriptive names for variables are frowned upon, unless the
description boosts readability. 

Local variables should be short and to the point. Loop counter should just be
called ``i``. Giving temporary and single-use variables long, descriptive names
is counterproductive and makes the code that much harder to read.

Global variables should be avoided, but in cases where they *must* be used, they
should have a name that reflects their purpose and scope. For instance:

.. code-block:: c

   int index_counter = 0;
   vec3 clear_color = {0.0f, 0.0f, 0.0f, 1.0f};

Typedefs
--------

Typedefs should generally be avoided at the local level, however, at the global
or API level, a bare structure could become a bit too wordy for general use.
Instead, use a typedef to avoid hitting the 80-column limit unless  it is
readable as it is.

The engine has already defined the standard C23 integer types for use in
engine code, so there is no need to do any kind of typedef for standard types.
Even if the backing type information is platform-dependant, a simple ``void*``
is almost always preferable to creating a new type wholesale.

Functions
---------

Functions should be short and (ideally) do one operation. They should fit on
no more than one or two 1080p screens, and any more than that is a sign the
function is too complex. Never be afraid to define a helper function local to
the function's translation unit. If a function is no more than five lines long,
it can be inlined for performance. Helper functions should be prefixed with an
underscore and marked ``static``, so that they aren't visible by the API.

The number of local variables should not exceed five or six, and any more than
that is a good sign of an overly complex function. If you need more than that,
the function's design should be re-thought. Psychology says that the average
human brain can only hold between five and seven things in working memory, so
functions should be designed accordingly. In source files, functions should be
separated with a single blank line. In header files, any function made part of
the API should be marked with ``RAPI`` before the function signature.

In function prototypes, the function name should be included with the type
information. This adds valuble information for the reader and makes function
arguments easier to track in debuggers.

Commenting
----------

The engine uses doxygen-style comments for generating documentation. Outside of
this, comments should be used to explain the what and why of a piece of code.
Avoid putting comments inside a function body unless its a TODO or FIXME tag.
Generally, comments should be placed at the head of a function for documentation
purposes, especially engine API functions.

.. code-block:: c

   /**
    * \brief Example function
    * \param[in] ex1 Example input
    * \param[out] ex2 Example output
    * \return Example return
    */
    int example(int ex1, int *ex2);

Structs should also be commented in a similar style:

.. code-block:: c

   /**
    * Example struct
    */
    struct example {
            int ID;             ///< Identifier
            struct member *m;   ///< Example member
    }

Member doc comments should be tabbed over so that they are in the same column.

Data structures
----------------

Data structures that have visibility outside the local thread should have a
reference count. The engine provides a reference accounting structure that is
thread-safe and works atomically. If you think your code will be visible outside
the current thread, don't hesitate to use it. It is better to be overly cautious
and use more memory than to take unnecessary risks with thread-global data.

In addition, reference counting means the engine can avoid locking, and allows
multiple threads to access the data structure in parallel, or merely ensuring
the underlying data doesn't get taken away during a lock or other operation.

A good rule of thumb is that if another thread can find your structure, and it
doesn't contain a reference count, it is almost always a bug.

Macros and enums
----------------

All macros, enum values and constants should be capitalized.

.. code-block:: c

   #define PI 3.1415926535
   
   enum state {
           STATE_READY,
           STATE_UNREADY,
           STATE_LOCKED
           ...
   }

   int GRAVITY = 9.8;

Generally, inline functions are preferred to macros resembling functions, as
bugs in macros are much harder to track down and don't appear in a debugger.
Macros with multiple statements should be enclosed in a do-while block:

.. code-block:: c

   #define MACRO(a, b, c)                       \
           do {                                 \
                   if (a == 5)                  \
                           do_this(b, c);       \
           } while(0)

You should avoid macros that affect control flow, depend on having a local
variable with a magic name and macros that are used as l-values. Furthermore,
macros that have a more complex expression should be enclosed in parentheses.

.. code-block:: c

   #define PI           3.14159265
   #define AREA(x)      (PI * x * x)

Macros should take into account common variable names, especially those
resembling functions. For instance:

.. code-block:: c

   #define FOO(x)               \
   ({                           \
           typeof(x) ret;       \
   })

Conditional compilation
-----------------------

Wherever possible, the use of preprocessor conditionals should be relegated to
header files. If you need to define a different function (say, for a different
platform) it is best to define functions for each case and a stub for the final
else.

References
----------

* `Linux kernel coding style <https://www.kernel.org/doc/html/v4.10/process/coding-style.html>`_
