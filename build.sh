cmake --preset="x64-ninja-debug"

cmake --build ./build/x64-ninja-debug/

function yes_or_no {
    while true; do
        read -p "$*  [y/n]: " yn
        case $yn in
            [Yy]*) return 0  ;;  
            [Nn]*) echo "Aborted" ; return  1 ;;
        esac
    done
}

yes_or_no "Run Fortunas Echo?" && ./build/x64-ninja-debug/FortunasEcho.exe
