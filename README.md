# WinterDevBake

Main purpose of this is for the custom syntax highlighting that I like to do. No web based C++ syntax highlighter can color classes, so this solves that by
allowing custom tags to be used. Simple symbols are handles in a preprocessor.
  
This takes an file containing an HTML file that has 1 custom tag with the signature `<wbake source="<some name>"/>`. A parser prints the HTML file out
until this is encountered, where it will call a function stored in a map with the key <some name>
  
