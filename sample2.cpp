#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define MAX_PARTICLE_COUNT 512


SDL_Surface* screen = NULL;                 /* ��� */
SDL_Surface* background_image = NULL;       /* �w�i�摜 */
SDL_Surface* particle_image = NULL;         /* ���q�̉摜 */


/* ���q */
typedef struct {
    double x, y;        /* ���W */
    double vx, vy;      /* ���x */
    int alive;          /* 0: ���S, 1: ���� */
    int type;           /* 0: ���������q, 1: �傫�����q */
    double frame;       /* �A�j���[�V�����͍����t���[���ڂ� */
} Particle;

/* ���q�̔z�� */
Particle particles[MAX_PARTICLE_COUNT];


/* lower �ȏ� upper �ȉ��̗�����Ԃ� */
double Random(double lower, double upper)
{
   return ((double)rand() / RAND_MAX) * (upper - lower) + lower;
}


/* ���q�����B
 * ���ۂɂ͗��q�̔z�񂩂玀��ł闱�q��������Ă���
 * ����𐶂��Ԃ点�āA�p�����[�^�[��ݒ肷��B
 */
int CreateParticle(double x, double y, double vx, double vy, int type)
{
    int i;

    /* ����ł闱�q��T�� */
    for (i = 0; i < MAX_PARTICLE_COUNT; ++i) {
        if (!particles[i].alive)
            break;
    }
    if (i < MAX_PARTICLE_COUNT) {   /* �������� */
        particles[i].alive = 1;
        particles[i].x = x;
        particles[i].y = y;
        particles[i].vx = vx;
        particles[i].vy = vy;
        particles[i].type = type;
        particles[i].frame = 0;
        return i;
    } else {                        /* ������Ȃ����� */
        return -1;
    }
}


/* index�Ԗڂ̗��q���X�V���� */
void UpdateParticle(int index)
{
    particles[index].vy += 0.4;
    particles[index].x += particles[index].vx;
    particles[index].y += particles[index].vy;

    particles[index].frame += 0.5;
    if (particles[index].frame >= 4)
        particles[index].frame = 0;

    /* ��ʉ��ɏo�Ă����玀�S */
    if (particles[index].y > 480)
        particles[index].alive = 0;
}


/* �X�V���� */
void Update(void)
{
    int i;

    /* �����Ă���S���q���X�V���� */
    for (i = 0; i < MAX_PARTICLE_COUNT; ++i) {
        if (particles[i].alive)
            UpdateParticle(i);
    }

    CreateParticle(320, 240, Random(-5, 5), Random(-8, -16), 0);
    CreateParticle(312, 232, Random(-5, 5), Random(-8, -16), 1);
}


/* ���q��`�悷�� */
void DrawParticle(int index)
{
    SDL_Rect srcrect;
    SDL_Rect destrect = { particles[index].x, particles[index].y };

    switch (particles[index].type) {
    case 0:
        srcrect.x = (int)particles[index].frame * 16;
        srcrect.y = 0;
        srcrect.w = srcrect.h = 16;
        break;
    case 1:
        srcrect.x = (int)particles[index].frame * 32;
        srcrect.y = 16;
        srcrect.w = srcrect.h = 32;
        break;
    }

    SDL_BlitSurface(particle_image, &srcrect, screen, &destrect);
}


/* �`�悷�� */
void Draw(void)
{
    int i;

    /* �w�i��`�悷�� */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* �����Ă���S���q��`�悷�� */
    for (i = 0; i < MAX_PARTICLE_COUNT; ++i) {
        if (particles[i].alive)
            DrawParticle(i);
    }

    /* ��ʂ��X�V���� */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}


/* ����������B
 * ���������Ƃ���0���A���s�����Ƃ���-1��Ԃ��B
 */
int Initialize(void)
{
    int i;

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
    background_image = IMG_Load("background2.png");
    if (background_image == NULL) {
        fprintf(stderr, "�摜�̓ǂݍ��݂Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    particle_image = IMG_Load("particle.png");
    if (particle_image == NULL) {
        fprintf(stderr, "�摜�̓ǂݍ��݂Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    /* ���������������� */
    srand(time(NULL));

    /* ���q�̔z������������� */
    for (i = 0; i < MAX_PARTICLE_COUNT; ++i)
        particles[i].alive = 0;

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
    SDL_FreeSurface(particle_image);

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
