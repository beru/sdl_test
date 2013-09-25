#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>


SDL_Surface* screen = NULL;                 /* ��� */
SDL_Surface* background_image = NULL;       /* �w�i�摜 */


/* �X�V���� */
void Update(void)
{
    /* TODO: �����ɃQ�[���̏�Ԃ��X�V���鏈�������� */
}


/* �`�悷�� */
void Draw(void)
{
    /* �w�i��`�悷�� */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* TODO: �����ɕ`�揈�������� */

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

    /* TODO: �����ŉ摜�≹����ǂݍ��� */
    background_image = IMG_Load("background.png");
    if (background_image == NULL) {
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
    /* TODO: �����ŉ摜�≹����������� */
    SDL_FreeSurface(background_image);

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
