<h1> KALPA_Chart_Converter </h1>

![KALPA](./fig/kalpa.jpg)

Table of Contents
- [English](#english)
  - [converter](#converter)
    - [How It Works](#how-it-works)
    - [Configuration](#configuration)
    - [Build \& Run](#build--run)
  - [frame\_viewer](#frame_viewer)
    - [Controls](#controls)
  - [thumbnail](#thumbnail)
  - [uploader/downloader](#uploaderdownloader)
- [Disclaimer](#disclaimer)
- [日本語](#日本語)
  - [converter](#converter-1)
    - [原理](#原理)
    - [設定](#設定)
    - [コンパイル・実行](#コンパイル実行)
  - [frame\_viewer](#frame_viewer-1)
    - [コンパイル・実行](#コンパイル実行-1)
    - [操作](#操作)
    - [thumbnail](#thumbnail-1)
    - [uploader/downloader](#uploaderdownloader-1)
- [免責事項](#免責事項)


# English
This software generates chart diagrams for the rhythm game [KALPA](http://www.queseragames.com/#Main) from screen-recorded videos. Sample outputs are provided in the ``samples`` directory. Additionally, it includes several utility tools: <br>
- A frame viewer for stepping through video frames and inspecting pixel coordinates and colors.
- A thumbnail extractor for saving song thumbnail images from the song selection screen.
- Command-line tools for uploading and downloading files to/from Google Drive.

## converter
Main program: it generates chart diagrams.
### How It Works
The program extracts frames of a video at specified intervals, and then applies a perspective transform to map a trapezoidal region (``src_pts``) into a rectangle. By stacking these transformed rectangular images, it can produce the whole chart diagram.

### Configuration
- First-time setup: You must manually configure the trapezoid vertex coordinates in ``constants.cpp (src_pts)``. To decide these constants, use the frame_viewer tool (described later) and hover your mouse over each vertex.
- ``config.json``: Configure this file per chart. Important parameters are listed below:
    - jpeg_quality: Output JPEG quality (1–100).
    - default_step: Default frame extraction interval. Set this parameter so that charts without speed changes convert correctly.
    - adjust_step_enabled: false: always use default_step. true: use custom step intervals defined in "step".
    - difficulty: Specify chart difficulty as a string. Use one of the categories from game version 3.2.13 (table below), or "EXTRA" for Stage Mode special charts.
    - step: Custom frame intervals, formatted as "start_frame": interval. Useful for charts with speed changes. Frame numbers can be checked via frame_viewer.
  
<br>
Difficulty Categories:

| Thumb Mode | Multi Finger Mode |
|:-----------|:------------|
| NORMAL       | S.HARD |
| HARD     | S.HARD+ |
| HARD+     | CHAOS |
| ABYSS   | COSMOS |

### Build & Run
Build
```bash
cd converter
mkdir build && cd build
cmake ..
make -j5
```
Run
```bash
./chart_converter
```

## frame_viewer
Tool for browsing video frame-by-frame and inspecting pixel coordinates and colors.
- Build & Run
<br>

Build
```bash
cd utils/frame_viewer
mkdir build && cd build
cmake ..
make
```
Run (example)
```bash
Usage: ./frame_viewer <video file>
```
Options: ``-s start_frame`` (specify starting frame)

### Controls
| Key | Function |
|:-----------|:------------|
| Space       | Play/Pause |
| n or Right Arrow     | Next frame |
| p or Left Arrow     | Previous frame |
| f         | Forward 100 frames |
| b         | Backward 100 frames |
| s         | Save current frame |
| q or Esc | Quit |

## thumbnail
Extracts song thumbnail images from KALPA’s song selection screenshot. The extraction area can be defined by the constants in ``thumbnail.cpp``:
```cpp
  const int w_edge = 1141;
  const int h_edge = 384;
  const int size = 599;
```
(w_edge, h_edge): pixel coordinates of the top-left corner of the thumbnail (check with ``check_pixels.cpp``). size: length of one side of the thumbnail.

- C++
```bash
g++ check_pixels.cpp -o check_pixels `pkg-config --cflags --libs opencv4`
```
```bash
g++ thumbnail.cpp -o thumbnail `pkg-config --cflags --libs opencv4`
```
- Python

(May not be mandatory) Create a virtual environment as follows, or install directly in the current environment.
```bash
sudo apt install python3.12-venv
source myenv/bin/activate
python3 -m venv myenv
python3 -m pip install opencv-python
```
Remember to run ``source myenv/bin/activate`` each time you open a terminal to activate the virtual environment.
Example run:
```bash
python3 thumbnail.py ../../videos/IMG_1522.PNG kitsubebi_py.png
```

## uploader/downloader
Command-line tools for uploading and downloading files to/from Google Drive. Before you use, enable the Google Drive API and create OAuth 2.0 client credentials to obtain ``credentials.json``. Place this file in ``utils/hidden`` and keep it private. A ``token.json`` will also be generated on first use.
<br>

Create the ``hidden/folder_path.json`` to designate your Google Drive folders:
``` json:folder_path.json
{
    "upload_folderID": <YOUR_UPLOAD_FOLDER_ID>,
    "download_folderID": <YOUR_DOWNLOAD_FOLDER_ID>
}
```
(Assuming Go environment is installed) Use the provided shell script to set up necessary modules:
```bash
cd utils/
./setup.sh
```

# Disclaimer
**By using this software, you agree to the following terms:**
- **This software is not intended for commercial use.**
- **Copyright belongs to the author.**
- **The author assumes no responsibility for any damages resulting from use of this software. Use at your own risk.**
- **Do not redistribute generated outputs to public websites or platforms.**
- **This software may be modified or taken down without notice.**

----------------------------------

# 日本語
 リズムゲーム[KALPA](http://www.queseragames.com/#Main)の譜面平面図を、端末の画面録画から作成します。samplesディレクトリに変換例を示します。ご参照ください。また、周辺ツールとして以下のコードを含みます:
    - 動画をフレーム単位でコマ送り/任意のピクセルについて、その座標と色を取得
    - 選曲画面から楽曲のサムネイル画像を取得して保存
    - Google Driveへのアップロード、ダウンロードのコマンドライン化

## converter
- 譜面平面図作成プログラム。
### 原理
動画中のフレームを設定した間隔で取り出したのち、それらの画像上の台形領域``src_pts``を長方形になるように射影変換します。そして変換された長方形の画像を順に積み上げていくことで譜面平面図が作成されます。

### 設定
初回のみ、``constants.cpp``の``src_pts``の数値を設定する必要があります。後述のツール``frame_viewer``を使って動画を確認し、切り取る台形の各頂点のピクセル座標を調べてください(該当の点にマウスカーソルをかざす)。
<br>

``config.json``は作成する譜面に応じて適切に設定します。注意しなければならないパラメータを以下に挙げます 。
- jpeg_quality: 生成されるjpeg画像ファイルの画質(1-100までの整数値)。
- default_step: 切り取りフレーム間隔の基準。譜面に速度変化がないときに最も適切に変換されるような整数値を設定します。
- adjust_step_enabled: falseのとき切り取りフレーム間隔をdefault_stepの一定値とし、trueのときカスタマイズした間隔が適用されます。
- difficulty: 譜面の難易度区分を文字列で指定します。ゲームアプリのバージョン3.2.13で用意されている難易度区分(下の表)のいずれかを選択します。これに加えて、Stage Modeで用意されている特別譜面 EXTRA を選ぶこともできます。
- step: カスタマイズした切り取り間隔を記述します。"適用を開始するフレーム番号": 切り取り間隔"とします。フレーム番号は後述のツール frame_viewerから参照できます。速度変化を伴う譜面を変換したいときに使うことになります。

難易度区分一覧:
| Thumb Mode | Multi Finger Mode |
|:-----------|:------------|
| NORMAL       | S.HARD |
| HARD     | S.HARD+ |
| HARD+     | CHAOS |
| ABYSS   | COSMOS |

### コンパイル・実行
コンパイル
```bash
cd converter
mkdir build && cd build
cmake ..
make -j5
```
実行
```bash
./chart_converter
```

## frame_viewer
動画をフレーム単位でコマ送りしたり、特定のピクセルの座標と色を確認したりできます。
### コンパイル・実行
コンパイル
```bash
cd utils/frame_viewer
mkdir build && cd build
cmake ..
make
```
実行
```bash
Usage: ./frame_viewer <video file>
```
オプション: ``-s start_frame`` (開始フレーム指定)

### 操作
| Key | 機能 |
|:-----------|:------------|
| スペース       | 再生/停止 |
| n または 右矢印     | 1フレーム進める |
| p または 左矢印     | 1フレーム巻き戻し |
| f         | 100フレーム進める |
| b         | 100フレーム巻き戻し |
| s         | 現在のフレームを保存 |
| q または Esc | 終了 |

### thumbnail
KALPAの選曲のスクリーンショットから楽曲サムネイルを切り出して保存します。切り出す領域は```thumbnail.cpp```の定数
```cpp
  const int w_edge = 1141;
  const int h_edge = 384;
  const int size = 599;
```
で指定されます。必要に応じて変更してください。
- (w_edge, h_edge)は``check_pixels.cpp``を実行して、サムネイルの左上隅にマウスポインタを合わせたときのピクセル座標です。sizeはサムネイルの一辺の長さです。

- C++
```bash
g++ check_pixels.cpp -o check_pixels `pkg-config --cflags --libs opencv4`
```

```bash
g++ thumbnail.cpp -o thumbnail `pkg-config --cflags --libs opencv4`
```

- Python
事前に仮想環境を作成する場合は以下を参考にしてください。現在の環境のままpip installできるならばそれでも構いません。
```bash
sudo apt install python3.12-venv
source myenv/bin/activate
python3 -m venv myenv
python3 -m pip install opencv-python
```
なお、ターミナルを立ち上げるたびに``source myenv/bin/activate``を実行して仮想環境を有効化するのを忘れないでください。

実行例: 
```bash
python3 thumbnail.py ../../videos/IMG_1522.PNG kitsubebi_py.png
```

### uploader/downloader
Google Driveへのアップロード、ダウンロードをコマンドラインで行うツールです。事前にGoogle Drive APIを有効化し、OAuth 2.0クライアントIDを作成して``credentials.json``を取得しておく必要があります。このファイルは``utils/hidden``に配置し、他者に見られないようにしてください。また、初回は``token.json``も生成されます。

uploader、downloaderそれぞれの``hidden/folder_path.json``を自分のGoogle Drive環境に合わせて修正してください。
``` json:folder_path.json
{
    "upload_folderID": <YOUR_UPLOAD_FOLDER_ID>,
    "download_folderID": <YOUR_DOWNLOAD_FOLDER_ID>
}
```

<br>

(Go実行環境をインストールしているとして)モジュールのセットアップには専用のシェルスクリプトを利用すると便利です:
```bash
cd utils/
./setup.sh
```

# 免責事項
**本ソフトウェアの利用は、以下の条件に同意したものとみなします:**
- **本ソフトウェアは商用利用を目的とはしておりません。**
- **本ソフトウェアの著作権は作者に帰属します。**
- **本ソフトウェアの利用により生じた如何なる損害についても、作者は一切の責任を負いません。自己責任でご利用ください。**
- **本ソフトウェアによる生成物の一般サイト等への二次頒布はお控えください。**
- **本ソフトウェアは予告なく改変、公開停止されることがあります。**