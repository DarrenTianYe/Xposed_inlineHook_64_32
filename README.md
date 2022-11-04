# Xposed_inlineHook32


注意事项，
1.                 // 默认是thumb模式，现在指定arm 指令集编译    arguments '-DANDROID_TOOLCHAIN=clang', '-DANDROID_ARM_MODE=arm'
         externalNativeBuild {
            cmake {
                // 默认是thumb模式，现在指定arm 指令集编译
                arguments '-DANDROID_TOOLCHAIN=clang', '-DANDROID_ARM_MODE=arm'
                // 指定一些编译选项
//                cppFlags "-std=c++11 -frtti -fexceptions"
//                // 也可以使用下面这种语法向变量传递参数：
//
//
//
//                // arguments "-D变量名=参数".
//                arguments "-DANDROID_ARM_NEON=TRUE",
//                        // 使用下面这种语法向变量传递多个参数（参数之间使用空格隔开）：
//                        // arguments "-D变量名=参数1 参数2"
//                        "-DANDROID_CPP_FEATURES=rtti exceptions"
//
//                // 指定ABI
//                abiFilters "armeabi-v7a" , "arm64-v8a"
            }
2.https://leeon7.github.io/2019/10/27/android-inline-hook/



