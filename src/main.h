#include <windows.h>

typedef struct {
    BOOL is_consecutive;        //�A�Ԃ�
    int digit;                  //�A�Ԃ������ꍇ�̌���
    int first_num;              //�A�Ԃ̍ŏ��̔ԍ�
    int last_num;               //�A�Ԃ̍Ō�̔ԍ�
    char drive[_MAX_DRIVE];     //�h���C�u��
    char dir[_MAX_DIR * 2];     //�f�B���N�g����
    char fname[_MAX_FNAME * 2]; //�t�@�C����
    char ext[_MAX_EXT * 2];     //�g���q
    char path[_MAX_PATH * 2];   //�p�X
    BITMAPINFOHEADER *format;   //format
    DWORD format_size;            //format_size
} consecutive_image;
