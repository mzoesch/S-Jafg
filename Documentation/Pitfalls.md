# When using Jetbrains Rider or Clion!!!
Multiline comments and docs should / must never be encountered with `Motor`. These include:
```C++
///     1.
///

/*      2.
 */

/**     3.
 */

...

// Use this instead for multiline comments.
//

//# And that for multiline docs.
//#
```

__Why?__:

Because of [this](https://youtrack.jetbrains.com/issue/RSCPP-34376/Unreal-IDE-and-the-compiler-expand-LINE-macro-differently-after-multi-line-comment-blocks) bug. The Jafg build tool makes heavy use of the `__LINE__` macro in C++ but [R++](https://www.jetbrains.com/resharper-cpp/) expands the `__LINE__` macro wrong (by not counting the multiline comments / docs as separated lines but only as one single line - this is of course completely wrong) which results in complete garbage errors in the intellisense (it will though __compile__ with __no errors__). Visual Studio is not affected by this.

The codebase therefore (because I use mostly CLion and Rider) does not contain any multiline docs and comments where the `Motor` might reflect...
