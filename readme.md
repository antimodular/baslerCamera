
Intro
-----
An example on how to use a [Basler camera](https://www.baslerweb.com/en/products/cameras/area-scan-cameras/ace/aca1920-25uc/) with openframeworks.cc

Install the [pylon macOS SDK](https://www.baslerweb.com/en/products/software/pylon-macos/)

Basler SDK setup
-----
Now follow these steps to connect to pylon SDK to your OF project:
file:///Applications/pylon%20Programmer's%20Guide%20and%20API%20Reference.app/Contents/Resources/Html/pylon_programmingguide.html


Common Settings for Building Applications with pylon

This section shows the most common OS X build settings for building an application using pylon and Apple Xcode. Consult the Advanced Topics section for more information, e.g. when a different configuration is required.

First add the pylon framework to your project, right-click on your project and choose Add files to "<Projectname>".... Browse to the install location of the pylon framework, typically /Library/Frameworks and choose pylon.framework, click Add button.

Add the following user-defined variable for Release and Debug configurations to your build target (Build Settings -> User-Defined -> PYLON_LIBRARY_DIR):

/Library
Add the following Framework Search Paths for Release and Debug configurations to your build target (Build Settings -> Search Paths -> Framework Search Paths):

$(PYLON_LIBRARY_DIR)/Frameworks
Add the following Header Search Paths for Release and Debug configurations to your build target (Build Settings -> Search Paths -> Header Search Paths):

$(PYLON_LIBRARY_DIR)/Frameworks/pylon.framework/Headers/GenICam
Add the following Runpath Search Path for Release and Debug configurations to your build target (Build Settings -> Linking -> Runpath Search Path):

$(PYLON_LIBRARY_DIR)/Frameworks

Install screenshots
-----
