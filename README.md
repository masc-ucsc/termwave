# termwave
Terminal Digital Electronics Wave viewer library

## Directory
### source
>This contains all the code for termwave.
### test
>Provided are example VCD files that shows how termwave handles different cases.
## Instructions

1. Download this entire project.
2. Navigate to the **termwave/source/** directory on a terminal.
3. Enter **make** which compiles the files to an executable.
4. Type **./main** optionally followed by the VCD file path and enter or just the first command and then enter the file path when asked. Press enter and it will print out the waveform for all signals in the VCD file. Example VCD files are provided in the **test** folder.
5. After you are done, enter **make clean** to remove objects and the executable.