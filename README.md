# TUI Task Manager

A simple terminal-based task manager implemented with ncurses in C

## Overview

This project implements a text-based user interface (TUI) for managing tasks inside a terminal window. It demonstrates ncurses usage for windows, keyboard handling and simple interactive layouts. Tasks support priority, categories, notes, deadlines, simple subtasks, status (done / not done) and persistent storage in JSON format

## Features

- interactive ncurses TUI with separate panes for tasks, details, categories and subtasks
- per-task priority from 1 to 9 displayed before each task
- task status shown with `[ ]` or `[x]`
- add, edit, delete tasks and subtasks
- add a note and deadline to each task (YYYY/MM/DD) with basic validation
- assign multiple categories to a task and browse categories
- sort tasks by priority, nearest deadline or alphabetically
- save and load all tasks to/from a JSON file
- simple CLI mode to list tasks without launching the TUI

## Dependencies

This project depends on the following libraries

- `ncurses` for terminal UI
- `cJSON` (or another JSON C library) for save/load functionality

On Debian/Ubuntu you can install these with

```
sudo apt install libncurses5-dev libncursesw5-dev libcjson-dev
```

## Build

A minimal example to build a single-file prototype

```
gcc main.c -o main -lncurses -lcjson
```

If your project has multiple source files and an include directory use

```
gcc src/*.c -Iinclude -o main -lncurses -lcjson
```

If you use pkg-config for ncurses the following can help

```
gcc main.c -o main $(pkg-config --cflags --libs ncurses) -lcjson
```

## Run

Start the TUI

```
./main
```

List tasks in CLI mode

```
./main list
```

On startup the program attempts to load tasks from `tasks.json` in the current directory

Press `w` to save the current tasks to `tasks.json`
Press `L` (capital L) to manually load tasks from `tasks.json`
Press `q` to quit

## Key bindings (default)

- `j` move down in task list
- `k` move up in task list
- `a` add a new task
- `d` delete selected task
- `e` edit selected task
- `r` edit selected task notes/description
- `n` add or edit deadline for the selected task
- `space` toggle task done / not done
- `c` move focus to categories pane, `d` add category while in categories pane and `k`/`j` to navigate categories
- `l` enter subtasks mode where `j`/`k` navigate subtasks, `a` add subtask, `d` delete subtask, `space` toggle subtask status, `h` return to main task view
- `w` save tasks to `tasks.json`
- `L` load tasks from `tasks.json`
- `q` quit the program

Adjust keys in code if you prefer different navigation

## Data format example (tasks.json)

```json
{
  "tasks": [
    {
      "title": "Buy groceries",
      "priority": 3,
      "done": false,
      "deadline": "2025/10/12",
      "notes": "Milk, eggs, bread",
      "categories": ["home", "shopping"],
      "subtasks": [ {"title": "Check coupons", "done": false} ]
    }
  ]
}
```

## Validation

Deadlines must be in `YYYY/MM/DD` format. The program checks for valid date parts and will report an error if the date is invalid

## Project structure suggestion

```
project-root/
  src/
    main.c
    ui.c
    tasks.c
  include/
    tasks.h
    ui.h
  data/
    tasks.json
  Makefile
  README.md
```

## CLI listing example

The program supports a simple non-interactive listing mode

```
./main list
```

which prints

```
1. [ ] 1st task
2. [x] 2nd task
3. [ ] 3rd task
```




