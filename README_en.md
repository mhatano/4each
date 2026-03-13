# 4each

This program sequentially executes a specified command on a list of file names.

You specify the command to execute with "-e <Command>". The remaining arguments (except for options) are passed as arguments to that command.

In other words,

```bash
% 4each -e 'command' filename
```

When you run the above command, it internally executes:

```bash
% command filename
```

This program was primarily designed to be used in combination with find. For example:

```bash
% find <somewhere> -name '*.txt' | xargs 4each -e 'nkf --guess'
```

This will sequentially execute `nkf --guess <filename>` for all `*.txt` files.

Of course, you can probably achieve the same thing by properly using the options for `find` and `xargs`, but escaping things correctly can be a hassle.

Since `foreach` may conflict with reserved words, the executable is named `4each` in the Makefile. If you have an executable named `4each`, you can run it as `./4each`.

Honestly, I was able to implement the basic features in about 30 minutes, so it didn’t feel necessary to use generative AI. Still, I think I created an interesting program.

I hope this will be helpful for someone’s daily work. I think I'll use it quite a lot myself.

If you have any suggestions or code ideas for improvements, please send them. Thank you.