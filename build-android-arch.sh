#!/bin/bash

#Make destination folders
rm -rf build/android_archs
mkdir -p build/android_archs
mkdir -p build/android_archs/app/src/main/res/drawable
mkdir -p build/android_archs/app/src/main/res/drawable-v24
mkdir -p build/android_archs/app/src/main/res/mipmap-anydpi-v26
mkdir -p build/android_archs/app/src/main/res/mipmap-hdpi
mkdir -p build/android_archs/app/src/main/res/mipmap-mdpi
mkdir -p build/android_archs/app/src/main/res/mipmap-xhdpi
mkdir -p build/android_archs/app/src/main/res/mipmap-xxhdpi
mkdir -p build/android_archs/app/src/main/res/mipmap-xxxhdpi
mkdir -p build/android_archs/app/src/main/res/values

#Copy the required files and folder to the android_archs folder
cp -r Chapter2/app/src/main/res                 build/android_archs/app/src/main
cp    Chapter2/app/src/main/AndroidManifest.xml build/android_archs/app/src/main/AndroidManifest.xml
cp    Chapter2/app/build.gradle                 build/android_archs/app/build.gradle
cp    Chapter2/app/proguard-rules.pro           build/android_archs/app/proguard-rules.pro
cp -r Chapter2/gradle                           build/android_archs/gradle
cp    Chapter2/build.gradle                     build/android_archs/build.gradle
cp    Chapter2/gradle.properties                build/android_archs/gradle.properties
cp    Chapter2/gradlew                          build/android_archs/gradlew
cp    Chapter2/gradlew.bat                      build/android_archs/gradlew.bat
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
    settings.gradle
popd

#Remove destination file and folders
rm -rf build/android_archs/app/
rm -rf build/android_archs/gradle
rm build/android_archs/build.gradle
rm build/android_archs/gradle.properties
rm build/android_archs/gradlew
rm build/android_archs/gradlew.bat
rm build/android_archs/settings.gradle
