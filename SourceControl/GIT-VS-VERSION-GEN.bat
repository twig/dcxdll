@ECHO OFF
SETLOCAL

REM ===================
REM Entry Point
REM ===================
:START
ECHO.
SET CACHE_FILE=%~s1GIT_VERSION
SET HEADER_OUT_FILE="%2"

CALL :INIT_VARS
CALL :GET_GIT_DATA
CALL :CHECK_CACHE
IF DEFINED fLEAVE_NOW GOTO END
CALL :WRITE_OUT
GOTO END

REM ====================
REM FUNCTIONS
REM ====================
:: --------------------
:INIT_VARS
:: --------------------
SET strFILE_DATE=
SET strFILE_HASH=
SET strFILE_DESCRIBE=
GOTO :EOF

:: --------------------
:CHECK_CACHE
:: --------------------
IF EXIST %HEADER_OUT_FILE% (
  IF EXIST "%CACHE_FILE%" (
    FOR /F %%A IN (%CACHE_FILE%) DO (
      IF "%%A" == "%strFILE_DESCRIBE%" (
        ECHO Build version is assumed unchanged from: %strFILE_DESCRIBE%.
        SET fLEAVE_NOW=1
      )
    )
  )
)
GOTO :EOF

:: --------------------
:GET_GIT_DATA
:: --------------------
FOR /F "delims=" %%A IN ('"git log -n 1 --format=format:%%ai"') DO SET strFILE_DATE=%%A
FOR /F "delims=" %%A IN ('"git log -n 1 --format=format:%%H"') DO SET strFILE_HASH=%%A
FOR /F "delims=" %%A IN ('git describe') DO SET strFILE_DESCRIBE=%%A
GOTO :EOF

:: --------------------
:WRITE_OUT
:: --------------------
ECHO %strFILE_DESCRIBE% > %CACHE_FILE%

ECHO //Auto Generated resource header, DO NOT EDIT!>"%HEADER_OUT_FILE%"
ECHO #pragma once >> "%HEADER_OUT_FILE%"
ECHO #define GIT_DATE TEXT("%strFILE_DATE%") >> "%HEADER_OUT_FILE%"
ECHO #define GIT_HASH TEXT("%strFILE_HASH%") >> "%HEADER_OUT_FILE%"
ECHO #define GIT_DESCRIBE TEXT("%strFILE_DESCRIBE%") >> "%HEADER_OUT_FILE%"
ECHO #define GIT_DATEA "%strFILE_DATE%" >> "%HEADER_OUT_FILE%"
ECHO #define GIT_HASHA "%strFILE_HASH%" >> "%HEADER_OUT_FILE%"
ECHO #define GIT_DESCRIBEA "%strFILE_DESCRIBE%" >> "%HEADER_OUT_FILE%"
ECHO #define GIT_DATEW L"%strFILE_DATE%" >> "%HEADER_OUT_FILE%"
ECHO #define GIT_HASHW L"%strFILE_HASH%" >> "%HEADER_OUT_FILE%"
ECHO #define GIT_DESCRIBEW L"%strFILE_DESCRIBE%" >> "%HEADER_OUT_FILE%"
GOTO :EOF

:: --------------------
:END
:: --------------------
