echo off

rem Make destination folders
rmdir /s /q build\android_archs
mkdir build\android_archs

rem Copy the required files and folder to the android_archs folder
xcopy /e /i /q Chapter2\app\src\main\res                 build\android_archs\app\src\main\res
copy           Chapter2\app\src\main\AndroidManifest.xml build\android_archs\app\src\main\AndroidManifest.xml
copy           Chapter2\app\build.gradle                 build\android_archs\app\build.gradle
copy           Chapter2\app\proguard-rules.pro           build\android_archs\app\proguard-rules.pro
xcopy /e /i /q Chapter2\gradle                           build\android_archs\gradle
copy           Chapter2\build.gradle                     build\android_archs\build.gradle
copy           Chapter2\gradle.properties                build\android_archs\gradle.properties
copy           Chapter2\gradlew                          build\android_archs\gradlew
copy           Chapter2\gradlew.bat                      build\android_archs\gradlew.bat
copy           Chapter2\settings.gradle                  build\android_archs\settings.gradle

rem Zip the folders
pushd build\android_archs
tar -a -cf AndroidBuildFolder.zip ^
    app ^
    gradle ^
    build.gradle ^
    gradle.properties ^
    gradlew ^
    gradlew.bat ^
    settings.gradle
popd

rem Remove destination file and folders
rmdir /s /q build\android_archs\app\
rmdir /s /q build\android_archs\gradle
del build\android_archs\build.gradle
del build\android_archs\gradle.properties
del build\android_archs\gradlew
del build\android_archs\gradlew.bat
del build\android_archs\settings.gradle