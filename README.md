![Image of Fallout4Test](https://staticdelivery.nexusmods.com/mods/1151/images/headers/51165_1624891066.jpg)

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

In version 1.1, generate .lip files supported. Record file .wav using CK or Audacity.
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
**ONLY SURROGATE**: Support for English and native languages without special characters.
For understanding, the application uses the ancient **ANSI** type string representation.  
The idea is to feed the text already in your native encoding.  
When loading .esp, .esl, .esm files all strings are translated from UTF-8 to WinCP, and when saved back.  
WinCP is the current encoding in your operating system.  
**IMPORTANT**: In **Win10**, in the language settings, there is now an option to work with UTF-8, you need to turn it off otherwise there will be only "?".

# Compilation
At the moment, the project is built on **MS Visual Studio 2019 version 16.0.2**.  
On **MS Visual Studio 2019 version 16.8.4**, the project has errors and even if they fix the behavior the .dll will not be working.
(Optional *CreationKitUnicodePlugin.dll* to compile, use **Lazarus 1.8.2 and higher**)  

# Important 
CK does not work well with ENB.
I wrote f4ck_loader, download from tag 0.9 or compile master yourself. 
The program is console and simple, for 15 seconds removes from the known to me .dll files that harm the work of CK this time is enough to load the CK itself.
All deleted .dll files will be restored after this time.

# License
I deliver the code as-is with no restrictions for the global community and fans **Fallout 4**.

# Future
In the future, I will not be so active in the project, due to the fact that this is enough for me.  
In addition, there are no ideas what you need to add to the project.

# Themes
1. ![Image of Fallout4Test:Theme3](https://staticdelivery.nexusmods.com/mods/1151/images/51165/51165-1620695225-231609444.png)
1. ![Image of Fallout4Test:Theme2](https://staticdelivery.nexusmods.com/mods/1151/images/51165/51165-1620695229-1041447606.png)
1. ![Image of Fallout4Test:Theme1](https://staticdelivery.nexusmods.com/mods/1151/images/51165/51165-1620695240-1122105286.png)
1. ![Image of Fallout4Test:Theme0](https://staticdelivery.nexusmods.com/mods/1151/images/51165/51165-1620695246-844758444.png)