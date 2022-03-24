#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "input.h"
#include "main.h"
#include <MagickWand/MagickWand.h>

#define PLUGIN_NAME "ImageMagick File Reader"

static MagickWand* magick_wand = NULL;

INPUT_PLUGIN_TABLE input_plugin_table = {
    INPUT_PLUGIN_FLAG_VIDEO,
    PLUGIN_NAME,
    "AllFile (*.*)\0*.*\0",
    PLUGIN_NAME " by Mr-Ojii",
    func_init,
    func_exit,
    func_open,
    func_close,
    func_info_get,
    func_read_video,
    NULL,                       //AUDIOは対応してない
    NULL,                       //すべてキーフレームなのでNULL
    NULL,                       //configする箇所がない..
};

EXTERN_C INPUT_PLUGIN_TABLE __declspec(dllexport) * __stdcall GetInputPluginTable(void)
{
    return &input_plugin_table;
}

BOOL func_init( void )
{
    MagickWandGenesis();
    magick_wand = NewMagickWand();
    return TRUE;
}

BOOL func_exit( void )
{
    magick_wand = DestroyMagickWand(magick_wand);
    MagickWandTerminus();
    return TRUE;
}

//CP932からCP65001(UTF-8)に変換する関数
inline char* string_932to65001(char* str)
{
    int size = MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, NULL, 0);
    if (size == 0)
        return NULL;
    wchar_t* wstr = malloc(size * sizeof(wchar_t));
    if (!wstr)
        return NULL;

    if (MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, wstr, size) == 0) {
        free(wstr);
        return NULL;
    }

    int size_r = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (size_r == 0) {
        free(wstr);
        return NULL;
    }
    char* ret = malloc(size_r);
    if (!ret) {
        free(wstr);
        return NULL;
    }
    if (WideCharToMultiByte(CP_UTF8, 0, wstr, size, ret, size_r, NULL, NULL) == 0) {
        free(ret);
        free(wstr);
        return NULL;
    }

    return ret;
}

INPUT_HANDLE func_open(LPSTR file)
{
    char* u_file = string_932to65001(file);
    if (u_file == NULL)
        return FALSE;
    //とりあえず、開けることを確認する
    MagickBooleanType state = MagickReadImage(magick_wand, u_file);
    free(u_file);
    //開けなかったか、単一フレームじゃないとき
    if (state == MagickFalse || MagickGetNumberImages(magick_wand) != 1)
        return NULL;

    //BMPv3だと、アルファチャンネル系が正常に動作しないので、
    //BMPv5から変換して、強引にアルファチャンネルに対応する
    if (MagickSetFormat(magick_wand, "BMP") == MagickFalse)
        return NULL;

    size_t memsize;
    char* image = (char*)MagickGetImageBlob(magick_wand, &memsize);
    if (image == NULL)
        return NULL;

    //開いたら、閉じる
    ClearMagickWand(magick_wand);

    consecutive_image* hp = malloc(sizeof(consecutive_image));
    if (!hp) {
        MagickRelinquishMemory(image);
        return NULL;
    }

    hp->format = malloc(sizeof(BITMAPINFOHEADER));
    if (hp->format) {
        BITMAPFILEHEADER* bfh = (BITMAPFILEHEADER*)image;
        BITMAPV5HEADER* bih = (BITMAPV5HEADER*)(image + sizeof(BITMAPFILEHEADER));
        hp->format_size = sizeof(BITMAPINFOHEADER);
        hp->format->biBitCount = bih->bV5BitCount;
        hp->format->biClrImportant = bih->bV5ClrImportant;
        hp->format->biClrUsed = bih->bV5ClrUsed;
        hp->format->biCompression = BI_RGB;
        hp->format->biHeight = bih->bV5Height;
        hp->format->biPlanes = bih->bV5Planes;
        hp->format->biSize = sizeof(BITMAPINFOHEADER);
        hp->format->biSizeImage = bih->bV5SizeImage;
        hp->format->biWidth = bih->bV5Width;
        hp->format->biXPelsPerMeter = bih->bV5XPelsPerMeter;
        hp->format->biYPelsPerMeter = bih->bV5YPelsPerMeter;
    }
    else {
        free(hp);
        hp = NULL;
        MagickRelinquishMemory(image);
        return NULL;
    }

    hp->is_consecutive = FALSE;


    unsigned int digit = 0, first_num = 0, last_num = 0;
    char drive[_MAX_DRIVE], dir[_MAX_DIR * 2], fname[_MAX_FNAME * 2], ext[_MAX_EXT * 2];
    _splitpath(file, drive, dir, fname, ext);
    int i;
    for (i = strlen(fname) - 1;; i--)
    {
        if (fname[i] >= '0' && fname[i] <= '9')
        {
            digit++;
        }
        else
        {
            break;
        }
    }
    i++;
    if (digit != 0)
    {
        first_num = atoi(&fname[i]);
        last_num = first_num;

        WIN32_FIND_DATA win32fd;
        char fmt[20];
        char path[_MAX_PATH * 2];
        fname[i] = '\0';
        sprintf((char*)&fmt, "%%s%%s%%s%%0%dd%%s", digit);
        while (TRUE)
        {
            sprintf((char*)&path, fmt, drive, dir, fname, ++last_num, ext);
            HANDLE fHandle = FindFirstFile(path, &win32fd);
            if (fHandle != INVALID_HANDLE_VALUE)
                FindClose(fHandle);
            else
                break;
        }
        last_num--;
        hp->is_consecutive = TRUE;
        hp->digit = digit;
        hp->first_num = first_num;
        hp->last_num = last_num;
    }
    strcpy(hp->path, file);
    strcpy(hp->drive, drive);
    strcpy(hp->dir, dir);
    strcpy(hp->fname, fname);
    strcpy(hp->ext, ext);

    MagickRelinquishMemory(image);

    return hp;
}
BOOL func_close(INPUT_HANDLE ih)
{
    consecutive_image* hp = ih;
    if( hp )
    {
        if( hp->format )
            free( hp->format );
        free( hp );
    }
    return TRUE;
}

