# StructuresDeveloperKitExamples
Examples that show how to use Tekla Structures developer kit

# Whats in the examples
We have a project UserApp2019i compiled with DevKit 2019i, illustrating the usage of v140 compiler and Win32. We also have a project UserApp2021 compiled with new DevKit 2021, illustrating the v142 and x64 only libraries

The UserApp2019i version should be working in 2020 and 2021 (in principle). UserApp2021 works only in 2021 and above.

# How to use the app 
This Tekla Structures Developer Kit Examples must be build and installed manually to the Tekla Structures in following way:
Edit file ..\%TSversion%\applications\applications.inp by adding new line with text “user” to the end of file
Create new folder “user” under “..\%TSversion%\applications\”  
Copy following files to the “..\%TSversion%\applications\user” folder:
application.inp
user_macros.inp
UserApp.exe
UserApp.pdb (optional, needed for debugging)
UserApp_resources.dll

