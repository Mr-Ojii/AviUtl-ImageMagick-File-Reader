#include <windows.h>

typedef struct {
    BOOL is_consecutive;        //連番か
    int digit;                  //連番だった場合の桁数
    int first_num;              //連番の最初の番号
    int last_num;               //連番の最後の番号
    char drive[_MAX_DRIVE];     //ドライブ名
    char dir[_MAX_DIR * 2];     //ディレクトリ名
    char fname[_MAX_FNAME * 2]; //ファイル名
    char ext[_MAX_EXT * 2];     //拡張子
    char path[_MAX_PATH * 2];   //パス
    BITMAPINFOHEADER *format;   //format
    DWORD format_size;            //format_size
} consecutive_image;
