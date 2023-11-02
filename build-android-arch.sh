#!/bin/bash

#Make destination folders
rm -rf build/android_archs
mkdir -p build/android_archs

#Copy the required files and folder to the android_archs folder
cp -r Chapter2/app/src/main/res                 build/android_archs/app/src/main/res
cp    Chapter2/app/src/main/AndroidManifest.xml build/android_archs/app/src/main/AndroidManifest.xml
cp    Chapter2/app/build.gradle                 build/android_archs/app/build.gradle
cp    Chapter2/app/proguard-rules.pro           build/android_archs/app/proguard-rules.pro
cp -r Chapter2/gradle                           build/android_archs/gradle
cp    Chapter2/build.gradle                     build/android_archs/build.gradle
cp    Chapter2/gradle.properties                build/android_archs/gradle.properties
cp    Chapter2/gradlew                          build/android_archs/gradlew
cp    Chapter2/gradlew.bat                      build/android_archs/gradlew.bat
cp    Chapter2/local.properties                 build/android_archs/local.properties
cp    Chapter2/settings.gradle                  build/android_archs/settings.gradle

#Zip the folders
pushd build/android_archs
zip -r AndroidBuildFolder.zip \
    app \
    gradle \
    build.gradle \
    gradle.properties \
    gradlew \
    gradlew.bat \
    local.properties \
    settings.gradle
popd

#Remove destination file and folders
rm -rf build/android_archs/app/
rm -rf build/android_archs/gradle
rm build/android_archs/build.gradle
rm build/android_archs/gradle.properties
rm build/android_archs/gradlew
rm build/android_archs/gradlew.bat
rm build/android_archs/local.properties
rm build/android_archs/settings.gradle