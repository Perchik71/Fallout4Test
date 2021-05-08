![Image of Fallout4Test](https://sun9-55.userapi.com/impg/JpgUBM7DP0LaBQAOhiYj6nzwV6MyVu-zFIlRqA/CrvDsSyYnxM.jpg?size=1280x640&quality=96&sign=924d36ca6257d72a5a0fce373f7cfe12&type=album)

# Fallout4Test
A collection of modifications, enhancements, and reverse engineered resources for Fallout 4 and CK Fallout 4

# Fixed or added
* Fast file loading
* Ability to remove hints on the progress bar
* Multiple UI Fix View Menu section and etc
* Create master files and open them as plugins
* Opening plugins as master files if they are in dependencies
* Filtering by active forms
* Filtering mods in the Data dialog
* Many other fixes are also from the author of "@Nukem9"

# Additional
In version 0.7, I redefined the HTML redactor material, because I don't know how to use it, to the redactor material from the author **ousnius**.
Url: https://www.nexusmods.com/fallout4/mods/3635  
Unzip the contents of the archive to the <root game>/Tools/MaterialEditor/ folder.

Generate .lip files supported. Record file .wav using CK or Audacity.
As soon as the wav file will be loaded, you need to select it from the list and click the generate button. 
If successful, the file will be where .wav is, but it will load only after opening the dialog again.
It still uses CK32 and it should be installed.

# Installation
After compiling the code, dynamic libraries are created: *tbb.dll*, *tbbmalloc.dll*, *winhttp.dll*.  
(Optional *CreationKitUnicodePlugin.dll* to compile, use **Lazarus 1.8.2 and higher**)  
All this should be in the root directory of the game.

# Mod Settings
To hide errors, create a file *CreationKitWarnings.txt*.  
**IMPORTANT**: Errors should be copied from the mod's Log window and one error is one line.  
Additionally, you can create a Localize folder and place a file with the same name, but with a different encoding.  
The file must be in **ANSI** encoding.  
*fallout4_test.ini* the main file with the settings and comes with the code, be sure to read.

# Unicode
For understanding, the application uses the ancient **ANSI** type string representation.  
The idea is to feed the text already in your native encoding.  
When loading .esp, .esl, .esm files all strings are translated from UTF-8 to WinCP, and when saved back.  
WinCP is the current encoding in your operating system.  
**IMPORTANT**: In **Win10**, in the language settings, there is now an option to work with UTF-8, you need to turn it off otherwise there will be only "?".

# Compilation
At the moment, the project is built on **MS Visual Studio 2019 version 16.0.2**.  
On **MS Visual Studio 2019 version 16.8.4**, the project has errors and even if they fix the behavior the .dll will not be working.
(Optional *CreationKitUnicodePlugin.dll* to compile, use **Lazarus 1.8.2 and higher**)  

# License
I deliver the code as-is with no restrictions for the global community and fans **Fallout 4**.

# Future
In the future, I will not be so active in the project, due to the fact that this is enough for me.  
In addition, there are no ideas what you need to add to the project.

# Themes
1. ![Image of Fallout4Test:Theme3](https://sun9-19.userapi.com/impg/UnLS-PwmQfTWh_GZOumqfkRXm4iFo8UAgoBdsw/w-e5kGGYvX0.jpg?size=1733x935&quality=96&sign=81f3b1c2914324ad62539c3db2c61b1c&type=album)
1. ![Image of Fallout4Test:Theme2](https://sun9-11.userapi.com/impg/bsb1HZf7RAyq9u4rxCOQEnCn1l6fOo2Q4kI7CA/azLXMrj73kk.jpg?size=1729x941&quality=96&sign=90130684760f8cc1c752b8803fde654c&type=album)
1. ![Image of Fallout4Test:Theme1](https://sun9-59.userapi.com/impg/UksWL5J6PGgvdHKLdJRn2lfrWc3DzYPmvzUD7w/9Z4EQfCPqkc.jpg?size=1731x939&quality=96&sign=cdf86044a58a33dcafeed097218b5ba5&type=album)
1. ![Image of Fallout4Test:Theme0](https://sun9-33.userapi.com/impg/AANr99EErU0DbPoFTN50PvkuoMPusd4NFFGCkQ/5U6H_5Jqlyc.jpg?size=1729x939&quality=96&sign=17d0e4623d85e0aca94b30e0ff8ddf63&type=album)