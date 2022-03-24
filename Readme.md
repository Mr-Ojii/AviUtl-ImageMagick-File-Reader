# AviUtl-ImageMagick-File-Reader
AviUtl 0.99k 以降用、ImageMagickを使用した、入力プラグインです。

リポジトリ名は`AviUtl-ImageMagick-File-Reader`ですが、  
プラグイン名としては`ImageMagick File Reader`です。

ImageMagickが対応している画像ファイルを読み込みます。  
連番が検出された場合、連番画像として読み込みます。

バイナリの頒布はしないので、使用したくなった場合はご自身でビルドして使用してください。

## ビルド確認環境
+ Windows11 21H2
+ Visual Studio Community 2022
+ ImageMagick-7.1.0-28-Q16-HDRI-x86-dll

## ビルド方法
1. ImageMagick DLL系(x86)のインストール
2. CMake(3.20.0以降)のインストール
3. CMakeで`src`フォルダをソースフォルダとして指定し、Win32用でImageMagickのinclude/libのパスを指定しConfigure、Generate(VS2022推奨)
4. 指定したコンパイラでビルド

## なぜバイナリの頒布をしない
+ 導入がいろいろと面倒
  1. `Visual C++ 再頒布可能パッケージ 2015-`のインストール
  2. ImageMagickのインストール、または頒布者がすべての依存DLL群を同梱する
    * 使用者がImageMagickをインストールする場合
      + インストールするものを間違えた場合、動作しない可能性があるが、バージョンがありすぎるので初心者に向かない
    * 頒布者がすべての依存DLL群を同梱するとなった場合
      + ざっと20-30ものDLL群を同梱するのでAviUtlのディレクトリが汚れる
