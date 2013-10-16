@echo off

SETLOCAL

set DATAVIS_TEMP_DIR=temp_dir
set DATAVIS_TEMP_DIR_FULL=..\%DATAVIS_TEMP_DIR%

if "%1"=="" (
    echo Usage: generate_package_win version [branch or SHA]
    echo Branch defaults to master.
    echo Creates the package in parent dir.
    echo A temporary dir %DATAVIS_TEMP_DIR_FULL% is utilized for intermediate steps.
    goto :end
)

if "%2"=="" (
    set DATAVIS_BRANCH=master
) else (
    set DATAVIS_BRANCH=%2
)

set DATAVIS_VERSION=%1
set DATAVIS_BUILD_DIR=%DATAVIS_TEMP_DIR_FULL%\tempbuild
set DATAVIS_PACKAGE_UNZIP_DIR=%DATAVIS_TEMP_DIR_FULL%\qtdatavisualization_%DATAVIS_VERSION%
set DATAVIS_TEMP_ZIP=qtdatavisualization_temp_%DATAVIS_VERSION%.zip
set DATAVIS_TEMP_ZIP_FULL=%DATAVIS_TEMP_DIR_FULL%\%DATAVIS_TEMP_ZIP%
set DATAVIS_XCOPY_CMD=xcopy /s /i /q
set DATAVIS_FINAL_ZIP=..\qtdatavisualization_%DATAVIS_VERSION%.zip

echo Exporting %DATAVIS_BRANCH% to %DATAVIS_TEMP_ZIP_FULL%...
rmdir /q /s %DATAVIS_TEMP_DIR_FULL% 2> NUL
md %DATAVIS_TEMP_DIR_FULL% 2> NUL
call git archive --format zip --output ../%DATAVIS_TEMP_DIR%/%DATAVIS_TEMP_ZIP% %DATAVIS_BRANCH%

echo Unzipping %DATAVIS_TEMP_ZIP_FULL% to %DATAVIS_PACKAGE_UNZIP_DIR% and %DATAVIS_BUILD_DIR%...
md %DATAVIS_PACKAGE_UNZIP_DIR% 2> NUL
md %DATAVIS_BUILD_DIR% 2> NUL
call 7z x -y -o%DATAVIS_PACKAGE_UNZIP_DIR% %DATAVIS_TEMP_ZIP_FULL%  > NUL
call 7z x -y -o%DATAVIS_BUILD_DIR% %DATAVIS_TEMP_ZIP_FULL%  > NUL

echo Generating includes, mkspecs, and docs in %DATAVIS_BUILD_DIR%...
pushd %DATAVIS_BUILD_DIR%
::Fake git build to get syncqt to run
md .git 2> NUL
qmake > NUL 2> NUL
nmake docs > NUL 2> NUL
popd

echo Copying generated files to %DATAVIS_PACKAGE_UNZIP_DIR%
%DATAVIS_XCOPY_CMD% %DATAVIS_BUILD_DIR%\doc %DATAVIS_PACKAGE_UNZIP_DIR%\doc
%DATAVIS_XCOPY_CMD% %DATAVIS_BUILD_DIR%\include %DATAVIS_PACKAGE_UNZIP_DIR%\include

echo Repackaging %DATAVIS_PACKAGE_UNZIP_DIR% to %DATAVIS_FINAL_ZIP%
del /q %DATAVIS_FINAL_ZIP% 2> NUL
call 7z a -r -y -tzip %DATAVIS_FINAL_ZIP% %DATAVIS_PACKAGE_UNZIP_DIR% >NUL

:end
