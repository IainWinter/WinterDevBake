# WinterDevBake

Main purpose of this is for the custom syntax highlighting that I like to do. No web based C++ syntax highlighter can color classes, so this solves that by
allowing custom tags to be used. Simple symbols are handled in a preprocessor.
  

```
usage: -s <source> [-a <article>] [-c <comment_name>] (-d <destination>] | -t)

Options:
        -s -> source file path of page template
        -a -> source file path of article template
        -c -> name to use for comment lookup
        -d -> destination file path
        -t -> only print to console
```
