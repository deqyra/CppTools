param (
    $Generator   = 'Ninja',
    $CCompiler   = 'clang-cl',
    $CxxCompiler = 'clang-cl',
    $UseLibCxx   = $true
)

$Flags = ''
$CpCompileCommands = $false

if ($CxxCompiler.Contains('clang') -And $UseLibCxx) {
    $Flags += ' -DCMAKE_CXX_FLAGS="-stdlib=libc++" -DCMAKE_EXE_LINKER_FLAGS="-stdlib=libc++"'
}

Switch ($Generator) {
    'Ninja' {
        $CpCompileCommands = $true
        $flags += ' -DCMAKE_EXPORT_COMPILE_COMMANDS=1'
    }
}

$Flags += " -DCMAKE_C_COMPILER=$CCompiler -DCMAKE_CXX_COMPILER=$CxxCompiler"

cmake -G "$Generator" $Flags ..

if ($CpCompileCommands) {
    Copy-Item compile_commands.json ..
}
