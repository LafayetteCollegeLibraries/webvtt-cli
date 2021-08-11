# webvtt-cli
This tool is meant to assist Lafayette College's Digital Scholarship Services (DSS) with video captions. It will take captions that have been written in a CSV and write them to a VTT file that can be used with a video.

## Building from source
This tool was built in VS Code. To build the source code, you will need to install CMake. Download a binary of the last version from [here](https://cmake.org/download/). Add cmake to your PATH. A tutorial on how to do this for Mac can be found [here](https://code2care.org/pages/permanently-set-path-variable-in-mac-zsh-shell). \
Replace the homebrew path with "/Applications/CMake.app/Contents/bin." \
Open this repo in Visual Studio Code. Install the "CMake" and "CMake Tools" extensions. After doing so, open the command pallete (shift-command-P or "View" &rarr; "Command Palette..."). Type "configure" and select "CMake: Configure." Then select "CMake: Build."

### Installing Google Test

Google Test will be required to build the tool as well as its unit tests. Download the latest release of Google Test from
[its repo](https://github.com/google/googletest). Unzip the file and drag the directory to the root of this repo. Name the directory "gtest."

## Running the tool
To run this tool, enter the following into the command line:

```
./csv2vtt fileName(s)
```

You can give the tool as many CSVs as you want, they will all be written to separate VTTs. You will know that a VTT has been written to when the message "Done writing VTT" appears. If there are any problems with your CSV, they will be printed out.
