# Example of virtual functions

Code sample using virtual functions to implement a DataSource class. The class
reads data from some source (command line, random number generator, ...) and
provides them to `main()`, which computes basic statistics.

## Branches

The `master` branch uses a command line interface. You should call the main
program from the command line like:

```sh
stats --stdin --prompt="Enter a value please"
stats --random-normal --count=1000000 --mean=6.2 --stdev=0.01
```

(By default, the Makefile produces a program called `stats`. Your IDE may ignore
that and produce an executable with a different name.)

The `prompt-args` branch prompts for the command line arguments as the first
line in `main()`, instead of actually reading the command line. This may be
easier to use with Visual Studio or XCode.
