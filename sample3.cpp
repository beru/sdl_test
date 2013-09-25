#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>


SDL_Surface* screen = NULL;                 /* ��� */
SDL_Surface* background_image = NULL;       /* �w�i�摜 */
SDL_Surface* letters_image = NULL;          /* �����摜 */

int counter = 0;                            /* ���t���[��1�ÂJ�E���g�A�b�v����J�E���^ */


/* (x, y)�Ƀe�L�X�g(�������A�����A�A���t�@�x�b�g�A�󔒂̂�)��\������B
 * printf �Ɠ����悤�ȏ��������ł���B
 */
void DrawText(int x, int y, const char* format, ...)
{
    int i;
    va_list ap;
    char buffer[256];

    va_start(ap, format);
    vsnprintf(buffer, 255, format, ap);
    va_end(ap);
    buffer[255] = '\0';
    for (i = 0; buffer[i] != '\0'; ++i) {
        SDL_Rect srcrect1, srcrect2;
        SDL_Rect destrect1 = { x + i*10, y };
        SDL_Rect destrect2 = { x + i*10 + 2, y + 2 };

        srcrect1.w = srcrect1.h = srcrect2.w = srcrect2.h = 10;
        if (isdigit(buffer[i])) {           /* ���� */
            srcrect1.x = (buffer[i] - '0') * 10;
            srcrect1.y = 20;
        } else if (isalpha(buffer[i])) {    /* �A���t�@�x�b�g */
            srcrect1.x = (toupper(buffer[i]) - 'A') * 10;
            srcrect1.y = 0;
        } else {                            /* ����ȊO�͋󔒂Ƃ݂Ȃ� */
            continue;
        }
        srcrect2.x = srcrect1.x;
        srcrect2.y = srcrect1.y + 10;
        SDL_BlitSurface(letters_image, &srcrect2, screen, &destrect2);
        SDL_BlitSurface(letters_image, &srcrect1, screen, &destrect1);
    }
}


/* �X�V���� */
void Update(void)
{
    ++counter;
    if (counter == 60 * 10) {              /* 10�b�o������I���C�x���g�𔭐������� */
        SDL_Event quit_event = { SDL_QUIT };
        SDL_PushEvent(&quit_event);
    }
}


/* �`�悷�� */
void Draw(void)
{
    /* �w�i��`�悷�� */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* ������60�t���[���̎����œ_�ł����ĕ\������ */
    if (counter / 30 % 2 == 0) {
        DrawText(200, 235, "PRESS ESC KEY TO EXIT");
    }
    /* �J�E���g�_�E����\������ */
    DrawText(270, 255, "%2d SEC", 10 - counter / 60);

    /* ��ʂ��X�V���� */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}


/* ����������B
 * ���������Ƃ���0���A���s�����Ƃ���-1��Ԃ��B
 */
int Initialize(void)
{
    /* SDL������������ */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL�̏������Ɏ��s���܂����F%s\n", SDL_GetError());
        return -1;
    }
    /* ��ʂ̃^�C�g����ύX���� */
    SDL_WM_SetCaption("My SDL Sample Game", NULL);
    /* ��ʂ����������� */
    screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
    if (screen == NULL) {
        fprintf(stderr, "��ʂ̏������Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    /* �摜��ǂݍ��� */
    background_image = IMG_Load("background.png");
    if (background_image == NULL) {
        fprintf(stderr, "�摜�̓ǂݍ��݂Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    letters_image = IMG_Load("letters.png");
    if (letters_image == NULL) {
        fprintf(stderr, "�摜�̓ǂݍ��݂Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    return 0;
}


/* ���C�����[�v */
void MainLoop(void)
{
    SDL_Event event;
    double next_frame = SDL_GetTicks();
    double wait = 1000.0 / 60;

    for (;;) {
        /* ���ׂẴC�x���g���������� */
        while (SDL_PollEvent(&event)) {
            /* QUIT �C�x���g���������邩�AESC �L�[�������ꂽ��I������ */
            if ((event.type == SDL_QUIT) ||
                (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
                return;
        }
        /* 1�b�Ԃ�60��Update�����悤�ɂ��� */
        if (SDL_GetTicks() >= next_frame) {
            Update();
            /* ���Ԃ��܂�����Ƃ���Draw���� */
            if (SDL_GetTicks() < next_frame + wait)
                Draw();
            next_frame += wait;
            SDL_Delay(0);
        }
    }
}


/* �I���������s�� */
void Finalize(void)
{
    SDL_FreeSurface(background_image);
    SDL_FreeSurface(letters_image);

    /* �I������ */
    SDL_Quit();
}


/* ���C���֐� */
int main(int argc, char* argv[])
{
    if (Initialize() < 0)
        return -1;
    MainLoop();
    Finalize();
    return 0;
}
