@echo off

set DATAVIS_TEMP_DIR=temp_dir
set DATAVIS_TEMP_DIR_FULL=..\%DATAVIS_TEMP_DIR%
set DATAVIS_BUILD_DIR=%DATAVIS_TEMP_DIR_FULL%\tempbuild
set DATAVIS_SOURCE_DIR=%CD%

if "%2"=="" (
    set DATAVIS_BRANCH=master
) else (
    set DATAVIS_BRANCH=%2
)
if "%1"=="" (
    echo Usage: generate_package_win version [branch or SHA]
    echo Branch defaults to master.
    echo Creates the package in parent dir
    echo Docs are generated in a shadow build dir %DATAVIS_BUILD_DIR%
) else (
    set DATAVIS_VERSION=%1
    set DATAVIS_PACKAGE_UNZIP_DIR=%DATAVIS_TEMP_DIR_FULL%\qtdatavisualization_%DATAVIS_VERSION%
    set DATAVIS_ZIP=qtdatavisualization_temp_%DATAVIS_VERSION%.zip
    set DATAVIS_ZIP_FULL=%DATAVIS_TEMP_DIR_FULL%\%DATAVIS_ZIP%
    echo Exporting %DATAVIS_BRANCH% to %DATAVIS_ZIP_FULL%...
    rmdir /q /s %DATAVIS_TEMP_DIR_FULL% 2> NUL
    md %DATAVIS_TEMP_DIR_FULL% 2> NUL
    call git archive --format zip --output ../%DATAVIS_TEMP_DIR%/%DATAVIS_ZIP% %DATAVIS_BRANCH%

    echo Unzipping %DATAVIS_ZIP_FULL% to %DATAVIS_PACKAGE_UNZIP_DIR%...
    md %DATAVIS_PACKAGE_UNZIP_DIR% 2> NUL
    call unzip %DATAVIS_ZIP_FULL% -d %DATAVIS_PACKAGE_UNZIP_DIR% > NUL

    echo Generating includes, mkspecs, and docs in %DATAVIS_BUILD_DIR%...
    md %DATAVIS_BUILD_DIR% 2> NUL
    pushd %DATAVIS_BUILD_DIR%
    qmake %DATAVIS_SOURCE_DIR%\qtdatavisualization.pro > NUL 2> NUL
    nmake docs > NUL 2> NUL
    popd

    echo Copying generated files to %DATAVIS_PACKAGE_UNZIP_DIR%
    set DATAVIS_XCOPY_CMD=xcopy /s /i /q
    %DATAVIS_XCOPY_CMD% %DATAVIS_BUILD_DIR%\doc %DATAVIS_PACKAGE_UNZIP_DIR%\doc
    %DATAVIS_XCOPY_CMD% %DATAVIS_BUILD_DIR%\mkspecs %DATAVIS_PACKAGE_UNZIP_DIR%\mkspecs
    %DATAVIS_XCOPY_CMD% %DATAVIS_BUILD_DIR%\include %DATAVIS_PACKAGE_UNZIP_DIR%\include

    set DATAVIS_FINAL_ZIP=..\qtdatavisualization_%DATAVIS_VERSION%.zip
    echo Repackaging %DATAVIS_PACKAGE_UNZIP_DIR% to %DATAVIS_FINAL_ZIP%
    del /q %DATAVIS_FINAL_ZIP% 2> NUL
    call 7z a -r -y -tzip %DATAVIS_FINAL_ZIP% %DATAVIS_PACKAGE_UNZIP_DIR% >NUL
)
