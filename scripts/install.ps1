param(
    [string]$SourceDir = (Get-Location).Path,
    [string]$Version = "dev",
    [string]$InstallDir = "$env:USERPROFILE\.local\bin",
    [switch]$System,
    [switch]$Uninstall
)

$ErrorActionPreference = "Stop"
$BinaryName = "any-compiler.exe"

if ($System) {
    $InstallDir = "$env:ProgramFiles\any-compiler\bin"
}

if ($Uninstall) {
    $targetBinary = Join-Path $InstallDir $BinaryName
    if (Test-Path $targetBinary) {
        Remove-Item -Force $targetBinary
        Write-Host "Removed $targetBinary"
    } else {
        Write-Host "$targetBinary is not installed."
    }
    exit 0
}

if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    throw "g++ not found in PATH. Install MinGW-w64 or another g++ toolchain first."
}

$mainFile = Join-Path $SourceDir "src/main.cpp"
if (-not (Test-Path $mainFile)) {
    throw "Could not find source file: $mainFile"
}

$buildDir = Join-Path $env:TEMP "any-compiler-build"
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
$builtBinary = Join-Path $buildDir $BinaryName

$versionFlag = "-DAPP_VERSION=`"$Version`""
Write-Host "Building any-compiler from source..."
& g++ -O2 -std=c++11 -Wall -Wextra $versionFlag $mainFile -o $builtBinary
if ($LASTEXITCODE -ne 0) {
    throw "Build failed."
}

New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null
Copy-Item -Path $builtBinary -Destination (Join-Path $InstallDir $BinaryName) -Force

$userPath = [Environment]::GetEnvironmentVariable("Path", "User")
if (-not $userPath) {
    $userPath = ""
}

$pathParts = $userPath -split ';' | Where-Object { $_ -ne "" }
if ($pathParts -notcontains $InstallDir) {
    $newUserPath = if ([string]::IsNullOrWhiteSpace($userPath)) { $InstallDir } else { "$userPath;$InstallDir" }
    [Environment]::SetEnvironmentVariable("Path", $newUserPath, "User")
    Write-Host "Added $InstallDir to user PATH."
    Write-Host "Open a new terminal to use 'any-compiler' directly."
}

$installedBinary = Join-Path $InstallDir $BinaryName
Write-Host "Installed: $installedBinary"
& $installedBinary --version
