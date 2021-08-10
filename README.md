# webvtt-cli
This tool is meant to assist Lafayette College's Digital Scholarship Services (DSS) with video captions. It will take captions that have been written in a CSV and write them to a VTT file that can be used with a video.

## Building from source
This tool was built with VS Code. The build task is outlined in [tasks.json](.vscode/tasks.json). To build from source, open the repo's directory and enter "make" in the command line. The executable should be located in the "bin" folder.

## Running the tool
To run this tool, enter the following into the command line:

```
./csv2vtt fileName(s)
```

You can give the tool as many CSVs as you want, they will all be written to separate VTTs. You will know that a VTT has been written to when the message "Done writing VTT" appears. If there are any problems with your CSV, they will be printed out.