BOOL func_info_get(INPUT_HANDLE ih, INPUT_INFO *iip)
{
    consecutive_image* hp = ih;
    iip->flag = INPUT_INFO_FLAG_VIDEO | INPUT_INFO_FLAG_VIDEO_RANDOM_ACCESS;

    //「JPEG/PNG File Reader」と同じく、rate scaleを0にする(黒魔術)
    iip->rate = 0;
    iip->scale = 0;

    //連番だったら、「フレーム数=連番の数」
    if (hp->is_consecutive)
        iip->n = hp->last_num - hp->first_num + 1;
    else
        iip->n = 1;

    iip->format = hp->format;
    iip->format_size = hp->format_size;
    iip->handler = 0;

    return TRUE;
}

int func_read_video(INPUT_HANDLE ih, int frame, void *buf)
{
    consecutive_image* hp = ih;
    char path[_MAX_PATH * 2];

    //連番だったら、要求フレーム数+first_numで、アクセスする画像番号を
    //それ以外だったら、そのまま
    if (hp->is_consecutive) {
        char fmt[20];
        sprintf((char*)&fmt, "%%s%%s%%s%%0%dd%%s", hp->digit);
        sprintf((char*)&path, fmt, hp->drive, hp->dir, hp->fname, frame + hp->first_num, hp->ext);
    }
    else {
        strcpy(path, hp->path);
    }

    char* u_file = string_932to65001(path);
    if (u_file == NULL)
        return 0;
    MagickBooleanType state = MagickReadImage(magick_wand, u_file);
    free(u_file);
    if (state == MagickFalse)
        return 0;

    if (MagickSetFormat(magick_wand, "BMP") == MagickFalse)
        return 0;

    size_t memsize;
    char* image = (char*)MagickGetImageBlob(magick_wand, &memsize);
    if (image == NULL)
        return 0;

    //開いたら、閉じる
    ClearMagickWand(magick_wand);

    BITMAPFILEHEADER* bfh = (BITMAPFILEHEADER*)image;
    int size = min(bfh->bfSize - bfh->bfOffBits, hp->format->biSizeImage);
    memcpy(buf, image + bfh->bfOffBits, size);

    MagickRelinquishMemory(image);

    return size;
}
