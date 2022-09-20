# Binary Analysis

[はじめて学ぶバイナリ解析](https://www.amazon.co.jp/%E3%81%AF%E3%81%98%E3%82%81%E3%81%A6%E5%AD%A6%E3%81%B6%E3%83%90%E3%82%A4%E3%83%8A%E3%83%AA%E8%A7%A3%E6%9E%90-%E4%B8%8D%E6%AD%A3%E3%81%AA%E3%82%B3%E3%83%BC%E3%83%89%E3%81%8B%E3%82%89%E3%82%B3%E3%83%B3%E3%83%94%E3%83%A5%E3%83%BC%E3%82%BF%E3%82%92%E5%AE%88%E3%82%8B%E3%82%B5%E3%82%A4%E3%83%90%E3%83%BC%E3%82%BB%E3%82%AD%E3%83%A5%E3%83%AA%E3%83%86%E3%82%A3%E6%8A%80%E8%A1%93-OnDeck-Books%EF%BC%88NextPublishing%EF%BC%89-%E5%B0%8F%E6%9E%97-%E4%BD%90%E4%BF%9D-ebook/dp/B084R85269)を通して学習した内容をこのレポジトリにまとめます。個人的に C 言語をひととおり学習した後だとイメージがつきやすかったです。

## リトルエンディアン方式

## レジスタとスタック

## アセンブラ

### 条件分岐

アセンブラで条件分岐の処理を記述すると以下のようになる。

```s
global main

main:
    mov eax, 0x5
    cmp eax, 0x3
    jz equal
    jmp neq

equal:
    mov eax, 0x1
    jmp exit

neq:
    mov eax, 0x0

exit:
```

- `main, equal, neq, exit`のことをラベルと呼ぶ。高級言語で言うところの関数に近い使い方をする。
- また、**ラベルは上から順番に実行されていくため、分岐処理で実行したくないラベルについては jmp 命令でラベルを飛ばす必要がある**
  - 高級言語に慣れているとイメージできないが、`equal`ラベルの処理が終了した後`neq`の処理は飛ばしたいので jmp 命令で終了用のラベルに飛ばすようにしている

## バッファオーバーフローの実践

バッファオーバーフローを利用して、コード中に埋め込まれている変数を書き換える演習を行う。なお、バッファオーバーフローは scanf 関数の脆弱性を利用して行う。

### 準備

- サンプルコード

```c
#include <stdio.h>
#include <string.h>

void vuln(char *str){
  char str2[] = "beef";
  char overflowme[16];
  printf("文字列を入力してください\n");
  scanf("%s", overflowme);
  if(strcmp(str, str2) == 0){
    printf("hacked!\n");
  }else{
    printf("failed!\n");
  }
}

int main(){
  char string[] = "fish";
  vuln(string);
}
```

- サンプルコードを以下コマンドでコンパイルする

```sh
gcc -m32 -fno-stack-protector bof_7-1.c -o test
```

### gdb-peda による解析

以下、点を確認する

- 入力した情報がメモリのどこから入っていくのか
- `str2`のアドレス

#### 実行結果

- `scanf`の実行前

```s
# ESP
0000| 0xbffff670 --> 0x80485e5 --> 0x68007325 ('%s')
0004| 0xbffff674 --> 0xbffff68b --> 0xe562f3b7
0008| 0xbffff678 --> 0xc2
0012| 0xbffff67c --> 0xb7eb9d56 (<handle_intel+102>:    test   eax,eax)
0016| 0xbffff680 --> 0xffffffff
0020| 0xbffff684 --> 0xbffff6ae --> 0xf6cb0804
0024| 0xbffff688 --> 0xb7e2fc34 --> 0x2aad
0028| 0xbffff68c --> 0xb7e562f3 (<__new_exitfn+19>:     add    ebx,0x179d0d)
0032| 0xbffff690 --> 0x0
0036| 0xbffff694 --> 0xc30000
# b
0040| 0xbffff698 --> 0x62000001
# eef
0044| 0xbffff69c --> 0x666565 ('eef')
0048| 0xbffff6a0 --> 0xbffff8a3 ("/home/vagrant/work/binary_analysis/s7/test")
0052| 0xbffff6a4 --> 0x2f ('/')
0056| 0xbffff6a8 --> 0xbffff6d8 --> 0x0
# EBP
```

- `scanf`の実行後

```s
0000| 0xbffff670 --> 0x80485e5 --> 0x68007325 ('%s')
0004| 0xbffff674 --> 0xbffff68b ('a' <repeats 22 times>)
0008| 0xbffff678 --> 0xc2
0012| 0xbffff67c --> 0xb7eb9d56 (<handle_intel+102>:    test   eax,eax)
0016| 0xbffff680 --> 0xffffffff
0020| 0xbffff684 --> 0xbffff6ae --> 0xf6cb0804
0024| 0xbffff688 --> 0x61e2fc34
0028| 0xbffff68c ('a' <repeats 21 times>)
0032| 0xbffff690 ('a' <repeats 17 times>)
0036| 0xbffff694 ('a' <repeats 13 times>)
0040| 0xbffff698 ("aaaaaaaaa")
0044| 0xbffff69c ("aaaaa")
0048| 0xbffff6a0 --> 0xbfff0061 --> 0x0
0052| 0xbffff6a4 --> 0x2f ('/')
0056| 0xbffff6a8 --> 0xbffff6d8 --> 0x0
```

- `str2`で定義されている`beef`は`0xbffff698`と`0xbffff69c`に分割されて格納されている
  - テキストでは、`beef`が 1stack に丸ごと格納されていたがおそらく難読化(攻撃防止)のためにコンパイラがこのような挙動をしている模様
- `0xbffff68b`から入力されていることが確認できる

## 関数のリターンアドレスの書き換え

プログラム中で呼び出す関数を任意に変更することにより、攻撃を仕掛けることを想定した演習を行う。

### 準備

以下、本演習で使用するサンプルコードになる。

```c
#include <stdio.h>

void pwn(){
  printf("hacked!\n");
}

void vuln(){
  char overflowme[48];
  scanf("%[^\n]", overflowme);
}

int main(){
  vuln();
  printf("failed!\n");
  return 0;
}
```

以下、コマンドをコンソール上で実行する

```sh
# root権限で実行する
sysctl -w kernel.randomize_va_space=0
gcc -m32 -fno-stack-protector bof_8-1.c -o test
```

### 演習

以下の点を確認する

- リターンアドレスまで、何文字入力をしてメモリを塗りつぶせば良いか特定する(**オフセットの特定**)
- 攻撃で呼び出したい関数はメモリ上のどこから始まっているか確認する

#### 演習結果

pattc, patto の使用方法などについてはメモの部分にまとめる。
実施した結果、以下のような解析結果となった。

- 入力部分からリターンアドレスまでのオフセットは 60byte 分
- 実行したい関数である`pwn`関数のアドレスは`0x804846d`

```s
gdb-peda$ patto 2AAH
2AAH found at offset: 60
gdb-peda$ p pwn
$2 = {<text variable, no debug info>} 0x804846d <pwn>
```

## Return to libc

プログラムが実行する関数のリターンアドレスを強制的に書き換えて、C 言語の標準ライブラリの libc を呼び出して攻撃を行うものを**Return to libc**と呼ぶ。

### 演習

この攻撃を行うにあたって、取得に必要な情報を以下に示す。

- リターンアドレスまでのオフセット
  - `pattc,patto`を使用
- system 関数のアドレス
  - `p`コマンドを使用
- "/bin/sh"が格納されているアドレス
  - **本来の攻撃では/bin/sh の文字列を対象の Stack へ配置して、それを呼び出して攻撃を行う**

```sh

```

## Memo

### アセンブラーのコンパイル

アセンブラーのコンパイルには nasm ライブラリを使用して、一度オブジェクトファイルに変換した後、gcc によりバイナリへコンパイルすることで実行できる。
以下、hi_6-1.s ファイルをバイナリへ変換する手順になる。

```bash
nasm -g -f elf32 hi_6-1.s
gcc hi_6-1.o -o test
```

### システムコール

コンソールへの出力などを実行する際、OS が提供しているシステムコール（API：規約）を介してそれを実現させる。
以下は画面へ文字を出力させるプログラム例である。

```s
global main

main:
  push 0x00006948
  mov eax, 0x4
  mov ebx, 0x1
  mov ecx, esp
  mov edx, 0x4
#   システムコールを呼び出すための特殊な命令
  int 0x80
  add esp, 0x4
```

- データの入出力を行うシステムコールは`write`で、汎用レジスタ`EAX`に 0x4 を指定することで呼び出すことができる。
  - EAX・・・システムコールの種別
- `EBX` ~ `EDX` はシステムコールの引数として認識され、それぞれ以下の意味合いとなっている
  - EBX・・・書き出し先のファイルディスクリプタ
  - ECX・・・書き出すデータが格納されたバッファへのポインタ
  - EDX・・・書き出すバイト数
- なお、EBX~EDX に指定するべき引数はシステムコール毎にことなるため、使用するシステムコール毎に調べて指定する。

#### 手順

各 os, cpu 毎でシステムコールの呼び出し方法は異なるため、それぞれに対応したものを調べる必要がある。

- [それぞれの OS でのシステムコール呼び出し参考手順](https://freestylewiki.xyz/fswiki/wiki.cgi?page=%E3%82%A2%E3%82%BB%E3%83%B3%E3%83%96%E3%83%A9%EF%BC%88%E3%82%B7%E3%82%B9%E3%83%86%E3%83%A0%E3%82%B3%E3%83%BC%E3%83%AB%EF%BC%89)
  - [文字コード](https://www.k-cube.co.jp/wakaba/server/ascii_code.html)
  - [32bit,64bit のシステムコール呼び出し方法まとめ](https://www.mztn.org/lxasm64/x86_x64_table.html)

### Hello World(32 ビット以上の文字列の出力)

```s
global main

main:
  # 00\n!dlroW
	push 0x000A2164
	push 0x6C726F57
  # スペース
	push 0x00000020
  # olleH
	push 0x0000006F
	push 0x6C6C6548
	mov eax, 0x4
	mov ebx, 0x1
	mov ecx, esp
	mov edx, 0x14
	int 0x80
```

- `Hello World!`を逆向きに並べてスタックに push
- 第二引数で指定した`ESP`から文字列の分だけのバイト数を第三引数に 16 進数で指定
- 学べたこと
  - リトルエンディアン方式
  - システムコール Write の使い方

### gdb-peda の pattc, patto コマンド使い方

```s
# TODO ↓文字列の生成
# 生成したい文字数を指定するが、取得したいアドレスまで届くように十分な文字数を指定する必要がある
gdb-peda$ pattc 70
'AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3'
...
[----------------------------------registers-----------------------------------]
EAX: 0x1
EBX: 0xb7fd0000 --> 0x1acda8
ECX: 0x13ac6a93
EDX: 0xbffff704 --> 0xb7fd0000 --> 0x1acda8
ESI: 0x0
EDI: 0x0
EBP: 0xbffff6d8 --> 0x0
ESP: 0xbffff6c0 --> 0xb7fd03c4 --> 0xb7fd11e0 --> 0x0
EIP: 0x80484a5 (<main+9>:       call   0x8048481 <vuln>)
EFLAGS: 0x286 (carry PARITY adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
   0x804849d <main+1>:  mov    ebp,esp
   0x804849f <main+3>:  and    esp,0xfffffff0
   0x80484a2 <main+6>:  sub    esp,0x10
=> 0x80484a5 <main+9>:  call   0x8048481 <vuln>
   0x80484aa <main+14>: mov    DWORD PTR [esp],0x804855e
   0x80484b1 <main+21>: call   0x8048330 <puts@plt>
   0x80484b6 <main+26>: mov    eax,0x0
   0x80484bb <main+31>: leave
Guessed arguments:
arg[0]: 0xb7fd03c4 --> 0xb7fd11e0 --> 0x0
[------------------------------------stack-------------------------------------]
0000| 0xbffff6c0 --> 0xb7fd03c4 --> 0xb7fd11e0 --> 0x0
0004| 0xbffff6c4 --> 0xb7fff000 --> 0x20f30
0008| 0xbffff6c8 --> 0x80484cb (<__libc_csu_init+11>:   add    ebx,0x1b35)
0012| 0xbffff6cc --> 0xb7fd0000 --> 0x1acda8
0016| 0xbffff6d0 --> 0x80484c0 (<__libc_csu_init>:      push   ebp)
0020| 0xbffff6d4 --> 0x0
0024| 0xbffff6d8 --> 0x0
0028| 0xbffff6dc --> 0xb7e3caf3 (<__libc_start_main+243>:       mov    DWORD PTR [esp],eax)
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
0x080484a5 in main ()
# TODO ↓文字列の入力箇所で生成した文字列を入力
gdb-peda$ n
AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3
[----------------------------------registers-----------------------------------]
EAX: 0x1
EBX: 0xb7fd0000 --> 0x1acda8
ECX: 0xb7fd18a4 --> 0x0
EDX: 0x1
ESI: 0x0
EDI: 0x0
EBP: 0x41416341 ('AcAA')
ESP: 0xbffff6c0 ("AAdAA3")
# TODO ↓呼び出している関数のvulnのリターンアドレスが以下のpattcで生成した文字列に塗りつぶされるためこの文字列をコピーする
EIP: 0x48414132 ('2AAH')
EFLAGS: 0x10286 (carry PARITY adjust zero SIGN trap INTERRUPT direction overflow)
[-------------------------------------code-------------------------------------]
Invalid $PC address: 0x48414132
[------------------------------------stack-------------------------------------]
0000| 0xbffff6c0 ("AAdAA3")
0004| 0xbffff6c4 --> 0xb7003341
0008| 0xbffff6c8 --> 0x80484cb (<__libc_csu_init+11>:   add    ebx,0x1b35)
0012| 0xbffff6cc --> 0xb7fd0000 --> 0x1acda8
0016| 0xbffff6d0 --> 0x80484c0 (<__libc_csu_init>:      push   ebp)
0020| 0xbffff6d4 --> 0x0
0024| 0xbffff6d8 --> 0x0
0028| 0xbffff6dc --> 0xb7e3caf3 (<__libc_start_main+243>:       mov    DWORD PTR [esp],eax)
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
Stopped reason: SIGSEGV
0x48414132 in ?? ()
# TODO ↓EIPで指定されているアドレスに格納されている文字列をpattoの引数に指定して実行すると入力部のアドレスからリターンアドレスまでの距離(オフセット)が取得できる
gdb-peda$ patto 2AAH
2AAH found at offset: 60
gdb-peda$ p pwn
$1 = {<text variable, no debug info>} 0x804846d <pwn>
gdb-peda$
```

- ブレークポイントは main 関数のみ設定して、そこから入力がある`vuln`の実行部まで`n`で一つずつ進める
  - ブレークポイントを`vuln`まで設定してしまうと EIP にリターンアドレスが入らない

### 関数のアドレスの特定

実行プロセスの中に存在する関数のアドレスは、`gdb-peda`対話モードの中で以下コマンドにより特定できる。`pwn`関数のアドレスを求める場合は以下のようにコマンドを実行する。

```s
gdb-peda$ p pwn
$2 = {<text variable, no debug info>} 0x804846d <pwn>
```
