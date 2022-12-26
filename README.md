这个是一个学习inline hook 的demo , 
1. xposed hook 代码修改后后免重启。 
2. 若重启不生效，则需要修改gradle 中app的 versioncode 。 
3. hook 代码代码有详细注释
4. 编译注意事项，
         1. // 默认是thumb模式，现在指定arm 指令集编译    arguments '-DANDROID_TOOLCHAIN=clang', '-DANDROID_ARM_MODE=arm'
         externalNativeBuild {
            cmake {
   // 默认是thumb模式，现在指定arm 指令集编译
         arguments '-DANDROID_TOOLCHAIN=clang', '-DANDROID_ARM_MODE=arm'
                // 指定一些编译选项
//                cppFlags "-std=c++11 -frtti -fexceptions"
//                // 也可以使用下面这种语法向变量传递参数：

//                // arguments "-D变量名=参数".
//                arguments "-DANDROID_ARM_NEON=TRUE",
//                        // 使用下面这种语法向变量传递多个参数（参数之间使用空格隔开）：
//                        // arguments "-D变量名=参数1 参数2"
//                        "-DANDROID_CPP_FEATURES=rtti exceptions"
//
//                // 指定ABI
//                abiFilters "armeabi-v7a" , "arm64-v8a"
            }
            
 5. 学习inline hook 相关博客
 
         2.https://leeon7.github.io/2019/10/27/android-inline-hook/
         3. https://xz.aliyun.com/t/9815 
         4.https://www.sunmoonblog.com/2019/07/15/inline-hook-basic/
         5. http://ele7enxxh.com/Android-Arm-Inline-Hook.html

6. 目前hook的思路：
最简单的实现
最容易想到的一种实现方式，使用跳板0覆盖一个函数的指令，当执行到这个函数的时候其实就是执行跳板0，跳板0在不修改寄存器的情况跳到执行hook函数。如果在hook函数内不需要执行原函数是最简单的，到这hook就是一个完整的hook。
如果需要执行原函数，那么在跳板0覆盖指令之前先备份指令，执行原函数之前把备份的指令再覆盖回去，执行之后再覆盖回跳板0。确实是最简单的方法、也确实可以，
但是也有一个很大的问题，就是多线程的问题，在把备份的指令覆盖回去之后，其他线程执行到这里不就hook不到了，甚至crash。
因为无法加锁(真正有效的锁)，而暂停所有线程的太重了，所以基本上只有自己确定某个函数不存在多线程问题或者无需调用原函数才可用。写个demo，熟悉下hook还行，真的实际使用是不行的。















