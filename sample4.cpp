#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>


/* ���̓f�[�^ */
typedef struct {
    int left;
    int up;
    int right;
    int down;
    int button1;
    int button2;
} InputData;


InputData current_input, prev_input;        /* ���̓f�[�^ */

SDL_Surface* screen = NULL;                 /* ��� */
SDL_Surface* background_image = NULL;       /* �w�i�摜 */
SDL_Surface* onpu_image = NULL;             /* ���� */

double x, y;                                /* �����̍��W */


/* ���̓f�[�^���X�V���� */
void UpdateInput(void)
{
    Uint8* keys = SDL_GetKeyState(NULL);

    prev_input = current_input;
    current_input.left = keys[SDLK_LEFT] | keys[SDLK_h];        /* [��], [H] */
    current_input.up = keys[SDLK_UP] | keys[SDLK_k];            /* [��], [K] */
    current_input.right = keys[SDLK_RIGHT] | keys[SDLK_l];      /* [��], [L] */
    current_input.down = keys[SDLK_DOWN] | keys[SDLK_j];        /* [��], [J] */
    current_input.button1 = keys[SDLK_LSHIFT] | keys[SDLK_z];   /* ��Shift, [Z] */
    current_input.button2 = keys[SDLK_LCTRL] | keys[SDLK_x];    /* ��Ctrl, [X] */
}


/* �X�V���� */
void Update(void)
{
    int speed = 6;

    UpdateInput();

    /* �{�^��1��������Ă���Ƃ��͒ᑬ */
    if (current_input.button1)
        speed = 2;
    /* �{�^��2�������ꂽ��(0, 0)�Ɉړ� */
    if (current_input.button2 && !prev_input.button2)   /* �����ꂽ�u�Ԃ̂� */
        x = y = 0;

    /* ���͂ɂ��������Ĉړ����� */
    if (current_input.left)
        x -= speed;
    if (current_input.right)
        x += speed;
    if (current_input.up)
        y -= speed;
    if (current_input.down)
        y += speed;

    /* ��ʊO�ɏo�Ȃ��悤�ɂ��� */
    if (x < 0)
        x = 0;
    else if (x > 640 - 64)
        x = 640 - 64;
    if (y < 0)
        y = 0;
    else if (y > 480 - 64)
        y = 480 - 64;
}


/* �`�悷�� */
void Draw(void)
{
    SDL_Rect destrect = { x, y };

    /* �w�i��`�悷�� */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* ������`�悷�� */
    SDL_BlitSurface(onpu_image, NULL, screen, &destrect);

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
    onpu_image = IMG_Load("onpu.png");
    if (onpu_image == NULL) {
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
    SDL_FreeSurface(onpu_image);

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
