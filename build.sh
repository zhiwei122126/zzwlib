
# 使用 --version-script 和 -soname= 生成指定 so 名称的，只导出部分符号的库
gcc -fPIC -shared zzwlib/a.c -Wl,-soname=libzzw.so.1 -o zzwlib/libzzw.so.1.1 -Wl,--version-script zzwlib/ver.map

# 使用soname 生成链接
sudo ldconfig /home/zhiwei/work/c/version/zzwlib/

# 使用 soname 链接程序, -rpath 加入运行时库寻找路径
gcc main.c -o main.elf -Wl,-rpath=/home/zhiwei/work/c/version/zzwlib zzwlib/libzzw.so.1
