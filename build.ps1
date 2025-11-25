if (Test-Path "$env:ProgramFiles\Git\bin") {
    $env:Path = "$env:ProgramFiles\Git\bin;$env:ProgramFiles\Git\mingw64\bin" + $env:Path
}
elseif (Test-Path "$env:ProgramFiles(x86)\Git\bin") {
    $env:Path = "$env:ProgramFiles(x86)\Git\bin;$env:ProgramFiles(x86)\Git\mingw64\bin;" + $env:Path
}

if (Get-Command "ninja" -ErrorAction SilentlyContinue) {
} else {
	winget install Ninja-build.Ninja
}

try {
    sh.exe build.sh $args
}
catch {
    echo "bash.exe doesn't found"
}
