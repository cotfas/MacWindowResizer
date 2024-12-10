# MacWindowResizer
Fast CLI Solution for resizing any window on MAC OS X

Compile with:
    
    gcc -framework Carbon -o resizer resizer.c
    
    
Use it with http://karabiner-elements.pqrs.org

Create hotkey mapping and call the resizer CLI with params:

    {
      "from": {
        "key_code": "1",
        "modifiers": {
          "mandatory": [
            "left_option"
          ]
        }
      },
      "to": [
        {
          "shell_command": "/Users/****/scripts/resizer .95 .95"
        }
      ],
      "type": "basic"
    }
    
Execute with:
    
    resizer .85 .85
    
Param1 - would be horizontal ratio and Param2 - would be vertical ratio

Remember to activate access in Settings->Accessibility for the CLI resizer, Terminal, Karabinier shell


Tested on:

    -Catalina 10.15.7
    -Monterey 12.1

Current issues:
- when connected to multiple monitors, will take the current window but it will be moved to the primary monitor, as a solution it should be taken the current active/focused display on:

      //getting display sizes
      auto mainDisplayId = CGDirectDisplayID();

— This project was created in 2020.
